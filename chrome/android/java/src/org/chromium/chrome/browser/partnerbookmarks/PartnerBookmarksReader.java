// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.partnerbookmarks;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.AsyncTask;

import org.chromium.base.ContextUtils;
import org.chromium.base.Log;
import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.metrics.RecordHistogram;
import org.chromium.chrome.browser.AppHooks;

import java.util.HashSet;
import java.util.LinkedHashMap;

import javax.annotation.concurrent.GuardedBy;

/**
 * Reads bookmarks from the partner content provider (if any).
*/
public class PartnerBookmarksReader {
    private static final String TAG = "PartnerBMReader";

    static final String LAST_EMPTY_READ_PREFS_NAME = "PartnerBookmarksReader.last_empty_read";

    private static boolean sInitialized;
    private static boolean sForceDisableEditing;

    /** Root bookmark id reserved for the implied root of the bookmarks */
    static final long ROOT_FOLDER_ID = 0;

    /** ID used to indicate an invalid bookmark node. */
    static final long INVALID_BOOKMARK_ID = -1;

    /** Storage for failed favicon retrieval attempts to throttle future requests. **/
    private PartnerBookmarksFaviconThrottle mFaviconThrottle;

    // JNI c++ pointer
    private long mNativePartnerBookmarksReader;

    /** The context (used to get a ContentResolver) */
    protected Context mContext;

    // Favicons are loaded asynchronously so we need to keep track of how many are currently in
    // progress, as well as whether or not we've finished reading bookmarks from this class so we
    // don't end up shutting the bookmark reader down prematurely.
    private final Object mProgressLock = new Object();
    @GuardedBy("mProgressLock")
    private int mNumFaviconsInProgress = 0;
    @GuardedBy("mProgressLock")
    private boolean mShutDown = false;
    private boolean mFinishedReading;

    /**
     * A callback used to indicate success or failure of favicon fetching when retrieving favicons
     * from cache or server.
     */
    private interface FetchFaviconCallback {
        @CalledByNative("FetchFaviconCallback")
        void onFaviconFetched(@FaviconFetchResult int result);

        @CalledByNative("FetchFaviconCallback")
        void onFaviconFetch();
    }

    /**
     * Creates the instance of the reader.
     * @param context A Context object.
     */
    public PartnerBookmarksReader(Context context) {
        mContext = context;
        mNativePartnerBookmarksReader = nativeInit();
        initializeAndDisableEditingIfNecessary();
    }

    /**
     * Asynchronously read bookmarks from the partner content provider
     */
    public void readBookmarks() {
        if (mNativePartnerBookmarksReader == 0) {
            assert false : "readBookmarks called after nativeDestroy.";
            return;
        }
        new ReadBookmarksTask().execute();
    }

    /**
     * Called when the partner bookmark needs to be pushed.
     * @param url       The URL.
     * @param title     The title.
     * @param isFolder  True if it's a folder.
     * @param parentId  NATIVE parent folder id.
     * @param favicon   .PNG blob for icon; used if no touchicon is set.
     * @param touchicon .PNG blob for icon.
     * @return          NATIVE id of a bookmark
     */
    private long onBookmarkPush(String url, String title, boolean isFolder, long parentId,
            byte[] favicon, byte[] touchicon) {
        FetchFaviconCallback callback = new FetchFaviconCallback() {
            @Override
            public void onFaviconFetched(@FaviconFetchResult int result) {
                RecordHistogram.recordEnumeratedHistogram(
                        "PartnerBookmark.FaviconThrottleFetchResult", result,
                        FaviconFetchResult.UMA_BOUNDARY);
                synchronized (mProgressLock) {
                    mFaviconThrottle.onFaviconFetched(url, result);
                    --mNumFaviconsInProgress;
                    if (mNumFaviconsInProgress == 0 && mFinishedReading) {
                        shutDown();
                    }
                }
            }

            @Override
            public void onFaviconFetch() {
                synchronized (mProgressLock) {
                    ++mNumFaviconsInProgress;
                }
            }
        };
        return nativeAddPartnerBookmark(mNativePartnerBookmarksReader, url, title, isFolder,
                parentId, favicon, touchicon,
                mFaviconThrottle.shouldFetchFromServerIfNecessary(url), callback);
    }

