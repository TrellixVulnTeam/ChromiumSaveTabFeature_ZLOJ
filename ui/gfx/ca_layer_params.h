// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_CA_LAYER_PARAMS_H_
#define UI_GFX_CA_LAYER_PARAMS_H_

#include "build/build_config.h"
#include "ui/gfx/gfx_export.h"

#if defined(OS_MACOSX) && !defined(OS_IOS)
#include "ui/gfx/mac/io_surface.h"
#endif

namespace gfx {

// The parameters required to add a composited frame to a CALayer. This
// is used only on macOS.
struct GFX_EXPORT CALayerParams {
  CALayerParams();
  CALayerParams(CALayerParams&& params);
  CALayerParams(const CALayerParams& params);
  CALayerParams& operator=(CALayerParams&& params);
  CALayerParams& operator=(const CALayerParams& params);
  ~CALayerParams();

#if defined(OS_MACOSX) && !defined(OS_IOS)
  // The |is_empty| flag is used to short-circuit code to handle CALayerParams
  // on non-macOS platforms.
  bool is_empty = false;
  // Can be used to instantiate a CALayerTreeHost in the browser process, which
  // will display a CALayerTree rooted in the GPU process. This is non-zero when
  // using remote CoreAnimation.
  uint32_t ca_context_id = 0;
  // Used to set the contents of a CALayer in the browser to an IOSurface that
  // is specified by the GPU process. This is non-null iff |ca_context_id| is
  // zero.
  gfx::ScopedRefCountedIOSurfaceMachPort io_surface_mach_port;
  // The geometry of the
  gfx::Size pixel_size;
  float scale_factor = 1.f;
#else
  bool is_empty = true;
#endif
};

}  // namespace gfx

#endif  // UI_GFX_CA_LAYER_PARAMS_H_
