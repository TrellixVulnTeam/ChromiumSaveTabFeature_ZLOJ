// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/os_crypt/key_storage_kwallet.h"

#include <utility>

#include "base/base64.h"
#include "base/rand_util.h"
#include "base/sequenced_task_runner.h"
#include "components/os_crypt/kwallet_dbus.h"
#include "dbus/bus.h"

KeyStorageKWallet::KeyStorageKWallet(
    base::nix::DesktopEnvironment desktop_env,
    std::string app_name,
    scoped_refptr<base::SequencedTaskRunner> dbus_task_runner)
    : desktop_env_(desktop_env),
      handle_(-1),
      app_name_(std::move(app_name)),
      dbus_task_runner_(dbus_task_runner) {}

KeyStorageKWallet::~KeyStorageKWallet() {
  // The handle is shared between programs that are using the same wallet.
  // Closing the wallet is a nop in the typical case.
  bool success = true;
  ignore_result(kwallet_dbus_->Close(handle_, false, app_name_, &success));
  kwallet_dbus_->GetSessionBus()->ShutdownAndBlock();
}

base::SequencedTaskRunner* KeyStorageKWallet::GetTaskRunner() {
  return dbus_task_runner_.get();
}

bool KeyStorageKWallet::Init() {
  DCHECK(dbus_task_runner_->RunsTasksInCurrentSequence());
  // Initialize using the production KWalletDBus.
  return InitWithKWalletDBus(nullptr);
}

bool KeyStorageKWallet::InitWithKWalletDBus(
    std::unique_ptr<KWalletDBus> optional_kwallet_dbus_ptr) {
  if (optional_kwallet_dbus_ptr) {
    kwallet_dbus_ = std::move(optional_kwallet_dbus_ptr);
  } else {
    // Initializing with production KWalletDBus
    kwallet_dbus_.reset(new KWalletDBus(desktop_env_));
    dbus::Bus::Options options;
    options.bus_type = dbus::Bus::SESSION;
    options.connection_type = dbus::Bus::PRIVATE;
    kwallet_dbus_->SetSessionBus(new dbus::Bus(options));
  }

  InitResult result = InitWallet();
  // If KWallet might not have started, attempt to start it and retry.
  if (result == InitResult::TEMPORARY_FAIL && kwallet_dbus_->StartKWalletd())
    result = InitWallet();

  return result == InitResult::SUCCESS;
}

KeyStorageKWallet::InitResult KeyStorageKWallet::InitWallet() {
  // Check that KWallet is enabled.
  bool enabled = false;
  KWalletDBus::Error error = kwallet_dbus_->IsEnabled(&enabled);
  switch (error) {
    case KWalletDBus::Error::CANNOT_CONTACT:
      return InitResult::TEMPORARY_FAIL;
    case KWalletDBus::Error::CANNOT_READ:
      return InitResult::PERMANENT_FAIL;
    case KWalletDBus::Error::SUCCESS:
      break;
  }
  if (!enabled)
    return InitResult::PERMANENT_FAIL;

  // Get the wallet name.
  error = kwallet_dbus_->NetworkWallet(&wallet_name_);
  switch (error) {
    case KWalletDBus::Error::CANNOT_CONTACT:
      return InitResult::TEMPORARY_FAIL;
    case KWalletDBus::Error::CANNOT_READ:
      return InitResult::PERMANENT_FAIL;
    case KWalletDBus::Error::SUCCESS:
      return InitResult::SUCCESS;
  }

  NOTREACHED();
  return InitResult::PERMANENT_FAIL;
}

std::string KeyStorageKWallet::GetKeyImpl() {
  DCHECK(dbus_task_runner_->RunsTasksInCurrentSequence());

  // Get handle
  KWalletDBus::Error error =
      kwallet_dbus_->Open(wallet_name_, app_name_, &handle_);
  if (error || handle_ == -1)
    return std::string();

  // Create folder
  if (!InitFolder())
    return std::string();

  // Read password
  std::string password;
  error =
      kwallet_dbus_->ReadPassword(handle_, KeyStorageLinux::kFolderName,
                                  KeyStorageLinux::kKey, app_name_, &password);
  if (error)
    return std::string();

  // If there is no entry, generate and write a new password.
  if (password.empty()) {
    base::Base64Encode(base::RandBytesAsString(16), &password);
    bool success;
    error = kwallet_dbus_->WritePassword(handle_, KeyStorageLinux::kFolderName,
                                         KeyStorageLinux::kKey, password,
                                         app_name_, &success);
    if (error || !success)
      return std::string();
  }

  return password;
}

bool KeyStorageKWallet::InitFolder() {
  bool has_folder = false;
  KWalletDBus::Error error = kwallet_dbus_->HasFolder(
      handle_, KeyStorageLinux::kFolderName, app_name_, &has_folder);
  if (error)
    return false;

  if (!has_folder) {
    bool success = false;
    error = kwallet_dbus_->CreateFolder(handle_, KeyStorageLinux::kFolderName,
                                        app_name_, &success);
    if (error || !success)
      return false;
  }

  return true;
}
