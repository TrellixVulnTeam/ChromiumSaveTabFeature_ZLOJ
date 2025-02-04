// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_DATA_REDUCTION_PROXY_CORE_BROWSER_NETWORK_PROPERTIES_MANAGER_H_
#define COMPONENTS_DATA_REDUCTION_PROXY_CORE_BROWSER_NETWORK_PROPERTIES_MANAGER_H_

#include <map>
#include <memory>
#include <string>

#include "base/macros.h"
#include "base/sequence_checker.h"
#include "base/sequenced_task_runner.h"
#include "components/data_reduction_proxy/proto/network_properties.pb.h"
#include "components/prefs/pref_service.h"

namespace base {
class Value;
}

namespace data_reduction_proxy {

// Stores the properties of a single network. Created on the UI thread, but
// lives on the IO thread. Guaranteed to be destroyed on IO thread if the IO
// thread is still available at the time of destruction. If the IO thread is
// unavailable, then the destruction will happen on the UI thread.
class NetworkPropertiesManager {
 public:
  NetworkPropertiesManager(
      PrefService* pref_service,
      scoped_refptr<base::SequencedTaskRunner> ui_task_runner);

  virtual ~NetworkPropertiesManager();

  // Called when the user clears the browsing history.
  void DeleteHistory();

  void ShutdownOnUIThread();

  void OnChangeInNetworkID(const std::string& network_id);

  // Returns true if usage of secure proxies are allowed on the current network.
  // Returns the status of core secure proxies if |is_core_proxy| is true.
  bool IsSecureProxyAllowed(bool is_core_proxy) const;

  // Returns true if usage of insecure proxies are allowed on the current
  // network.  Returns the status of core non-secure proxies if |is_core_proxy|
  // is true.
  bool IsInsecureProxyAllowed(bool is_core_proxy) const;

  // Returns true if usage of secure proxies has been disallowed by the carrier
  // on the current network.
  bool IsSecureProxyDisallowedByCarrier() const;

  // Sets the status of whether the usage of secure proxies is disallowed by the
  // carrier on the current network.
  void SetIsSecureProxyDisallowedByCarrier(bool disallowed_by_carrier);

  // Returns true if the current network has a captive portal.
  bool IsCaptivePortal() const;

  // Sets the status of whether the current network has a captive portal or not.
  // If the current network has captive portal, usage of secure proxies is
  // disallowed.
  void SetIsCaptivePortal(bool is_captive_portal);

  // Returns true if the warmup URL probe has failed
  // on secure (or insecure), core (or non-core) data saver proxies on the
  // current network.
  bool HasWarmupURLProbeFailed(bool secure_proxy, bool is_core_proxy) const;

  // Sets the status of whether the fetching of warmup URL failed on the current
  // network. Sets the status for secure (or insecure), core (or non-core) data
  // saver proxies.
  void SetHasWarmupURLProbeFailed(bool secure_proxy,
                                  bool is_core_proxy,
                                  bool warmup_url_probe_failed);

 private:
  // Map from network IDs to network properties.
  typedef std::map<std::string, NetworkProperties> NetworkPropertiesContainer;

  // PrefManager writes or updates the network properties prefs. Created on
  // UI thread, and should be used on the UI thread. May be destroyed on UI
  // or IO thread.
  class PrefManager;

  // Called when there is a change in the network property of the current
  // network.
  void OnChangeInNetworkPropertyOnIOThread();

  static NetworkPropertiesContainer ConvertDictionaryValueToParsedPrefs(
      const base::Value* value);

  // Task runner on which prefs should be accessed.
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner_;

  // Network properties of different networks. Should be accessed on the IO
  // thread.
  NetworkPropertiesContainer network_properties_container_;

  // ID of the current network.
  std::string network_id_;

  // State of the proxies on the current network.
  NetworkProperties network_properties_;

  std::unique_ptr<PrefManager> pref_manager_;

  // Should be dereferenced only on the UI thread.
  base::WeakPtr<PrefManager> pref_manager_weak_ptr_;

  SEQUENCE_CHECKER(sequence_checker_);

  DISALLOW_COPY_AND_ASSIGN(NetworkPropertiesManager);
};

}  // namespace data_reduction_proxy

#endif  // COMPONENTS_DATA_REDUCTION_PROXY_CORE_BROWSER_NETWORK_PROPERTIES_MANAGER_H_