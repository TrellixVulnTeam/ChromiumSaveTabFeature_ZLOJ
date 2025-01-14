// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/keyboard/notification_manager.h"
#include "base/observer_list.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/keyboard/keyboard_controller_observer.h"

namespace keyboard {

template <typename T>
bool ValueNotificationConsolidator<T>::ShouldSendNotification(
    const T new_value) {
  if (never_sent_) {
    value_ = new_value;
    never_sent_ = false;
    return true;
  }
  const bool value_changed = new_value != value_;
  if (value_changed) {
    value_ = new_value;
  }
  return value_changed;
}

NotificationManager::NotificationManager() {}

void NotificationManager::SendNotifications(
    bool bounds_obscure_usable_region,
    bool bounds_affect_layout,
    const gfx::Rect& bounds,
    const base::ObserverList<KeyboardControllerObserver>& observers) {
  bool is_available = !bounds.IsEmpty();
  bool send_availability_notification =
      ShouldSendAvailabilityNotification(is_available);

  bool send_visual_bounds_notification =
      ShouldSendVisualBoundsNotification(bounds);

  const gfx::Rect occluded_region =
      bounds_obscure_usable_region ? bounds : gfx::Rect();
  bool send_occluded_bounds_notification =
      ShouldSendOccludedBoundsNotification(occluded_region);

  const gfx::Rect workspace_layout_offset_region =
      bounds_affect_layout ? bounds : gfx::Rect();
  bool send_displaced_bounds_notification =
      ShouldSendWorkspaceDisplacementBoundsNotification(
          workspace_layout_offset_region);

  for (KeyboardControllerObserver& observer : observers) {
    if (send_availability_notification)
      observer.OnKeyboardAvailabilityChanging(is_available);

    if (send_visual_bounds_notification)
      observer.OnKeyboardVisibleBoundsChanging(bounds);

    if (send_occluded_bounds_notification)
      observer.OnKeyboardWorkspaceOccludedBoundsChanging(occluded_region);

    if (send_displaced_bounds_notification) {
      observer.OnKeyboardWorkspaceDisplacingBoundsChanging(
          workspace_layout_offset_region);
    }

    // TODO(blakeo): remove this when all consumers have migrated to one of
    // the notifications above.
    observer.OnKeyboardBoundsChanging(bounds);
  }
}

bool NotificationManager::ShouldSendAvailabilityNotification(
    bool current_availability) {
  return availability_.ShouldSendNotification(current_availability);
}

bool NotificationManager::ShouldSendVisualBoundsNotification(
    const gfx::Rect& new_bounds) {
  return visual_bounds_.ShouldSendNotification(
      CanonicalizeEmptyRectangles(new_bounds));
}

bool NotificationManager::ShouldSendOccludedBoundsNotification(
    const gfx::Rect& new_bounds) {
  return occluded_bounds_.ShouldSendNotification(
      CanonicalizeEmptyRectangles(new_bounds));
}

bool NotificationManager::ShouldSendWorkspaceDisplacementBoundsNotification(
    const gfx::Rect& new_bounds) {
  return workspace_displaced_bounds_.ShouldSendNotification(
      CanonicalizeEmptyRectangles(new_bounds));
}

const gfx::Rect NotificationManager::CanonicalizeEmptyRectangles(
    const gfx::Rect rect) const {
  if (rect.IsEmpty()) {
    return gfx::Rect();
  }
  return rect;
}

}  // namespace keyboard
