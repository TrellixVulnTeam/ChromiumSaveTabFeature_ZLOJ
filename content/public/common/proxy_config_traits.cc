// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/common/proxy_config_traits.h"

#include "base/logging.h"
#include "url/gurl.h"
#include "url/mojo/url_gurl_struct_traits.h"

namespace mojo {

std::vector<std::string>
StructTraits<content::mojom::ProxyBypassRulesDataView,
             net::ProxyBypassRules>::rules(const net::ProxyBypassRules& r) {
  std::vector<std::string> out;
  for (const auto& rule : r.rules()) {
    out.push_back(rule->ToString());
  }
  return out;
}

bool StructTraits<content::mojom::ProxyBypassRulesDataView,
                  net::ProxyBypassRules>::
    Read(content::mojom::ProxyBypassRulesDataView data,
         net::ProxyBypassRules* out_proxy_bypass_rules) {
  std::vector<std::string> rules;
  if (!data.ReadRules(&rules))
    return false;
  for (const auto& rule : rules) {
    if (!out_proxy_bypass_rules->AddRuleFromString(rule))
      return false;
  }
  return true;
}

std::vector<std::string>
StructTraits<content::mojom::ProxyListDataView, net::ProxyList>::proxies(
    const net::ProxyList& r) {
  std::vector<std::string> out;
  for (const auto& proxy : r.GetAll()) {
    out.push_back(proxy.ToPacString());
  }
  return out;
}

bool StructTraits<content::mojom::ProxyListDataView, net::ProxyList>::Read(
    content::mojom::ProxyListDataView data,
    net::ProxyList* out_proxy_list) {
  std::vector<std::string> proxies;
  if (!data.ReadProxies(&proxies))
    return false;
  for (const auto& proxy : proxies) {
    net::ProxyServer proxy_server = net::ProxyServer::FromPacString(proxy);
    if (!proxy_server.is_valid())
      return false;
    out_proxy_list->AddProxyServer(proxy_server);
  }
  return true;
}

content::mojom::ProxyRulesType
EnumTraits<content::mojom::ProxyRulesType, net::ProxyConfig::ProxyRules::Type>::
    ToMojom(net::ProxyConfig::ProxyRules::Type net_proxy_rules_type) {
  switch (net_proxy_rules_type) {
    case net::ProxyConfig::ProxyRules::TYPE_NO_RULES:
      return content::mojom::ProxyRulesType::TYPE_NO_RULES;
    case net::ProxyConfig::ProxyRules::TYPE_SINGLE_PROXY:
      return content::mojom::ProxyRulesType::TYPE_SINGLE_PROXY;
    case net::ProxyConfig::ProxyRules::TYPE_PROXY_PER_SCHEME:
      return content::mojom::ProxyRulesType::TYPE_PROXY_PER_SCHEME;
  }
  return content::mojom::ProxyRulesType::TYPE_NO_RULES;
}

bool EnumTraits<content::mojom::ProxyRulesType,
                net::ProxyConfig::ProxyRules::Type>::
    FromMojom(content::mojom::ProxyRulesType mojo_proxy_rules_type,
              net::ProxyConfig::ProxyRules::Type* out) {
  switch (mojo_proxy_rules_type) {
    case content::mojom::ProxyRulesType::TYPE_NO_RULES:
      *out = net::ProxyConfig::ProxyRules::TYPE_NO_RULES;
      return true;
    case content::mojom::ProxyRulesType::TYPE_SINGLE_PROXY:
      *out = net::ProxyConfig::ProxyRules::TYPE_SINGLE_PROXY;
      return true;
    case content::mojom::ProxyRulesType::TYPE_PROXY_PER_SCHEME:
      *out = net::ProxyConfig::ProxyRules::TYPE_PROXY_PER_SCHEME;
      return true;
  }
  return false;
}

bool StructTraits<content::mojom::ProxyRulesDataView,
                  net::ProxyConfig::ProxyRules>::
    Read(content::mojom::ProxyRulesDataView data,
         net::ProxyConfig::ProxyRules* out_proxy_rules) {
  out_proxy_rules->reverse_bypass = data.reverse_bypass();
  return data.ReadBypassRules(&out_proxy_rules->bypass_rules) &&
         data.ReadType(&out_proxy_rules->type) &&
         data.ReadSingleProxies(&out_proxy_rules->single_proxies) &&
         data.ReadProxiesForHttp(&out_proxy_rules->proxies_for_http) &&
         data.ReadProxiesForHttps(&out_proxy_rules->proxies_for_https) &&
         data.ReadProxiesForFtp(&out_proxy_rules->proxies_for_ftp) &&
         data.ReadFallbackProxies(&out_proxy_rules->fallback_proxies);
}

content::mojom::ProxyConfigSource
EnumTraits<content::mojom::ProxyConfigSource, net::ProxyConfigSource>::ToMojom(
    net::ProxyConfigSource net_proxy_config_source) {
  switch (net_proxy_config_source) {
    case net::PROXY_CONFIG_SOURCE_UNKNOWN:
      return content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_UNKNOWN;
    case net::PROXY_CONFIG_SOURCE_SYSTEM:
      return content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_SYSTEM;
    case net::PROXY_CONFIG_SOURCE_SYSTEM_FAILED:
      return content::mojom::ProxyConfigSource::
          PROXY_CONFIG_SOURCE_SYSTEM_FAILED;
    case net::PROXY_CONFIG_SOURCE_GCONF:
      return content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_GCONF;
    case net::PROXY_CONFIG_SOURCE_GSETTINGS:
      return content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_GSETTINGS;
    case net::PROXY_CONFIG_SOURCE_KDE:
      return content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_KDE;
    case net::PROXY_CONFIG_SOURCE_ENV:
      return content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_ENV;
    case net::PROXY_CONFIG_SOURCE_CUSTOM:
      return content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_CUSTOM;
    case net::PROXY_CONFIG_SOURCE_TEST:
      return content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_TEST;
    case net::NUM_PROXY_CONFIG_SOURCES:
      break;
  }
  return content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_UNKNOWN;
}

bool EnumTraits<content::mojom::ProxyConfigSource, net::ProxyConfigSource>::
    FromMojom(content::mojom::ProxyConfigSource mojo_proxy_config_source,
              net::ProxyConfigSource* out) {
  switch (mojo_proxy_config_source) {
    case content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_UNKNOWN:
      *out = net::PROXY_CONFIG_SOURCE_UNKNOWN;
      return true;
    case content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_SYSTEM:
      *out = net::PROXY_CONFIG_SOURCE_SYSTEM;
      return true;
    case content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_SYSTEM_FAILED:
      *out = net::PROXY_CONFIG_SOURCE_SYSTEM_FAILED;
      return true;
    case content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_GCONF:
      *out = net::PROXY_CONFIG_SOURCE_GCONF;
      return true;
    case content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_GSETTINGS:
      *out = net::PROXY_CONFIG_SOURCE_GSETTINGS;
      return true;
    case content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_KDE:
      *out = net::PROXY_CONFIG_SOURCE_KDE;
      return true;
    case content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_ENV:
      *out = net::PROXY_CONFIG_SOURCE_ENV;
      return true;
    case content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_CUSTOM:
      *out = net::PROXY_CONFIG_SOURCE_CUSTOM;
      return true;
    case content::mojom::ProxyConfigSource::PROXY_CONFIG_SOURCE_TEST:
      *out = net::PROXY_CONFIG_SOURCE_TEST;
      return true;
  }
  return false;
}

bool StructTraits<content::mojom::ProxyConfigDataView, net::ProxyConfig>::Read(
    content::mojom::ProxyConfigDataView data,
    net::ProxyConfig* out_proxy_config) {
  GURL pac_url;
  net::ProxyConfigSource source;
  if (!data.ReadPacUrl(&pac_url) ||
      !data.ReadProxyRules(&out_proxy_config->proxy_rules()) ||
      !data.ReadSource(&source)) {
    return false;
  }
  out_proxy_config->set_pac_url(pac_url);
  out_proxy_config->set_source(source);

  out_proxy_config->set_auto_detect(data.auto_detect());
  out_proxy_config->set_pac_mandatory(data.pac_mandatory());
  out_proxy_config->set_id(data.id());
  return true;
}

}  // namespace mojo
