// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.preferences.languages;

import org.chromium.chrome.browser.preferences.PrefServiceBridge;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

/**
 * Manages languages details for languages settings.
 *
 * The LanguagesManager is responsible for fetching languages details from native.
 */
class LanguagesManager {
    /**
     * An observer interface that allows other classes to know when the accept language list is
     * updated in native side.
     */
    interface AcceptLanguageObserver {
        /**
         * Called when the accept languages for the current user are updated.
         */
        void onDataUpdated();
    }

    private static LanguagesManager sManager;

    private final PrefServiceBridge mPrefServiceBridge;
    private final Map<String, LanguageItem> mLanguagesMap;

    private AcceptLanguageObserver mObserver;

    private LanguagesManager() {
        mPrefServiceBridge = PrefServiceBridge.getInstance();

        // Get all language data from native.
        mLanguagesMap = new LinkedHashMap<>();
        for (LanguageItem item : mPrefServiceBridge.getChromeLanguageList()) {
            mLanguagesMap.put(item.getCode(), item);
        }
    }

    private void notifyAcceptLanguageObserver() {
        if (mObserver != null) mObserver.onDataUpdated();
    }

    /**
     * Sets the observer tracking the user accept languages changes.
     */
    public void setAcceptLanguageObserver(AcceptLanguageObserver observer) {
        mObserver = observer;
    }

    /**
     * @return A list of LanguageItems for the current user's accept languages.
     */
    public List<LanguageItem> getUserAcceptLanguageItems() {
        // Always read the latest user accept language code list from native.
        List<String> codes = mPrefServiceBridge.getUserLanguageCodes();

        List<LanguageItem> results = new ArrayList<>();
        // Keep the same order as accept language codes list.
        for (String code : codes) {
            // Check language code and only languages supported on Android are added in.
            if (mLanguagesMap.containsKey(code)) results.add(mLanguagesMap.get(code));
        }
        return results;
    }

    /**
     * @return A list of LanguageItems, excluding the current user's accept languages.
     */
    public List<LanguageItem> getLanguageItemsExcludingUserAccept() {
        // Always read the latest user accept language code list from native.
        List<String> codes = mPrefServiceBridge.getUserLanguageCodes();

        List<LanguageItem> results = new ArrayList<>();
        // Keep the same order as mLanguagesMap.
        for (LanguageItem item : mLanguagesMap.values()) {
            if (!codes.contains(item.getCode())) results.add(item);
        }
        return results;
    }

    /**
     * Add a language to the current user's accept languages.
     * @param code The language code to remove.
     */
    public void addToAcceptLanguages(String code) {
        mPrefServiceBridge.updateUserAcceptLanguages(code, true /* is_add */);
        notifyAcceptLanguageObserver();
    }

    /**
     * Remove a language from the current user's accept languages.
     * @param code The language code to remove.
     */
    public void removeFromAcceptLanguages(String code) {
        mPrefServiceBridge.updateUserAcceptLanguages(code, false /* is_add */);
        notifyAcceptLanguageObserver();
    }

    /**
     * Get the static instance of ChromePreferenceManager if it exists else create it.
     * @return the LanguagesManager singleton.
     */
    public static LanguagesManager getInstance() {
        if (sManager == null) sManager = new LanguagesManager();
        return sManager;
    }

    /**
     * Called to release unused resources.
     */
    public static void recycle() {
        sManager = null;
    }
}
