// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/file_system_provider/provided_file_system_info.h"

#include "base/logging.h"

namespace chromeos {
namespace file_system_provider {

ProviderId::ProviderId(const std::string& internal_id,
                       ProviderType provider_type)
    : internal_id_(internal_id), type_(provider_type) {}

ProviderId::ProviderId() : type_(INVALID) {}

// static
ProviderId ProviderId::CreateFromExtensionId(const std::string& extension_id) {
  return ProviderId(extension_id, EXTENSION);
}

// static
ProviderId ProviderId::CreateFromNativeId(const std::string& native_id) {
  return ProviderId(native_id, NATIVE);
}

const std::string& ProviderId::GetExtensionId() const {
  CHECK_EQ(EXTENSION, type_);
  return internal_id_;
}

const std::string& ProviderId::GetNativeId() const {
  CHECK_EQ(NATIVE, type_);
  return internal_id_;
}

const std::string& ProviderId::GetIdUnsafe() const {
  return internal_id_;
}

ProviderId::ProviderType ProviderId::GetType() const {
  return type_;
}

// Returns the internal_id_ for extensions for  backwards compatibility,
// Adds '@' for native ids to avoid collisions.
std::string ProviderId::ToString() const {
  switch (type_) {
    case EXTENSION:
      return internal_id_;
    case NATIVE:
      return std::string("@") + internal_id_;
    case INVALID:
      NOTREACHED();
      return "";
  }
}

bool ProviderId::operator==(const ProviderId& other) const {
  return type_ == other.GetType() && internal_id_ == other.GetIdUnsafe();
}

bool ProviderId::operator<(const ProviderId& other) const {
  return std::tie(type_, internal_id_) <
         std::tie(other.type_, other.internal_id_);
}

MountOptions::MountOptions()
    : writable(false),
      supports_notify_tag(false),
      opened_files_limit(0),
      persistent(true) {}

MountOptions::MountOptions(const std::string& file_system_id,
                           const std::string& display_name)
    : file_system_id(file_system_id),
      display_name(display_name),
      writable(false),
      supports_notify_tag(false),
      opened_files_limit(0),
      persistent(true) {}

MountOptions::MountOptions(const MountOptions& source) = default;

ProvidedFileSystemInfo::ProvidedFileSystemInfo()
    : writable_(false),
      supports_notify_tag_(false),
      configurable_(false),
      watchable_(false),
      source_(extensions::SOURCE_FILE) {}

ProvidedFileSystemInfo::ProvidedFileSystemInfo(
    const ProviderId& provider_id,
    const MountOptions& mount_options,
    const base::FilePath& mount_path,
    bool configurable,
    bool watchable,
    extensions::FileSystemProviderSource source)
    : provider_id_(provider_id),
      file_system_id_(mount_options.file_system_id),
      display_name_(mount_options.display_name),
      writable_(mount_options.writable),
      supports_notify_tag_(mount_options.supports_notify_tag),
      opened_files_limit_(mount_options.opened_files_limit),
      mount_path_(mount_path),
      configurable_(configurable),
      watchable_(watchable),
      source_(source) {
  DCHECK_LE(0, mount_options.opened_files_limit);
}

ProvidedFileSystemInfo::ProvidedFileSystemInfo(
    const std::string& extension_id,
    const MountOptions& mount_options,
    const base::FilePath& mount_path,
    bool configurable,
    bool watchable,
    extensions::FileSystemProviderSource source)
    : ProvidedFileSystemInfo(ProviderId::CreateFromExtensionId(extension_id),
                             mount_options,
                             mount_path,
                             configurable,
                             watchable,
                             source) {}

ProvidedFileSystemInfo::ProvidedFileSystemInfo(
    const ProvidedFileSystemInfo& other) = default;

ProvidedFileSystemInfo::~ProvidedFileSystemInfo() {}

}  // namespace file_system_provider
}  // namespace chromeos
