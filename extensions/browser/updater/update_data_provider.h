// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXTENSIONS_BROWSER_UPDATER_UPDATE_DATA_PROVIDER_H_
#define EXTENSIONS_BROWSER_UPDATER_UPDATE_DATA_PROVIDER_H_

#include <string>
#include <vector>

#include "base/callback.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "extensions/browser/updater/extension_installer.h"

namespace base {
class FilePath;
}

namespace content {
class BrowserContext;
}

namespace update_client {
struct CrxComponent;
}

namespace extensions {

// This class exists to let an UpdateClient retrieve information about a set of
// extensions it is doing an update check for.
class UpdateDataProvider : public base::RefCounted<UpdateDataProvider> {
 public:
  using UpdateClientCallback = ExtensionInstaller::UpdateClientCallback;
  using InstallCallback = base::OnceCallback<void(
      content::BrowserContext* context,
      const std::string& /* extension_id */,
      const std::string& /* public_key */,
      const base::FilePath& /* unpacked_dir */,
      UpdateClientCallback /* update_client_callback */)>;

  // We need a browser context to use when retrieving data for a set of
  // extension ids, as well as an install callback for proceeding with
  // installation steps once the UpdateClient has downloaded and unpacked
  // an update for an extension.
  UpdateDataProvider(content::BrowserContext* context,
                     InstallCallback install_callback);

  // Notify this object that the associated browser context is being shut down
  // the pointer to the context should be dropped and no more work should be
  // done.
  void Shutdown();

  // Matches update_client::UpdateClient::CrxDataCallback
  void GetData(const std::vector<std::string>& ids,
               std::vector<update_client::CrxComponent>* data);

 private:
  friend class base::RefCounted<UpdateDataProvider>;
  ~UpdateDataProvider();

  // This function should be called on the browser UI thread.
  void RunInstallCallback(const std::string& extension_id,
                          const std::string& public_key,
                          const base::FilePath& unpacked_dir,
                          UpdateClientCallback update_client_callback);

  content::BrowserContext* context_;
  InstallCallback install_callback_;

  DISALLOW_COPY_AND_ASSIGN(UpdateDataProvider);
};

}  // namespace extensions

#endif  // EXTENSIONS_BROWSER_UPDATER_UPDATE_DATA_PROVIDER_H_
