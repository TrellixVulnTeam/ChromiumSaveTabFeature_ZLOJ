// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_FONT_CACHE_DISPATCHER_WIN_H_
#define CONTENT_COMMON_FONT_CACHE_DISPATCHER_WIN_H_

#include <windows.h>

#include "base/macros.h"
#include "base/memory/singleton.h"
#include "content/common/font_cache_win.mojom.h"

namespace service_manager {
struct BindSourceInfo;
}

namespace content {

// Dispatches messages used for font caching on Windows. This is needed because
// Windows can't load fonts into its kernel cache in sandboxed processes. So the
// sandboxed process asks the browser process to do this for it.
class FontCacheDispatcher : public mojom::FontCacheWin {
 public:
  FontCacheDispatcher();
  ~FontCacheDispatcher() override;

  static void Create(mojom::FontCacheWinRequest request,
                     const service_manager::BindSourceInfo& source_info);

 private:
  // mojom::FontCacheWin
  void PreCacheFont(const LOGFONT&) override;
  void ReleaseCachedFonts() override;

  DISALLOW_COPY_AND_ASSIGN(FontCacheDispatcher);
};

}  // namespace content

#endif  // CONTENT_COMMON_FONT_CACHE_DISPATCHER_WIN_H_