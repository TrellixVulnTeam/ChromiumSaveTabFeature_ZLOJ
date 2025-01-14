// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/os_crypt/key_storage_linux.h"

#include "base/bind.h"
#include "base/environment.h"
#include "base/logging.h"
#include "base/nix/xdg_util.h"
#include "base/sequenced_task_runner.h"
#include "base/synchronization/waitable_event.h"
#include "base/task_runner_util.h"
#include "base/threading/thread_restrictions.h"
#include "components/os_crypt/key_storage_config_linux.h"
#include "components/os_crypt/key_storage_util_linux.h"

#if defined(USE_LIBSECRET)
#include "components/os_crypt/key_storage_libsecret.h"
#endif
#if defined(USE_KEYRING)
#include "components/os_crypt/key_storage_keyring.h"
#endif
#if defined(USE_KWALLET)
#include "components/os_crypt/key_storage_kwallet.h"
#endif

#if defined(GOOGLE_CHROME_BUILD)
const char KeyStorageLinux::kFolderName[] = "Chrome Keys";
const char KeyStorageLinux::kKey[] = "Chrome Safe Storage";
#else
const char KeyStorageLinux::kFolderName[] = "Chromium Keys";
const char KeyStorageLinux::kKey[] = "Chromium Safe Storage";
#endif

namespace {

// Copies the password value from |result| to |password| and notifies on
// |on_password_received| that the result is ready.
void OnPasswordReceived(base::WaitableEvent* on_password_received,
                        std::string* password,
                        const std::string& result) {
  *password = result;
  if (on_password_received)
    on_password_received->Signal();
}

// Copies the initialisation result from |result| to |success| and notifies on
// |on_initialized| that the result is ready.
void OnInitialized(base::WaitableEvent* on_initialized,
                   bool* success,
                   const bool& result) {
  *success = result;
  if (on_initialized)
    on_initialized->Signal();
}

}  // namespace

// static
std::unique_ptr<KeyStorageLinux> KeyStorageLinux::CreateService(
    const os_crypt::Config& config) {
#if defined(USE_LIBSECRET) || defined(USE_KEYRING) || defined(USE_KWALLET)
  // Select a backend.
  bool use_backend = !config.should_use_preference ||
                     os_crypt::GetBackendUse(config.user_data_path);
  std::unique_ptr<base::Environment> env(base::Environment::Create());
  base::nix::DesktopEnvironment desktop_env =
      base::nix::GetDesktopEnvironment(env.get());
  os_crypt::SelectedLinuxBackend selected_backend =
      os_crypt::SelectBackend(config.store, use_backend, desktop_env);

  // TODO(crbug.com/782851) Schedule the initialisation on each backend's
  // favourite thread.

  // Try initializing the selected backend.
  // In case of GNOME_ANY, prefer Libsecret
  std::unique_ptr<KeyStorageLinux> key_storage;

#if defined(USE_LIBSECRET)
  if (selected_backend == os_crypt::SelectedLinuxBackend::GNOME_ANY ||
      selected_backend == os_crypt::SelectedLinuxBackend::GNOME_LIBSECRET) {
    key_storage = std::make_unique<KeyStorageLibsecret>();
    if (key_storage->WaitForInitOnTaskRunner()) {
      VLOG(1) << "OSCrypt using Libsecret as backend.";
      return key_storage;
    }
  }
#endif  // defined(USE_LIBSECRET)

#if defined(USE_KEYRING)
  if (selected_backend == os_crypt::SelectedLinuxBackend::GNOME_ANY ||
      selected_backend == os_crypt::SelectedLinuxBackend::GNOME_KEYRING) {
    key_storage =
        std::make_unique<KeyStorageKeyring>(config.main_thread_runner);
    if (key_storage->WaitForInitOnTaskRunner()) {
      VLOG(1) << "OSCrypt using Keyring as backend.";
      return key_storage;
    }
  }
#endif  // defined(USE_KEYRING)

#if defined(USE_KWALLET)
  if (selected_backend == os_crypt::SelectedLinuxBackend::KWALLET ||
      selected_backend == os_crypt::SelectedLinuxBackend::KWALLET5) {
    DCHECK(!config.product_name.empty());
    base::nix::DesktopEnvironment used_desktop_env =
        selected_backend == os_crypt::SelectedLinuxBackend::KWALLET
            ? base::nix::DESKTOP_ENVIRONMENT_KDE4
            : base::nix::DESKTOP_ENVIRONMENT_KDE5;
    key_storage = std::make_unique<KeyStorageKWallet>(
        used_desktop_env, config.product_name, config.dbus_task_runner);
    if (key_storage->WaitForInitOnTaskRunner()) {
      VLOG(1) << "OSCrypt using KWallet as backend.";
      return key_storage;
    }
  }
#endif  // defined(USE_KWALLET)
#endif  // defined(USE_LIBSECRET) || defined(USE_KEYRING) ||
        // defined(USE_KWALLET)

  // The appropriate store was not available.
  VLOG(1) << "OSCrypt did not initialize a backend.";
  return nullptr;
}

bool KeyStorageLinux::WaitForInitOnTaskRunner() {
  base::ScopedAllowBaseSyncPrimitivesOutsideBlockingScope allow_sync_primitives;
  base::SequencedTaskRunner* task_runner = GetTaskRunner();

  // We don't need to change threads if the backend has no preference or if we
  // are already on the right thread.
  if (!task_runner || task_runner->RunsTasksInCurrentSequence())
    return Init();

  base::WaitableEvent initialized(
      base::WaitableEvent::ResetPolicy::MANUAL,
      base::WaitableEvent::InitialState::NOT_SIGNALED);
  bool success;
  PostTaskAndReplyWithResult(
      task_runner, FROM_HERE,
      base::BindOnce(&KeyStorageLinux::Init, base::Unretained(this)),
      base::BindOnce(&OnInitialized, &initialized, &success));
  initialized.Wait();
  return success;
}

std::string KeyStorageLinux::GetKey() {
  base::ScopedAllowBaseSyncPrimitivesOutsideBlockingScope allow_sync_primitives;
  base::SequencedTaskRunner* task_runner = GetTaskRunner();

  // We don't need to change threads if the backend has no preference or if we
  // are already on the right thread.
  if (!task_runner || task_runner->RunsTasksInCurrentSequence())
    return GetKeyImpl();

  base::WaitableEvent password_loaded(
      base::WaitableEvent::ResetPolicy::MANUAL,
      base::WaitableEvent::InitialState::NOT_SIGNALED);
  std::string password;
  PostTaskAndReplyWithResult(
      task_runner, FROM_HERE,
      base::BindOnce(&KeyStorageLinux::GetKeyImpl, base::Unretained(this)),
      base::BindOnce(&OnPasswordReceived, &password_loaded, &password));
  password_loaded.Wait();
  return password;
}

base::SequencedTaskRunner* KeyStorageLinux::GetTaskRunner() {
  return nullptr;
}