    /**
     * Sets our finished reading flag, and if there is no work being done on the native side, shuts
     * down the bookmark reader.
     */
    protected void onBookmarksRead() {
        mFinishedReading = true;
        synchronized (mProgressLock) {
            if (mNumFaviconsInProgress == 0) {
                shutDown();
            }
        }
    }

    /** Notifies the reader is complete and partner bookmarks should be submitted to the shim. */
    protected void shutDown() {
        synchronized (mProgressLock) {
            if (mShutDown) return;

            nativePartnerBookmarksCreationComplete(mNativePartnerBookmarksReader);
            nativeDestroy(mNativePartnerBookmarksReader);
            mNativePartnerBookmarksReader = 0;
            if (mFaviconThrottle != null) {
                mFaviconThrottle.commit();
            }
            mShutDown = true;
        }
    }

    /** Handles fetching partner bookmarks in a background thread. */
    private class ReadBookmarksTask extends AsyncTask<Void, Void, Void> {
        private final Object mRootSync = new Object();

        private void handleZeroBookmark() {
            SharedPreferences.Editor editor = ContextUtils.getAppSharedPreferences().edit();
            editor.putLong(LAST_EMPTY_READ_PREFS_NAME, System.currentTimeMillis());
            editor.apply();

            Log.w(TAG,
                    "Obtained zero partner bookmarks. "
                            + "Will skip reading partner bookmarks for a while.");
        }

        @Override
        protected Void doInBackground(Void... params) {
            if (mFaviconThrottle == null) {
                // Initialize the throttle here since we need to load shared preferences on the
                // background thread as well.
                mFaviconThrottle = new PartnerBookmarksFaviconThrottle(mContext);
            }
            PartnerBookmark.BookmarkIterator bookmarkIterator =
                    AppHooks.get().getPartnerBookmarkIterator();
            RecordHistogram.recordBooleanHistogram(
                    "PartnerBookmark.Null", bookmarkIterator == null);
            if (bookmarkIterator == null) {
                handleZeroBookmark();
                return null;
            }

            // Get a snapshot of the bookmarks.
            LinkedHashMap<Long, PartnerBookmark> idMap = new LinkedHashMap<Long, PartnerBookmark>();
            HashSet<String> urlSet = new HashSet<String>();

            PartnerBookmark rootBookmarksFolder = createRootBookmarksFolderBookmark();
            idMap.put(ROOT_FOLDER_ID, rootBookmarksFolder);

            while (bookmarkIterator.hasNext()) {
                PartnerBookmark bookmark = bookmarkIterator.next();
                if (bookmark == null) continue;

                // Check for duplicate ids.
                if (idMap.containsKey(bookmark.mId)) {
                    Log.i(TAG, "Duplicate bookmark id: "
                            +  bookmark.mId + ". Dropping bookmark.");
                    continue;
                }

                // Check for duplicate URLs.
                if (!bookmark.mIsFolder && urlSet.contains(bookmark.mUrl)) {
                    Log.i(TAG, "More than one bookmark pointing to "
                            + bookmark.mUrl
                            + ". Keeping only the first one for consistency with Chromium.");
                    continue;
                }

                idMap.put(bookmark.mId, bookmark);
                urlSet.add(bookmark.mUrl);
            }
            bookmarkIterator.close();
            int count = urlSet.size();
            RecordHistogram.recordCount100Histogram("PartnerBookmark.Count", count);

            if (count == 0) {
                handleZeroBookmark();
            } else {
                SharedPreferences pref = ContextUtils.getAppSharedPreferences();
                if (pref.contains(LAST_EMPTY_READ_PREFS_NAME)) {
                    pref.edit().remove(LAST_EMPTY_READ_PREFS_NAME).apply();
                }
            }

            // Recreate the folder hierarchy and read it.
            recreateFolderHierarchy(idMap);
            if (rootBookmarksFolder.mEntries.size() == 0) {
                Log.e(TAG, "ATTENTION: not using partner bookmarks as none were provided");
                return null;
            }
            if (rootBookmarksFolder.mEntries.size() != 1) {
                Log.e(TAG, "ATTENTION: more than one top-level partner bookmarks, ignored");
                return null;
            }

            readBookmarkHierarchy(rootBookmarksFolder, new HashSet<PartnerBookmark>());

            return null;
        }

