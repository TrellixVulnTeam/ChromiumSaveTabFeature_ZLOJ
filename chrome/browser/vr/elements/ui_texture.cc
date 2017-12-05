// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/vr/elements/ui_texture.h"

#include <set>
#include <string>
#include <vector>

#include "base/i18n/char_iterator.h"
#include "base/i18n/rtl.h"
#include "base/memory/ptr_util.h"
#include "base/strings/string_util.h"
#include "base/trace_event/trace_event.h"
#include "chrome/browser/vr/font_fallback.h"
#include "third_party/icu/source/common/unicode/uscript.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/font_list.h"
#include "ui/gfx/render_text.h"
#include "ui/gfx/text_elider.h"
#include "ui/gl/gl_bindings.h"

namespace vr {

namespace {

static constexpr char kDefaultFontFamily[] = "sans-serif";

static bool force_font_fallback_failure_for_testing_ = false;

std::set<UChar32> CollectDifferentChars(base::string16 text) {
  std::set<UChar32> characters;
  for (base::i18n::UTF16CharIterator it(&text); !it.end(); it.Advance()) {
    characters.insert(it.get());
  }
  return characters;
}

}  // namespace

UiTexture::UiTexture() = default;

UiTexture::~UiTexture() = default;

void UiTexture::DrawAndLayout(SkCanvas* canvas, const gfx::Size& texture_size) {
  TRACE_EVENT0("gpu", "UiTexture::DrawAndLayout");
  canvas->drawColor(SK_ColorTRANSPARENT);
  dirty_ = false;
  Draw(canvas, texture_size);
}

bool UiTexture::LocalHitTest(const gfx::PointF& point) const {
  return false;
}

void UiTexture::OnInitialized() {
  set_dirty();
}

std::vector<std::unique_ptr<gfx::RenderText>> UiTexture::PrepareDrawStringRect(
    const base::string16& text,
    const gfx::FontList& font_list,
    SkColor color,
    gfx::Rect* bounds,
    UiTexture::TextAlignment text_alignment,
    UiTexture::WrappingBehavior wrapping_behavior) {
  DCHECK(bounds);

  std::vector<std::unique_ptr<gfx::RenderText>> lines;

  if (wrapping_behavior == kWrappingBehaviorWrap) {
    gfx::Rect rect(*bounds);
    std::vector<base::string16> strings;
    gfx::ElideRectangleText(text, font_list, bounds->width(),
                            bounds->height() ? bounds->height() : INT_MAX,
                            gfx::WRAP_LONG_WORDS, &strings);

    int height = 0;
    int line_height = 0;
    for (size_t i = 0; i < strings.size(); i++) {
      std::unique_ptr<gfx::RenderText> render_text = CreateConfiguredRenderText(
          strings[i], font_list, color, text_alignment);

      if (i == 0) {
        // Measure line and center text vertically.
        line_height = render_text->GetStringSize().height();
        rect.set_height(line_height);
        if (bounds->height()) {
          const int text_height = strings.size() * line_height;
          rect += gfx::Vector2d(0, (bounds->height() - text_height) / 2);
        }
      }

      render_text->SetDisplayRect(rect);
      height += line_height;
      rect += gfx::Vector2d(0, line_height);
      lines.push_back(std::move(render_text));
    }

    // Set calculated height.
    if (bounds->height() == 0)
      bounds->set_height(height);

  } else {
    std::unique_ptr<gfx::RenderText> render_text =
        CreateConfiguredRenderText(text, font_list, color, text_alignment);
    if (bounds->width() != 0)
      render_text->SetElideBehavior(gfx::TRUNCATE);

    if (bounds->width() == 0)
      bounds->set_width(render_text->GetStringSize().width());
    if (bounds->height() == 0)
      bounds->set_height(render_text->GetStringSize().height());

    render_text->SetDisplayRect(*bounds);
    lines.push_back(std::move(render_text));
  }
  return lines;
}

std::unique_ptr<gfx::RenderText> UiTexture::CreateRenderText() {
  std::unique_ptr<gfx::RenderText> render_text(
      gfx::RenderText::CreateInstance());

  // Subpixel rendering is counterproductive when drawing VR textures.
  render_text->set_subpixel_rendering_suppressed(true);

  return render_text;
}

std::unique_ptr<gfx::RenderText> UiTexture::CreateConfiguredRenderText(
    const base::string16& text,
    const gfx::FontList& font_list,
    SkColor color,
    UiTexture::TextAlignment text_alignment) {
  std::unique_ptr<gfx::RenderText> render_text(CreateRenderText());
  render_text->SetText(text);
  render_text->SetFontList(font_list);
  render_text->SetColor(color);

  switch (text_alignment) {
    case UiTexture::kTextAlignmentNone:
      break;
    case UiTexture::kTextAlignmentLeft:
      render_text->SetHorizontalAlignment(gfx::ALIGN_LEFT);
      break;
    case UiTexture::kTextAlignmentRight:
      render_text->SetHorizontalAlignment(gfx::ALIGN_RIGHT);
      break;
    case UiTexture::kTextAlignmentCenter:
      render_text->SetHorizontalAlignment(gfx::ALIGN_CENTER);
      break;
  }

  const int font_style = font_list.GetFontStyle();
  render_text->SetStyle(gfx::ITALIC, (font_style & gfx::Font::ITALIC) != 0);
  render_text->SetStyle(gfx::UNDERLINE,
                        (font_style & gfx::Font::UNDERLINE) != 0);
  render_text->SetWeight(font_list.GetFontWeight());

  return render_text;
}

bool UiTexture::IsRTL() {
  return base::i18n::IsRTL();
}

bool UiTexture::GetFontList(const std::string& preferred_font_name,
                            int font_size,
                            base::string16 text,
                            gfx::FontList* font_list) {
  if (force_font_fallback_failure_for_testing_)
    return false;

  gfx::Font preferred_font(preferred_font_name, font_size);
  std::vector<gfx::Font> fonts{preferred_font};

  std::set<std::string> names;
  // TODO(acondor): Query BrowserProcess to obtain the application locale.
  for (UChar32 c : CollectDifferentChars(text)) {
    std::string name;
    bool found_name = GetFallbackFontNameForChar(preferred_font, c, "", &name);
    if (!found_name)
      return false;
    if (!name.empty())
      names.insert(name);
  }
  for (const auto& name : names) {
    DCHECK(!name.empty());
    fonts.push_back(gfx::Font(name, font_size));
  }
  *font_list = gfx::FontList(fonts);
  return true;
}

bool UiTexture::GetDefaultFontList(int font_size,
                                   base::string16 text,
                                   gfx::FontList* font_list) {
  return GetFontList(kDefaultFontFamily, font_size, text, font_list);
}

SkColor UiTexture::foreground_color() const {
  DCHECK(foreground_color_);
  return foreground_color_.value();
}

SkColor UiTexture::background_color() const {
  DCHECK(background_color_);
  return background_color_.value();
}

void UiTexture::SetForegroundColor(SkColor color) {
  if (foreground_color_ == color)
    return;
  foreground_color_ = color;
  set_dirty();
}

void UiTexture::SetBackgroundColor(SkColor color) {
  if (background_color_ == color)
    return;
  background_color_ = color;
  set_dirty();
}

void UiTexture::SetForceFontFallbackFailureForTesting(bool force) {
  force_font_fallback_failure_for_testing_ = force;
}

}  // namespace vr