        @Override
        protected void onPostExecute(Void v) {
            synchronized (mRootSync) {
                onBookmarksRead();
            }
        }

        private void recreateFolderHierarchy(LinkedHashMap<Long, PartnerBookmark> idMap) {
            for (PartnerBookmark bookmark : idMap.values()) {
                if (bookmark.mId == ROOT_FOLDER_ID) continue;

                // Look for invalid parent ids and self-cycles.
                if (!idMap.containsKey(bookmark.mParentId) || bookmark.mParentId == bookmark.mId) {
                    bookmark.mParent = idMap.get(ROOT_FOLDER_ID);
                    bookmark.mParent.mEntries.add(bookmark);
                    continue;
                }

                bookmark.mParent = idMap.get(bookmark.mParentId);
                bookmark.mParent.mEntries.add(bookmark);
            }
        }

        private PartnerBookmark createRootBookmarksFolderBookmark() {
            PartnerBookmark root = new PartnerBookmark();
            root.mId = ROOT_FOLDER_ID;
            root.mTitle = "[IMPLIED_ROOT]";
            root.mNativeId = INVALID_BOOKMARK_ID;
            root.mParentId = ROOT_FOLDER_ID;
            root.mIsFolder = true;
            return root;
        }

        private void readBookmarkHierarchy(
                PartnerBookmark bookmark, HashSet<PartnerBookmark> processedNodes) {
            // Avoid cycles in the hierarchy that could lead to infinite loops.
            if (processedNodes.contains(bookmark)) return;
            processedNodes.add(bookmark);

            if (bookmark.mId != ROOT_FOLDER_ID) {
                try {
                    synchronized (mRootSync) {
                        bookmark.mNativeId =
                                onBookmarkPush(
                                        bookmark.mUrl, bookmark.mTitle,
                                        bookmark.mIsFolder, bookmark.mParentId,
                                        bookmark.mFavicon, bookmark.mTouchicon);
                    }
                } catch (IllegalArgumentException e) {
                    Log.w(TAG, "Error inserting bookmark " + bookmark.mTitle, e);
                }
                if (bookmark.mNativeId == INVALID_BOOKMARK_ID) {
                    Log.e(TAG, "Error creating bookmark '" + bookmark.mTitle + "'.");
                    return;
                }
            }

            if (bookmark.mIsFolder) {
                for (PartnerBookmark entry : bookmark.mEntries) {
                    if (entry.mParent != bookmark) {
                        Log.w(TAG, "Hierarchy error in bookmark '"
                                + bookmark.mTitle + "'. Skipping.");
                        continue;
                    }
                    entry.mParentId = bookmark.mNativeId;
                    readBookmarkHierarchy(entry, processedNodes);
                }
            }
        }
    }

    /**
     * Disables partner bookmarks editing.
     */
    public static void disablePartnerBookmarksEditing() {
        sForceDisableEditing = true;
        if (sInitialized) nativeDisablePartnerBookmarksEditing();
    }

    private static void initializeAndDisableEditingIfNecessary() {
        sInitialized = true;
        if (sForceDisableEditing) disablePartnerBookmarksEditing();
    }

    // JNI
    private native long nativeInit();
    private native void nativeReset(long nativePartnerBookmarksReader);
    private native void nativeDestroy(long nativePartnerBookmarksReader);
    private native long nativeAddPartnerBookmark(long nativePartnerBookmarksReader, String url,
            String title, boolean isFolder, long parentId, byte[] favicon, byte[] touchicon,
            boolean fetchUncachedFaviconsFromServer, FetchFaviconCallback callback);
    private native void nativePartnerBookmarksCreationComplete(long nativePartnerBookmarksReader);
    private static native void nativeDisablePartnerBookmarksEditing();
}
