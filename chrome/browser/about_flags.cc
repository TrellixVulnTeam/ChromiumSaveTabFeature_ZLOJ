// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/about_flags.h"

#include <iterator>
#include <map>
#include <set>
#include <utility>

#include "base/base_switches.h"
#include "base/bind.h"
#include "base/callback.h"
#include "base/command_line.h"
#include "base/feature_list.h"
#include "base/i18n/base_i18n_switches.h"
#include "base/macros.h"
#include "base/memory/singleton.h"
#include "base/metrics/histogram_macros.h"
#include "base/metrics/metrics_hashes.h"
#include "base/stl_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task_scheduler/switches.h"
#include "base/values.h"
#include "build/build_config.h"
#include "cc/base/switches.h"
#include "chrome/browser/experiments/memory_ablation_experiment.h"
#include "chrome/browser/flag_descriptions.h"
#include "chrome/browser/predictors/loading_predictor_config.h"
#include "chrome/browser/predictors/resource_prefetch_common.h"
#include "chrome/browser/prerender/prerender_field_trial.h"
#include "chrome/browser/ui/blocked_content/tab_under_navigation_throttle.h"
#include "chrome/common/channel_info.h"
#include "chrome/common/chrome_content_client.h"
#include "chrome/common/chrome_features.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/features.h"
#include "chrome/common/pause_tabs_field_trial.h"
#include "components/autofill/core/browser/autofill_experiments.h"
#include "components/autofill/core/common/autofill_features.h"
#include "components/autofill/core/common/autofill_switches.h"
#include "components/autofill/core/common/autofill_util.h"
#include "components/browser_sync/browser_sync_switches.h"
#include "components/cloud_devices/common/cloud_devices_switches.h"
#include "components/data_reduction_proxy/core/common/data_reduction_proxy_features.h"
#include "components/data_reduction_proxy/core/common/data_reduction_proxy_switches.h"
#include "components/dom_distiller/core/dom_distiller_switches.h"
#include "components/error_page/common/error_page_switches.h"
#include "components/favicon/core/features.h"
#include "components/feature_engagement/public/feature_constants.h"
#include "components/feature_engagement/public/feature_list.h"
#include "components/flags_ui/feature_entry.h"
#include "components/flags_ui/feature_entry_macros.h"
#include "components/flags_ui/flags_storage.h"
#include "components/flags_ui/flags_ui_switches.h"
#include "components/nacl/common/features.h"
#include "components/nacl/common/nacl_switches.h"
#include "components/network_session_configurator/common/network_features.h"
#include "components/network_session_configurator/common/network_switches.h"
#include "components/ntp_snippets/features.h"
#include "components/ntp_snippets/ntp_snippets_constants.h"
#include "components/ntp_tiles/constants.h"
#include "components/ntp_tiles/switches.h"
#include "components/offline_pages/core/offline_page_feature.h"
#include "components/omnibox/browser/omnibox_field_trial.h"
#include "components/omnibox/browser/omnibox_switches.h"
#include "components/password_manager/core/common/password_manager_features.h"
#include "components/payments/core/features.h"
#include "components/previews/core/previews_features.h"
#include "components/proximity_auth/switches.h"
#include "components/search_provider_logos/features.h"
#include "components/search_provider_logos/switches.h"
#include "components/security_state/core/security_state.h"
#include "components/security_state/core/switches.h"
#include "components/signin/core/browser/profile_management_switches.h"
#include "components/signin/core/browser/signin_features.h"
#include "components/signin/core/browser/signin_switches.h"
#include "components/spellcheck/common/spellcheck_features.h"
#include "components/spellcheck/common/spellcheck_switches.h"
#include "components/spellcheck/spellcheck_build_features.h"
#include "components/ssl_config/ssl_config_switches.h"
#include "components/suggestions/features.h"
#include "components/sync/driver/sync_driver_switches.h"
#include "components/tracing/common/tracing_switches.h"
#include "components/translate/core/browser/translate_infobar_delegate.h"
#include "components/translate/core/browser/translate_prefs.h"
#include "components/translate/core/browser/translate_ranker_impl.h"
#include "components/version_info/version_info.h"
#include "content/public/common/content_features.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/feature_h264_with_openh264_ffmpeg.h"
#include "content/public/common/features.h"
#include "device/base/features.h"
#include "device/vr/features/features.h"
#include "extensions/features/features.h"
#include "google_apis/drive/drive_switches.h"
#include "gpu/config/gpu_switches.h"
#include "media/audio/audio_features.h"
#include "media/base/media_switches.h"
#include "media/media_features.h"
#include "media/midi/midi_switches.h"
#include "net/cert/cert_verify_proc_android.h"
#include "net/nqe/effective_connection_type.h"
#include "ppapi/features/features.h"
#include "printing/features/features.h"
#include "services/device/public/cpp/device_features.h"
#include "services/service_manager/sandbox/switches.h"
#include "third_party/libaom/av1_features.h"
#include "ui/app_list/app_list_features.h"
#include "ui/base/ui_base_features.h"
#include "ui/base/ui_base_switches.h"
#include "ui/compositor/compositor_switches.h"
#include "ui/display/display_switches.h"
#include "ui/events/event_switches.h"
#include "ui/gfx/switches.h"
#include "ui/gl/gl_features.h"
#include "ui/gl/gl_switches.h"
#include "ui/keyboard/keyboard_switches.h"
#include "ui/native_theme/native_theme_features.h"
#include "ui/views/views_switches.h"

#if defined(OS_ANDROID)
#include "chrome/browser/android/chrome_feature_list.h"
#else  // OS_ANDROID
#include "ui/message_center/public/cpp/message_center_switches.h"
#endif  // OS_ANDROID

#if defined(OS_CHROMEOS)
#include "ash/public/cpp/ash_switches.h"
#include "chromeos/chromeos_switches.h"
#include "components/arc/arc_features.h"
#include "components/ui_devtools/switches.h"
#include "third_party/cros_system_api/switches/chrome_switches.h"
#endif  // OS_CHROMEOS

#if defined(OS_MACOSX)
#include "chrome/browser/ui/browser_dialogs.h"
#endif  // OS_MACOSX

#if BUILDFLAG(ENABLE_APP_LIST)
#include "ui/app_list/app_list_switches.h"
#endif  // ENABLE_APP_LIST

#if BUILDFLAG(ENABLE_EXTENSIONS)
#include "extensions/common/switches.h"
#endif  // ENABLE_EXTENSIONS

#if defined(USE_OZONE)
#include "ui/ozone/public/ozone_switches.h"
#endif  // USE_OZONE

#if defined(OS_WIN)
#include "base/win/windows_version.h"
#endif  // OS_WIN

#if defined(TOOLKIT_VIEWS)
#include "chrome/browser/ui/tabs/tab_features.h"
#endif

using flags_ui::FeatureEntry;
using flags_ui::kOsAndroid;
using flags_ui::kOsCrOS;
using flags_ui::kOsCrOSOwnerOnly;
using flags_ui::kOsLinux;
using flags_ui::kOsMac;
using flags_ui::kOsWin;

namespace about_flags {

namespace {

const unsigned kOsAll = kOsMac | kOsWin | kOsLinux | kOsCrOS | kOsAndroid;
const unsigned kOsDesktop = kOsMac | kOsWin | kOsLinux | kOsCrOS;

#if defined(USE_AURA)
const unsigned kOsAura = kOsWin | kOsLinux | kOsCrOS;
#endif  // USE_AURA

const FeatureEntry::Choice kTouchEventFeatureDetectionChoices[] = {
    {flags_ui::kGenericExperimentChoiceAutomatic, "", ""},
    {flags_ui::kGenericExperimentChoiceEnabled,
     switches::kTouchEventFeatureDetection,
     switches::kTouchEventFeatureDetectionEnabled},
    {flags_ui::kGenericExperimentChoiceDisabled,
     switches::kTouchEventFeatureDetection,
     switches::kTouchEventFeatureDetectionDisabled}};

#if defined(USE_AURA)
const FeatureEntry::Choice kOverscrollHistoryNavigationChoices[] = {
    {flags_ui::kGenericExperimentChoiceEnabled, "", ""},
    {flags_ui::kGenericExperimentChoiceDisabled,
     switches::kOverscrollHistoryNavigation, "0"},
    {flag_descriptions::kOverscrollHistoryNavigationSimpleUi,
     switches::kOverscrollHistoryNavigation, "2"}};

const FeatureEntry::Choice kOverscrollStartThresholdChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kOverscrollStartThreshold133Percent,
     switches::kOverscrollStartThreshold, "133"},
    {flag_descriptions::kOverscrollStartThreshold166Percent,
     switches::kOverscrollStartThreshold, "166"},
    {flag_descriptions::kOverscrollStartThreshold200Percent,
     switches::kOverscrollStartThreshold, "200"}};
#endif  // USE_AURA

const FeatureEntry::Choice kTouchTextSelectionStrategyChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kTouchSelectionStrategyCharacter,
     switches::kTouchTextSelectionStrategy, "character"},
    {flag_descriptions::kTouchSelectionStrategyDirection,
     switches::kTouchTextSelectionStrategy, "direction"}};

const FeatureEntry::Choice kTraceUploadURL[] = {
    {flags_ui::kGenericExperimentChoiceDisabled, "", ""},
    {flag_descriptions::kTraceUploadUrlChoiceOther, switches::kTraceUploadURL,
     "https://performance-insights.appspot.com/upload?tags=flags,Other"},
    {flag_descriptions::kTraceUploadUrlChoiceEmloading,
     switches::kTraceUploadURL,
     "https://performance-insights.appspot.com/upload?tags=flags,emloading"},
    {flag_descriptions::kTraceUploadUrlChoiceQa, switches::kTraceUploadURL,
     "https://performance-insights.appspot.com/upload?tags=flags,QA"},
    {flag_descriptions::kTraceUploadUrlChoiceTesting, switches::kTraceUploadURL,
     "https://performance-insights.appspot.com/upload?tags=flags,TestingTeam"}};

#if BUILDFLAG(ENABLE_NACL)
const FeatureEntry::Choice kNaClDebugMaskChoices[] = {
    // Secure shell can be used on ChromeOS for forwarding the TCP port opened
    // by
    // debug stub to a remote machine. Since secure shell uses NaCl, we usually
    // want to avoid debugging that. The PNaCl translator is also a NaCl module,
    // so by default we want to avoid debugging that.
    // NOTE: As the default value must be the empty string, the mask excluding
    // the PNaCl translator and secure shell is substituted elsewhere.
    {flag_descriptions::kNaclDebugMaskChoiceExcludeUtilsPnacl, "", ""},
    {flag_descriptions::kNaclDebugMaskChoiceDebugAll, switches::kNaClDebugMask,
     "*://*"},
    {flag_descriptions::kNaclDebugMaskChoiceIncludeDebug,
     switches::kNaClDebugMask, "*://*/*debug.nmf"}};
#endif  // ENABLE_NACL

const FeatureEntry::Choice kPassiveListenersChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kPassiveEventListenerTrue,
     switches::kPassiveListenersDefault, "true"},
    {flag_descriptions::kPassiveEventListenerForceAllTrue,
     switches::kPassiveListenersDefault, "forcealltrue"},
};

const FeatureEntry::Choice kMarkHttpAsChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kMarkHttpAsDangerous,
     security_state::switches::kMarkHttpAs,
     security_state::switches::kMarkHttpAsDangerous}};

const FeatureEntry::Choice kDataReductionProxyLoFiChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kDataReductionProxyLoFiAlwaysOn,
     data_reduction_proxy::switches::kDataReductionProxyLoFi,
     data_reduction_proxy::switches::kDataReductionProxyLoFiValueAlwaysOn},
    {flag_descriptions::kDataReductionProxyLoFiCellularOnly,
     data_reduction_proxy::switches::kDataReductionProxyLoFi,
     data_reduction_proxy::switches::kDataReductionProxyLoFiValueCellularOnly},
    {flag_descriptions::kDataReductionProxyLoFiDisabled,
     data_reduction_proxy::switches::kDataReductionProxyLoFi,
     data_reduction_proxy::switches::kDataReductionProxyLoFiValueDisabled},
    {flag_descriptions::kDataReductionProxyLoFiSlowConnectionsOnly,
     data_reduction_proxy::switches::kDataReductionProxyLoFi,
     data_reduction_proxy::switches::
         kDataReductionProxyLoFiValueSlowConnectionsOnly}};

const FeatureEntry::Choice kDataReductionProxyServerExperiment[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kDataReductionProxyServerAlternative1,
     data_reduction_proxy::switches::kDataReductionProxyExperiment,
     data_reduction_proxy::switches::kDataReductionProxyServerAlternative1},
    {flag_descriptions::kDataReductionProxyServerAlternative2,
     data_reduction_proxy::switches::kDataReductionProxyExperiment,
     data_reduction_proxy::switches::kDataReductionProxyServerAlternative2},
    {flag_descriptions::kDataReductionProxyServerAlternative3,
     data_reduction_proxy::switches::kDataReductionProxyExperiment,
     data_reduction_proxy::switches::kDataReductionProxyServerAlternative3},
    {flag_descriptions::kDataReductionProxyServerAlternative4,
     data_reduction_proxy::switches::kDataReductionProxyExperiment,
     data_reduction_proxy::switches::kDataReductionProxyServerAlternative4},
    {flag_descriptions::kDataReductionProxyServerAlternative5,
     data_reduction_proxy::switches::kDataReductionProxyExperiment,
     data_reduction_proxy::switches::kDataReductionProxyServerAlternative5},
    {flag_descriptions::kDataReductionProxyServerAlternative6,
     data_reduction_proxy::switches::kDataReductionProxyExperiment,
     data_reduction_proxy::switches::kDataReductionProxyServerAlternative6},
    {flag_descriptions::kDataReductionProxyServerAlternative7,
     data_reduction_proxy::switches::kDataReductionProxyExperiment,
     data_reduction_proxy::switches::kDataReductionProxyServerAlternative7},
    {flag_descriptions::kDataReductionProxyServerAlternative8,
     data_reduction_proxy::switches::kDataReductionProxyExperiment,
     data_reduction_proxy::switches::kDataReductionProxyServerAlternative8},
    {flag_descriptions::kDataReductionProxyServerAlternative9,
     data_reduction_proxy::switches::kDataReductionProxyExperiment,
     data_reduction_proxy::switches::kDataReductionProxyServerAlternative9},
    {flag_descriptions::kDataReductionProxyServerAlternative10,
     data_reduction_proxy::switches::kDataReductionProxyExperiment,
     data_reduction_proxy::switches::kDataReductionProxyServerAlternative10}};

const FeatureEntry::Choice kShowSavedCopyChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kEnableShowSavedCopyPrimary,
     error_page::switches::kShowSavedCopy,
     error_page::switches::kEnableShowSavedCopyPrimary},
    {flag_descriptions::kEnableShowSavedCopySecondary,
     error_page::switches::kShowSavedCopy,
     error_page::switches::kEnableShowSavedCopySecondary},
    {flag_descriptions::kDisableShowSavedCopy,
     error_page::switches::kShowSavedCopy,
     error_page::switches::kDisableShowSavedCopy}};

const FeatureEntry::Choice kDefaultTileWidthChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kDefaultTileWidthShort, switches::kDefaultTileWidth,
     "128"},
    {flag_descriptions::kDefaultTileWidthTall, switches::kDefaultTileWidth,
     "256"},
    {flag_descriptions::kDefaultTileWidthGrande, switches::kDefaultTileWidth,
     "512"},
    {flag_descriptions::kDefaultTileWidthVenti, switches::kDefaultTileWidth,
     "1024"}};

const FeatureEntry::Choice kDefaultTileHeightChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kDefaultTileHeightShort, switches::kDefaultTileHeight,
     "128"},
    {flag_descriptions::kDefaultTileHeightTall, switches::kDefaultTileHeight,
     "256"},
    {flag_descriptions::kDefaultTileHeightGrande, switches::kDefaultTileHeight,
     "512"},
    {flag_descriptions::kDefaultTileHeightVenti, switches::kDefaultTileHeight,
     "1024"}};

#if !BUILDFLAG(ENABLE_MIRROR)

const FeatureEntry::FeatureParam kAccountConsistencyMirror[] = {
    {signin::kAccountConsistencyFeatureMethodParameter,
     signin::kAccountConsistencyFeatureMethodMirror}};

#if BUILDFLAG(ENABLE_DICE_SUPPORT)
const FeatureEntry::FeatureParam kAccountConsistencyDice[] = {
    {signin::kAccountConsistencyFeatureMethodParameter,
     signin::kAccountConsistencyFeatureMethodDice}};

const FeatureEntry::FeatureParam kAccountConsistencyDicePrepareMigration[] = {
    {signin::kAccountConsistencyFeatureMethodParameter,
     signin::kAccountConsistencyFeatureMethodDicePrepareMigration}};

const FeatureEntry::FeatureParam
    kAccountConsistencyDicePrepareMigrationChromeSyncEndpoint[] = {
        {signin::kAccountConsistencyFeatureMethodParameter,
         signin::
             kAccountConsistencyFeatureMethodDicePrepareMigrationChromeSyncEndpoint}};

const FeatureEntry::FeatureParam kAccountConsistencyDiceMigration[] = {
    {signin::kAccountConsistencyFeatureMethodParameter,
     signin::kAccountConsistencyFeatureMethodDiceMigration}};

const FeatureEntry::FeatureParam kAccountConsistencyDiceFixAuthErrors[] = {
    {signin::kAccountConsistencyFeatureMethodParameter,
     signin::kAccountConsistencyFeatureMethodDiceFixAuthErrors}};
#endif  // BUILDFLAG(ENABLE_DICE_SUPPORT)

const FeatureEntry::FeatureVariation kAccountConsistencyFeatureVariations[] = {
    {"Mirror", kAccountConsistencyMirror, arraysize(kAccountConsistencyMirror),
     nullptr /* variation_id */}
#if BUILDFLAG(ENABLE_DICE_SUPPORT)
    ,
    {"Dice", kAccountConsistencyDice, arraysize(kAccountConsistencyDice),
     nullptr /* variation_id */},
    {"Dice (migration)", kAccountConsistencyDiceMigration,
     arraysize(kAccountConsistencyDiceMigration), nullptr /* variation_id */},
    {"Dice (prepare migration)", kAccountConsistencyDicePrepareMigration,
     arraysize(kAccountConsistencyDicePrepareMigration),
     nullptr /* variation_id */},
    {"Dice (prepare migration, Chrome sync endpoint)",
     kAccountConsistencyDicePrepareMigrationChromeSyncEndpoint,
     arraysize(kAccountConsistencyDicePrepareMigrationChromeSyncEndpoint),
     nullptr /* variation_id */},
    {"Dice (fix auth errors)", kAccountConsistencyDiceFixAuthErrors,
     arraysize(kAccountConsistencyDiceFixAuthErrors),
     nullptr /* variation_id */}
#endif  // BUILDFLAG(ENABLE_DICE_SUPPORT)
};

#endif  // !BUILDFLAG(ENABLE_MIRROR)

const FeatureEntry::Choice kSimpleCacheBackendChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flags_ui::kGenericExperimentChoiceDisabled,
     switches::kUseSimpleCacheBackend, "off"},
    {flags_ui::kGenericExperimentChoiceEnabled,
     switches::kUseSimpleCacheBackend, "on"}};

#if defined(OS_ANDROID)
const FeatureEntry::Choice kReaderModeHeuristicsChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kReaderModeHeuristicsMarkup,
     switches::kReaderModeHeuristics,
     switches::reader_mode_heuristics::kOGArticle},
    {flag_descriptions::kReaderModeHeuristicsAdaboost,
     switches::kReaderModeHeuristics,
     switches::reader_mode_heuristics::kAdaBoost},
    {flag_descriptions::kReaderModeHeuristicsAlwaysOn,
     switches::kReaderModeHeuristics,
     switches::reader_mode_heuristics::kAlwaysTrue},
    {flag_descriptions::kReaderModeHeuristicsAlwaysOff,
     switches::kReaderModeHeuristics, switches::reader_mode_heuristics::kNone},
    {flag_descriptions::kReaderModeHeuristicsAllArticles,
     switches::kReaderModeHeuristics,
     switches::reader_mode_heuristics::kAllArticles},
};

const FeatureEntry::Choice kChromeHomeSwipeLogicChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kChromeHomeSwipeLogicRestrictArea,
     switches::kChromeHomeSwipeLogicType, "restrict-area"},
    {flag_descriptions::kChromeHomeSwipeLogicVelocity,
     switches::kChromeHomeSwipeLogicType, "velocity"},
};

#endif  // OS_ANDROID

const FeatureEntry::Choice kNumRasterThreadsChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kNumRasterThreadsOne, switches::kNumRasterThreads, "1"},
    {flag_descriptions::kNumRasterThreadsTwo, switches::kNumRasterThreads, "2"},
    {flag_descriptions::kNumRasterThreadsThree, switches::kNumRasterThreads,
     "3"},
    {flag_descriptions::kNumRasterThreadsFour, switches::kNumRasterThreads,
     "4"}};

const FeatureEntry::Choice kGpuRasterizationMSAASampleCountChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kGpuRasterizationMsaaSampleCountZero,
     switches::kGpuRasterizationMSAASampleCount, "0"},
    {flag_descriptions::kGpuRasterizationMsaaSampleCountTwo,
     switches::kGpuRasterizationMSAASampleCount, "2"},
    {flag_descriptions::kGpuRasterizationMsaaSampleCountFour,
     switches::kGpuRasterizationMSAASampleCount, "4"},
    {flag_descriptions::kGpuRasterizationMsaaSampleCountEight,
     switches::kGpuRasterizationMSAASampleCount, "8"},
    {flag_descriptions::kGpuRasterizationMsaaSampleCountSixteen,
     switches::kGpuRasterizationMSAASampleCount, "16"},
};

const FeatureEntry::Choice kMHTMLGeneratorOptionChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kMhtmlSkipNostoreMain, switches::kMHTMLGeneratorOption,
     switches::kMHTMLSkipNostoreMain},
    {flag_descriptions::kMhtmlSkipNostoreAll, switches::kMHTMLGeneratorOption,
     switches::kMHTMLSkipNostoreAll},
};

const FeatureEntry::Choice kEnableGpuRasterizationChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flags_ui::kGenericExperimentChoiceEnabled,
     switches::kEnableGpuRasterization, ""},
    {flags_ui::kGenericExperimentChoiceDisabled,
     switches::kDisableGpuRasterization, ""},
    {flag_descriptions::kForceGpuRasterization,
     switches::kForceGpuRasterization, ""},
};

#if defined(OS_CHROMEOS)
const FeatureEntry::Choice kMemoryPressureThresholdChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kConservativeThresholds,
     chromeos::switches::kMemoryPressureThresholds,
     chromeos::switches::kConservativeThreshold},
    {flag_descriptions::kAggressiveCacheDiscardThresholds,
     chromeos::switches::kMemoryPressureThresholds,
     chromeos::switches::kAggressiveCacheDiscardThreshold},
    {flag_descriptions::kAggressiveTabDiscardThresholds,
     chromeos::switches::kMemoryPressureThresholds,
     chromeos::switches::kAggressiveTabDiscardThreshold},
    {flag_descriptions::kAggressiveThresholds,
     chromeos::switches::kMemoryPressureThresholds,
     chromeos::switches::kAggressiveThreshold},
};
#endif  // OS_CHROMEOS

const FeatureEntry::Choice kExtensionContentVerificationChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kExtensionContentVerificationBootstrap,
     switches::kExtensionContentVerification,
     switches::kExtensionContentVerificationBootstrap},
    {flag_descriptions::kExtensionContentVerificationEnforce,
     switches::kExtensionContentVerification,
     switches::kExtensionContentVerificationEnforce},
    {flag_descriptions::kExtensionContentVerificationEnforceStrict,
     switches::kExtensionContentVerification,
     switches::kExtensionContentVerificationEnforceStrict},
};

const FeatureEntry::Choice kTopChromeMaterialDesignChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kTopChromeMdMaterial, switches::kTopChromeMD,
     switches::kTopChromeMDMaterial},
    {flag_descriptions::kTopChromeMdMaterialHybrid, switches::kTopChromeMD,
     switches::kTopChromeMDMaterialHybrid},
    {flag_descriptions::kTopChromeMdMaterialAuto, switches::kTopChromeMD,
     switches::kTopChromeMDMaterialAuto},
};

#if defined(OS_CHROMEOS)
const FeatureEntry::Choice kAshShelfColorChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flags_ui::kGenericExperimentChoiceEnabled, ash::switches::kAshShelfColor,
     ash::switches::kAshShelfColorEnabled},
    {flags_ui::kGenericExperimentChoiceDisabled, ash::switches::kAshShelfColor,
     ash::switches::kAshShelfColorDisabled}};

const FeatureEntry::Choice kAshShelfColorSchemeChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kAshShelfColorSchemeLightVibrant,
     ash::switches::kAshShelfColorScheme,
     ash::switches::kAshShelfColorSchemeLightVibrant},
    {flag_descriptions::kAshShelfColorSchemeNormalVibrant,
     ash::switches::kAshShelfColorScheme,
     ash::switches::kAshShelfColorSchemeNormalVibrant},
    {flag_descriptions::kAshShelfColorSchemeDarkVibrant,
     ash::switches::kAshShelfColorScheme,
     ash::switches::kAshShelfColorSchemeDarkVibrant},
    {flag_descriptions::kAshShelfColorSchemeLightMuted,
     ash::switches::kAshShelfColorScheme,
     ash::switches::kAshShelfColorSchemeLightMuted},
    {flag_descriptions::kAshShelfColorSchemeNormalMuted,
     ash::switches::kAshShelfColorScheme,
     ash::switches::kAshShelfColorSchemeNormalMuted},
    {flag_descriptions::kAshShelfColorSchemeDarkMuted,
     ash::switches::kAshShelfColorScheme,
     ash::switches::kAshShelfColorSchemeDarkMuted},
};

const FeatureEntry::Choice kAshMaterialDesignInkDropAnimationSpeed[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kMaterialDesignInkDropAnimationFast,
     switches::kMaterialDesignInkDropAnimationSpeed,
     switches::kMaterialDesignInkDropAnimationSpeedFast},
    {flag_descriptions::kMaterialDesignInkDropAnimationSlow,
     switches::kMaterialDesignInkDropAnimationSpeed,
     switches::kMaterialDesignInkDropAnimationSpeedSlow}};

const FeatureEntry::Choice kDataSaverPromptChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flags_ui::kGenericExperimentChoiceEnabled,
     chromeos::switches::kEnableDataSaverPrompt, ""},
    {flags_ui::kGenericExperimentChoiceDisabled,
     chromeos::switches::kDisableDataSaverPrompt, ""},
    {flag_descriptions::kDatasaverPromptDemoMode,
     chromeos::switches::kEnableDataSaverPrompt,
     chromeos::switches::kDataSaverPromptDemoMode},
};

const FeatureEntry::Choice kUseMusChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kEnableMusDescription, switches::kMus, ""},
    {flag_descriptions::kEnableMashDescription, switches::kMash, ""},
};

const FeatureEntry::Choice kUiShowCompositedLayerBordersChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kUiShowCompositedLayerBordersRenderPass,
     cc::switches::kUIShowCompositedLayerBorders,
     cc::switches::kCompositedRenderPassBorders},
    {flag_descriptions::kUiShowCompositedLayerBordersSurface,
     cc::switches::kUIShowCompositedLayerBorders,
     cc::switches::kCompositedSurfaceBorders},
    {flag_descriptions::kUiShowCompositedLayerBordersLayer,
     cc::switches::kUIShowCompositedLayerBorders,
     cc::switches::kCompositedLayerBorders},
    {flag_descriptions::kUiShowCompositedLayerBordersAll,
     cc::switches::kUIShowCompositedLayerBorders, ""}};

#endif  // OS_CHROMEOS

const FeatureEntry::Choice kV8CacheOptionsChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flags_ui::kGenericExperimentChoiceDisabled, switches::kV8CacheOptions,
     "none"},
    {flag_descriptions::kV8CacheOptionsParse, switches::kV8CacheOptions,
     "parse"},
    {flag_descriptions::kV8CacheOptionsCode, switches::kV8CacheOptions, "code"},
};

#if defined(OS_ANDROID)
const FeatureEntry::Choice kProgressBarCompletionChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kProgressBarCompletionLoadEvent,
     switches::kProgressBarCompletion, "loadEvent"},
    {flag_descriptions::kProgressBarCompletionResourcesBeforeDcl,
     switches::kProgressBarCompletion, "resourcesBeforeDOMContentLoaded"},
    {flag_descriptions::kProgressBarCompletionDomContentLoaded,
     switches::kProgressBarCompletion, "domContentLoaded"},
    {flag_descriptions::
         kProgressBarCompletionResourcesBeforeDclAndSameOriginIframes,
     switches::kProgressBarCompletion,
     "resourcesBeforeDOMContentLoadedAndSameOriginIFrames"},
};
#endif  // OS_ANDROID

const FeatureEntry::Choice kSavePreviousDocumentResourcesChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kSavePreviousDocumentResourcesNever,
     switches::kSavePreviousDocumentResources, "never"},
    {flag_descriptions::kSavePreviousDocumentResourcesUntilOnDOMContentLoaded,
     switches::kSavePreviousDocumentResources, "onDOMContentLoaded"},
    {flag_descriptions::kSavePreviousDocumentResourcesUntilOnLoad,
     switches::kSavePreviousDocumentResources, "onload"},
};

#if defined(OS_CHROMEOS)
const FeatureEntry::Choice kCrosRegionsModeChoices[] = {
    {flag_descriptions::kCrosRegionsModeDefault, "", ""},
    {flag_descriptions::kCrosRegionsModeOverride,
     chromeos::switches::kCrosRegionsMode,
     chromeos::switches::kCrosRegionsModeOverride},
    {flag_descriptions::kCrosRegionsModeHide,
     chromeos::switches::kCrosRegionsMode,
     chromeos::switches::kCrosRegionsModeHide},
};
#endif  // OS_CHROMEOS

const FeatureEntry::Choice kForceUIDirectionChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kForceDirectionLtr, switches::kForceUIDirection,
     switches::kForceDirectionLTR},
    {flag_descriptions::kForceDirectionRtl, switches::kForceUIDirection,
     switches::kForceDirectionRTL},
};

const FeatureEntry::Choice kForceTextDirectionChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kForceDirectionLtr, switches::kForceTextDirection,
     switches::kForceDirectionLTR},
    {flag_descriptions::kForceDirectionRtl, switches::kForceTextDirection,
     switches::kForceDirectionRTL},
};

#if defined(OS_CHROMEOS)
const FeatureEntry::Choice kAshUiModeChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kUiModeTablet, ash::switches::kAshUiMode,
     ash::switches::kAshUiModeTablet},
    {flag_descriptions::kUiModeClamshell, ash::switches::kAshUiMode,
     ash::switches::kAshUiModeClamshell},
    {flag_descriptions::kUiModeAuto, ash::switches::kAshUiMode,
     ash::switches::kAshUiModeAuto},
};
#endif  // OS_CHROMEOS

#if defined(OS_ANDROID)
const FeatureEntry::FeatureParam
    kContentSuggestionsCategoryOrderFeatureVariationGeneral[] = {
        {ntp_snippets::kCategoryOrderParameter,
         ntp_snippets::kCategoryOrderGeneral},
};

const FeatureEntry::FeatureParam
    kContentSuggestionsCategoryOrderFeatureVariationEMOriented[] = {
        {ntp_snippets::kCategoryOrderParameter,
         ntp_snippets::kCategoryOrderEmergingMarketsOriented},
};

const FeatureEntry::FeatureVariation
    kContentSuggestionsCategoryOrderFeatureVariations[] = {
        {"(general)", kContentSuggestionsCategoryOrderFeatureVariationGeneral,
         arraysize(kContentSuggestionsCategoryOrderFeatureVariationGeneral),
         nullptr},
        {"(emerging markets oriented)",
         kContentSuggestionsCategoryOrderFeatureVariationEMOriented,
         arraysize(kContentSuggestionsCategoryOrderFeatureVariationEMOriented),
         nullptr}};

const FeatureEntry::FeatureParam
    kContentSuggestionsCategoryRankerFeatureVariationConstant[] = {
        {ntp_snippets::kCategoryRankerParameter,
         ntp_snippets::kCategoryRankerConstantRanker},
};

const FeatureEntry::FeatureParam
    kContentSuggestionsCategoryRankerFeatureVariationClickBased[] = {
        {ntp_snippets::kCategoryRankerParameter,
         ntp_snippets::kCategoryRankerClickBasedRanker},
};

const FeatureEntry::FeatureVariation
    kContentSuggestionsCategoryRankerFeatureVariations[] = {
        {"(constant)",
         kContentSuggestionsCategoryRankerFeatureVariationConstant,
         arraysize(kContentSuggestionsCategoryRankerFeatureVariationConstant),
         nullptr},
        {"(click based)",
         kContentSuggestionsCategoryRankerFeatureVariationClickBased,
         arraysize(kContentSuggestionsCategoryRankerFeatureVariationClickBased),
         nullptr}};
#endif  // OS_ANDROID

#if defined(OS_ANDROID)
const FeatureEntry::FeatureParam kForceFetchedSuggestionsNotifications[] = {
    {"force_fetched_suggestions_notifications", "true"},
    {"enable_fetched_suggestions_notifications", "true"}};

const FeatureEntry::FeatureVariation
    kContentSuggestionsNotificationsFeatureVariations[] = {
        {"(notify always, server side)", nullptr, 0, "3313312"},
        {"(notify always, client side)", kForceFetchedSuggestionsNotifications,
         arraysize(kForceFetchedSuggestionsNotifications), nullptr}};
#endif  // OS_ANDROID

#if defined(OS_ANDROID)
const FeatureEntry::FeatureVariation kRemoteSuggestionsFeatureVariations[] = {
    {"via content suggestion server (backed by ChromeReader)", nullptr, 0,
     "3313421"},
    {"via content suggestion server (backed by Google Now)", nullptr, 0,
     "3313422"}};
#endif  // OS_ANDROID

#if defined(OS_ANDROID)
const FeatureEntry::FeatureParam kCondensedTileLayoutForSmallScreensEnabled[] =
    {{"condensed_tile_layout_for_small_screens_enabled", "true"}};

const FeatureEntry::FeatureParam kCondensedTileLayoutForLargeScreensEnabled[] =
    {{"condensed_tile_layout_for_large_screens_enabled", "true"}};

const FeatureEntry::FeatureVariation
    kNTPCondensedTileLayoutFeatureVariations[] = {
        {"(small screens)", kCondensedTileLayoutForSmallScreensEnabled,
         arraysize(kCondensedTileLayoutForSmallScreensEnabled), nullptr},
        {"(large screens)", kCondensedTileLayoutForLargeScreensEnabled,
         arraysize(kCondensedTileLayoutForLargeScreensEnabled), nullptr}};
#endif  // OS_ANDROID

#if defined(OS_ANDROID)
const FeatureEntry::Choice kHerbPrototypeChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flags_ui::kGenericExperimentChoiceDisabled,
     switches::kTabManagementExperimentTypeDisabled, ""},
    {flag_descriptions::kHerbPrototypeFlavorElderberry,
     switches::kTabManagementExperimentTypeElderberry, ""},
};
#endif  // OS_ANDROID

const FeatureEntry::Choice kEnableUseZoomForDSFChoices[] = {
    {flag_descriptions::kEnableUseZoomForDsfChoiceDefault, "", ""},
    {flag_descriptions::kEnableUseZoomForDsfChoiceEnabled,
     switches::kEnableUseZoomForDSF, "true"},
    {flag_descriptions::kEnableUseZoomForDsfChoiceDisabled,
     switches::kEnableUseZoomForDSF, "false"},
};

const FeatureEntry::Choice kTLS13VariantChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kTLS13VariantDisabled, switches::kTLS13Variant,
     switches::kTLS13VariantDisabled},
    {flag_descriptions::kTLS13VariantDraft, switches::kTLS13Variant,
     switches::kTLS13VariantDraft},
    {flag_descriptions::kTLS13VariantExperiment, switches::kTLS13Variant,
     switches::kTLS13VariantExperiment},
    // The RecordType variant was deprecated.
    {flag_descriptions::kTLS13VariantDeprecated, switches::kTLS13Variant,
     switches::kTLS13VariantDisabled},
    // The NoSessionID variant was deprecated.
    {flag_descriptions::kTLS13VariantDeprecated, switches::kTLS13Variant,
     switches::kTLS13VariantDisabled},
    {flag_descriptions::kTLS13VariantExperiment2, switches::kTLS13Variant,
     switches::kTLS13VariantExperiment2},
    {flag_descriptions::kTLS13VariantExperiment3, switches::kTLS13Variant,
     switches::kTLS13VariantExperiment3},
};

#if !defined(OS_ANDROID)
const FeatureEntry::Choice kEnableAudioFocusChoices[] = {
    {flag_descriptions::kEnableAudioFocusDisabled, "", ""},
    {flag_descriptions::kEnableAudioFocusEnabled, switches::kEnableAudioFocus,
     ""},
#if BUILDFLAG(ENABLE_PLUGINS)
    {flag_descriptions::kEnableAudioFocusEnabledDuckFlash,
     switches::kEnableAudioFocus, switches::kEnableAudioFocusDuckFlash},
#endif  // BUILDFLAG(ENABLE_PLUGINS)
};
#endif  // !defined(OS_ANDROID)

const FeatureEntry::Choice kForceColorProfileChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kForceColorProfileSRGB, switches::kForceColorProfile,
     "srgb"},
    {flag_descriptions::kForceColorProfileP3, switches::kForceColorProfile,
     "display-p3-d65"},
    {flag_descriptions::kForceColorProfileColorSpin,
     switches::kForceColorProfile, "color-spin-gamma24"},
    {flag_descriptions::kForceColorProfileHdr, switches::kForceColorProfile,
     "scrgb-linear"},
};

const FeatureEntry::Choice kAutoplayPolicyChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kAutoplayPolicyNoUserGestureRequired,
     switches::kAutoplayPolicy,
     switches::autoplay::kNoUserGestureRequiredPolicy},
#if defined(OS_ANDROID)
    {flag_descriptions::kAutoplayPolicyUserGestureRequired,
     switches::kAutoplayPolicy, switches::autoplay::kUserGestureRequiredPolicy},
#else
    {flag_descriptions::kAutoplayPolicyUserGestureRequiredForCrossOrigin,
     switches::kAutoplayPolicy,
     switches::autoplay::kUserGestureRequiredForCrossOriginPolicy},
#endif
    {flag_descriptions::kAutoplayPolicyDocumentUserActivation,
     switches::kAutoplayPolicy,
     switches::autoplay::kDocumentUserActivationRequiredPolicy},
};

const FeatureEntry::Choice kForceEffectiveConnectionTypeChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kEffectiveConnectionTypeUnknownDescription,
     switches::kForceEffectiveConnectionType,
     net::kEffectiveConnectionTypeUnknown},
    {flag_descriptions::kEffectiveConnectionTypeOfflineDescription,
     switches::kForceEffectiveConnectionType,
     net::kEffectiveConnectionTypeOffline},
    {flag_descriptions::kEffectiveConnectionTypeSlow2GDescription,
     switches::kForceEffectiveConnectionType,
     net::kEffectiveConnectionTypeSlow2G},
    {flag_descriptions::kEffectiveConnectionType2GDescription,
     switches::kForceEffectiveConnectionType, net::kEffectiveConnectionType2G},
    {flag_descriptions::kEffectiveConnectionType3GDescription,
     switches::kForceEffectiveConnectionType, net::kEffectiveConnectionType3G},
    {flag_descriptions::kEffectiveConnectionType4GDescription,
     switches::kForceEffectiveConnectionType, net::kEffectiveConnectionType4G},
};

// Ensure that all effective connection types returned by Network Quality
// Estimator (NQE) are also exposed via flags.
static_assert(net::EFFECTIVE_CONNECTION_TYPE_LAST + 1 ==
                  arraysize(kForceEffectiveConnectionTypeChoices),
              "ECT enum value is not handled.");
static_assert(net::EFFECTIVE_CONNECTION_TYPE_UNKNOWN == 0,
              "ECT enum value is not handled.");
static_assert(net::EFFECTIVE_CONNECTION_TYPE_4G + 1 ==
                  net::EFFECTIVE_CONNECTION_TYPE_LAST,
              "ECT enum value is not handled.");

const FeatureEntry::FeatureParam kNoStatePrefetchEnabled[] = {
    {prerender::kNoStatePrefetchFeatureModeParameterName,
     prerender::kNoStatePrefetchFeatureModeParameterPrefetch}};

const FeatureEntry::FeatureParam kNoStatePrefetchPrerender[] = {
    {prerender::kNoStatePrefetchFeatureModeParameterName,
     prerender::kNoStatePrefetchFeatureModeParameterPrerender}};

const FeatureEntry::FeatureParam kNoStatePrefetchSimpleLoad[] = {
    {prerender::kNoStatePrefetchFeatureModeParameterName,
     prerender::kNoStatePrefetchFeatureModeParameterSimpleLoad}};

const FeatureEntry::FeatureVariation kNoStatePrefetchFeatureVariations[] = {
    {"No-state prefetch", kNoStatePrefetchEnabled,
     arraysize(kNoStatePrefetchEnabled), nullptr},
    {"Prerender", kNoStatePrefetchPrerender,
     arraysize(kNoStatePrefetchPrerender), nullptr},
    {"Simple load", kNoStatePrefetchSimpleLoad,
     arraysize(kNoStatePrefetchSimpleLoad), nullptr}};

const FeatureEntry::FeatureParam kSpeculativeResourcePrefetchingPrefetching[] =
    {{predictors::kModeParamName, predictors::kPrefetchingMode}};

const FeatureEntry::FeatureParam kSpeculativeResourcePrefetchingLearning[] = {
    {predictors::kModeParamName, predictors::kLearningMode}};

const FeatureEntry::FeatureVariation
    kSpeculativeResourcePrefetchingFeatureVariations[] = {
        {"Prefetching", kSpeculativeResourcePrefetchingPrefetching,
         arraysize(kSpeculativeResourcePrefetchingPrefetching), nullptr},
        {"Learning", kSpeculativeResourcePrefetchingLearning,
         arraysize(kSpeculativeResourcePrefetchingLearning), nullptr}};

#if defined(OS_LINUX) || defined(OS_MACOSX) || defined(OS_WIN)
const FeatureEntry::FeatureParam kPauseBackgroundTabsMinimalEngagment[] = {
    {pausetabs::kFeatureName, pausetabs::kModeParamMinimal}};

const FeatureEntry::FeatureParam kPauseBackgroundTabsLowEngagment[] = {
    {pausetabs::kFeatureName, pausetabs::kModeParamLow}};

const FeatureEntry::FeatureParam kPauseBackgroundTabsMediumEngagment[] = {
    {pausetabs::kFeatureName, pausetabs::kModeParamMedium}};

const FeatureEntry::FeatureParam kPauseBackgroundTabsHighEngagment[] = {
    {pausetabs::kFeatureName, pausetabs::kModeParamHigh}};

const FeatureEntry::FeatureParam kPauseBackgroundTabsMaxEngagment[] = {
    {pausetabs::kFeatureName, pausetabs::kModeParamMax}};

const FeatureEntry::FeatureVariation kPauseBackgroundTabsVariations[] = {
    {"minimal engagement", kPauseBackgroundTabsMinimalEngagment,
     arraysize(kPauseBackgroundTabsMinimalEngagment), nullptr},
    {"low engagement", kPauseBackgroundTabsLowEngagment,
     arraysize(kPauseBackgroundTabsLowEngagment), nullptr},
    {"medium engagement", kPauseBackgroundTabsMediumEngagment,
     arraysize(kPauseBackgroundTabsMediumEngagment), nullptr},
    {"high engagement", kPauseBackgroundTabsHighEngagment,
     arraysize(kPauseBackgroundTabsHighEngagment), nullptr},
    {"max engagement", kPauseBackgroundTabsMaxEngagment,
     arraysize(kPauseBackgroundTabsMaxEngagment), nullptr}};
#endif

#if defined(OS_ANDROID)
const FeatureEntry::FeatureParam
    kAutofillCreditCardPopupLayoutFeatureVariationIconAtStart[] = {
        {"is_credit_card_icon_at_start", "true"}};

const FeatureEntry::FeatureParam
    kAutofillCreditCardPopupLayoutFeatureVariationDropdownItemHeight[] = {
        {"dropdown_item_height", "56"}};

const FeatureEntry::FeatureParam
    kAutofillCreditCardPopupLayoutFeatureVariationExpanded[] = {
        {"is_credit_card_icon_at_start", "true"},
        {"dropdown_item_height", "56"},
        {"margin", "18"}};

const FeatureEntry::FeatureVariation
    kAutofillCreditCardPopupLayoutFeatureVariations[] = {
        {"Display credit card icon at start",
         kAutofillCreditCardPopupLayoutFeatureVariationIconAtStart,
         arraysize(kAutofillCreditCardPopupLayoutFeatureVariationIconAtStart),
         nullptr},
        {"Increase dropdown item height",
         kAutofillCreditCardPopupLayoutFeatureVariationDropdownItemHeight,
         arraysize(
             kAutofillCreditCardPopupLayoutFeatureVariationDropdownItemHeight),
         nullptr},
        {"Display credit card icon at start and increase dropdown item height",
         kAutofillCreditCardPopupLayoutFeatureVariationExpanded,
         arraysize(kAutofillCreditCardPopupLayoutFeatureVariationExpanded),
         nullptr}};

const FeatureEntry::FeatureParam
    kAutofillKeyboardAccessoryFeatureVariationAnimationDuration[] = {
        {autofill::kAutofillKeyboardAccessoryAnimationDurationKey, "1000"}};

const FeatureEntry::FeatureParam
    kAutofillKeyboardAccessoryFeatureVariationLimitLabelWidth[] = {
        {autofill::kAutofillKeyboardAccessoryLimitLabelWidthKey, "true"}};

const FeatureEntry::FeatureParam
    kAutofillKeyboardAccessoryFeatureVariationShowHint[] = {
        {autofill::kAutofillKeyboardAccessoryHintKey, "true"}};

const FeatureEntry::FeatureParam
    kAutofillKeyboardAccessoryFeatureVariationAnimateWithHint[] = {
        {autofill::kAutofillKeyboardAccessoryAnimationDurationKey, "1000"},
        {autofill::kAutofillKeyboardAccessoryHintKey, "true"}};

const FeatureEntry::FeatureVariation
    kAutofillKeyboardAccessoryFeatureVariations[] = {
        {"Animate", kAutofillKeyboardAccessoryFeatureVariationAnimationDuration,
         arraysize(kAutofillKeyboardAccessoryFeatureVariationAnimationDuration),
         nullptr},
        {"Limit label width",
         kAutofillKeyboardAccessoryFeatureVariationLimitLabelWidth,
         arraysize(kAutofillKeyboardAccessoryFeatureVariationLimitLabelWidth),
         nullptr},
        {"Show hint", kAutofillKeyboardAccessoryFeatureVariationShowHint,
         arraysize(kAutofillKeyboardAccessoryFeatureVariationShowHint),
         nullptr},
        {"Animate with hint",
         kAutofillKeyboardAccessoryFeatureVariationAnimateWithHint,
         arraysize(kAutofillKeyboardAccessoryFeatureVariationAnimateWithHint),
         nullptr}};
#endif  // OS_ANDROID

const FeatureEntry::FeatureParam
    kAutofillCreditCardLastUsedDateFeatureVariationExpDate[] = {
        {"show_expiration_date", "true"}};

const FeatureEntry::FeatureVariation
    kAutofillCreditCardLastUsedDateFeatureVariations[] = {
        {"Display expiration date",
         kAutofillCreditCardLastUsedDateFeatureVariationExpDate,
         arraysize(kAutofillCreditCardLastUsedDateFeatureVariationExpDate),
         nullptr}};

const FeatureEntry::FeatureParam kMemoryAblation5MiB_512[] = {
    {kMemoryAblationFeatureSizeParam, "5242880"},
    {kMemoryAblationFeatureMaxRAMParam, "512"}};
const FeatureEntry::FeatureParam kMemoryAblation10MiB_512_1024[] = {
    {kMemoryAblationFeatureSizeParam, "10485760"},
    {kMemoryAblationFeatureMinRAMParam, "512"},
    {kMemoryAblationFeatureMaxRAMParam, "1024"}};
const FeatureEntry::FeatureParam kMemoryAblation20MiB_1024_2048[] = {
    {kMemoryAblationFeatureSizeParam, "20971520"},
    {kMemoryAblationFeatureMinRAMParam, "1024"},
    {kMemoryAblationFeatureMaxRAMParam, "2048"}};
const FeatureEntry::FeatureParam kMemoryAblation30MiB[] = {
    {kMemoryAblationFeatureSizeParam, "31457280"}};
const FeatureEntry::FeatureParam kMemoryAblation40MiB_512_4096[] = {
    {kMemoryAblationFeatureSizeParam, "41943040"},
    {kMemoryAblationFeatureMinRAMParam, "512"},
    {kMemoryAblationFeatureMaxRAMParam, "4096"}};
const FeatureEntry::FeatureParam kMemoryAblation50MiB_2048_4096[] = {
    {kMemoryAblationFeatureSizeParam, "52428800"},
    {kMemoryAblationFeatureMinRAMParam, "2048"},
    {kMemoryAblationFeatureMaxRAMParam, "4096"}};
const FeatureEntry::FeatureParam kMemoryAblation100MiB_4096[] = {
    {kMemoryAblationFeatureSizeParam, "104857600"},
    {kMemoryAblationFeatureMinRAMParam, "4096"}};

const FeatureEntry::FeatureVariation kMemoryAblationFeatureVariations[] = {
    {"5 MiB (RAM <= 512)", kMemoryAblation5MiB_512,
     arraysize(kMemoryAblation5MiB_512), nullptr},
    {"10 MiB (512 <= RAM <= 1024)", kMemoryAblation10MiB_512_1024,
     arraysize(kMemoryAblation10MiB_512_1024), nullptr},
    {"20 MiB (1024 <= RAM <= 2048)", kMemoryAblation20MiB_1024_2048,
     arraysize(kMemoryAblation20MiB_1024_2048), nullptr},
    {"30 MiB (any RAM)", kMemoryAblation30MiB, arraysize(kMemoryAblation30MiB),
     nullptr},
    {"40 MiB (512 <= RAM <= 4096)", kMemoryAblation40MiB_512_4096,
     arraysize(kMemoryAblation40MiB_512_4096), nullptr},
    {"50 MiB (2048 <= RAM <= 4096)", kMemoryAblation50MiB_2048_4096,
     arraysize(kMemoryAblation50MiB_2048_4096), nullptr},
    {"100 MiB (RAM >= 4096)", kMemoryAblation100MiB_4096,
     arraysize(kMemoryAblation100MiB_4096), nullptr}};

#if defined(OS_ANDROID)
const FeatureEntry::FeatureParam kPersistentMenuItemEnabled[] = {
    {"persistent_menu_item_enabled", "true"}};

const FeatureEntry::FeatureVariation kDataReductionMainMenuFeatureVariations[] =
    {{"(persistent)", kPersistentMenuItemEnabled,
      arraysize(kPersistentMenuItemEnabled), nullptr}};
#endif  // OS_ANDROID

const FeatureEntry::Choice kEnableHeapProfilingChoices[] = {
    {flags_ui::kGenericExperimentChoiceDisabled, "", ""},
    {flag_descriptions::kEnableHeapProfilingModePseudo,
     switches::kEnableHeapProfiling, switches::kEnableHeapProfilingModePseudo},
    {flag_descriptions::kEnableHeapProfilingModeNative,
     switches::kEnableHeapProfiling, switches::kEnableHeapProfilingModeNative},
    {flag_descriptions::kEnableHeapProfilingTaskProfiler,
     switches::kEnableHeapProfiling,
     switches::kEnableHeapProfilingTaskProfiler}};

const FeatureEntry::Choice kEnableOutOfProcessHeapProfilingChoices[] = {
    {flags_ui::kGenericExperimentChoiceDisabled, "", ""},
    {flag_descriptions::kEnableOutOfProcessHeapProfilingModeMinimal,
     switches::kMemlog, switches::kMemlogModeMinimal},
    {flag_descriptions::kEnableOutOfProcessHeapProfilingModeAll,
     switches::kMemlog, switches::kMemlogModeAll},
    {flag_descriptions::kEnableOutOfProcessHeapProfilingModeBrowser,
     switches::kMemlog, switches::kMemlogModeBrowser},
    {flag_descriptions::kEnableOutOfProcessHeapProfilingModeGpu,
     switches::kMemlog, switches::kMemlogModeGpu},
    {flag_descriptions::kEnableOutOfProcessHeapProfilingModeRendererSampling,
     switches::kMemlog, switches::kMemlogModeRendererSampling}};

const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches4[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "4"}};
const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches6[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "6"}};
const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches8[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "8"}};
const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches10[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "10"}};
const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches12[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "12"}};

const FeatureEntry::FeatureVariation
    kOmniboxUIMaxAutocompleteMatchesVariations[] = {
        {"4 matches", kOmniboxUIMaxAutocompleteMatches4,
         arraysize(kOmniboxUIMaxAutocompleteMatches4), nullptr},
        {"6 matches", kOmniboxUIMaxAutocompleteMatches6,
         arraysize(kOmniboxUIMaxAutocompleteMatches6), nullptr},
        {"8 matches", kOmniboxUIMaxAutocompleteMatches8,
         arraysize(kOmniboxUIMaxAutocompleteMatches8), nullptr},
        {"10 matches", kOmniboxUIMaxAutocompleteMatches10,
         arraysize(kOmniboxUIMaxAutocompleteMatches10), nullptr},
        {"12 matches", kOmniboxUIMaxAutocompleteMatches12,
         arraysize(kOmniboxUIMaxAutocompleteMatches12), nullptr}};

const FeatureEntry::FeatureParam kOmniboxUIVerticalMargin4px[] = {
    {OmniboxFieldTrial::kUIVerticalMarginParam, "4"}};
const FeatureEntry::FeatureParam kOmniboxUIVerticalMargin6px[] = {
    {OmniboxFieldTrial::kUIVerticalMarginParam, "6"}};
const FeatureEntry::FeatureParam kOmniboxUIVerticalMargin8px[] = {
    {OmniboxFieldTrial::kUIVerticalMarginParam, "8"}};
const FeatureEntry::FeatureParam kOmniboxUIVerticalMargin10px[] = {
    {OmniboxFieldTrial::kUIVerticalMarginParam, "10"}};
const FeatureEntry::FeatureParam kOmniboxUIVerticalMargin12px[] = {
    {OmniboxFieldTrial::kUIVerticalMarginParam, "12"}};
const FeatureEntry::FeatureParam kOmniboxUIVerticalMargin14px[] = {
    {OmniboxFieldTrial::kUIVerticalMarginParam, "14"}};

const FeatureEntry::FeatureVariation kOmniboxUIVerticalMarginVariations[] = {
    {"4px vertical margin", kOmniboxUIVerticalMargin4px,
     arraysize(kOmniboxUIVerticalMargin4px), nullptr},
    {"6px vertical margin", kOmniboxUIVerticalMargin6px,
     arraysize(kOmniboxUIVerticalMargin6px), nullptr},
    {"8px vertical margin", kOmniboxUIVerticalMargin8px,
     arraysize(kOmniboxUIVerticalMargin8px), nullptr},
    {"10px vertical margin", kOmniboxUIVerticalMargin10px,
     arraysize(kOmniboxUIVerticalMargin10px), nullptr},
    {"12px vertical margin", kOmniboxUIVerticalMargin12px,
     arraysize(kOmniboxUIVerticalMargin12px), nullptr},
    {"14px vertical margin", kOmniboxUIVerticalMargin14px,
     arraysize(kOmniboxUIVerticalMargin14px), nullptr}};

const FeatureEntry::FeatureParam kClientPlaceholdersForServerLoFiEnabled[] = {
    {"replace_server_placeholders", "true"}};

const FeatureEntry::FeatureVariation
    kClientPlaceholdersForServerLoFiFeatureVariations[] = {
        {"(replace Server Lo-Fi placeholders)",
         kClientPlaceholdersForServerLoFiEnabled,
         arraysize(kClientPlaceholdersForServerLoFiEnabled), nullptr}};

const FeatureEntry::Choice kAsyncImageDecodingChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flags_ui::kGenericExperimentChoiceEnabled,
     cc::switches::kEnableCheckerImaging, ""},
    {flags_ui::kGenericExperimentChoiceDisabled,
     cc::switches::kDisableCheckerImaging, ""},
};

const FeatureEntry::Choice kUseDdljsonApiChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
#if defined(OS_ANDROID)
    {"(force test doodle 0)", search_provider_logos::switches::kGoogleDoodleUrl,
     "https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_android0.json"},
    {"(force test doodle 1)", search_provider_logos::switches::kGoogleDoodleUrl,
     "https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_android1.json"},
    {"(force test doodle 2)", search_provider_logos::switches::kGoogleDoodleUrl,
     "https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_android2.json"},
    {"(force test doodle 3)", search_provider_logos::switches::kGoogleDoodleUrl,
     "https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_android3.json"},
    {"(force test doodle 4)", search_provider_logos::switches::kGoogleDoodleUrl,
     "https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_android4.json"},
#else
    {"(force test doodle 0)", search_provider_logos::switches::kGoogleDoodleUrl,
     "https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_desktop0.json"},
    {"(force test doodle 1)", search_provider_logos::switches::kGoogleDoodleUrl,
     "https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_desktop1.json"},
    {"(force test doodle 2)", search_provider_logos::switches::kGoogleDoodleUrl,
     "https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_desktop2.json"},
    {"(force test doodle 3)", search_provider_logos::switches::kGoogleDoodleUrl,
     "https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_desktop3.json"},
    {"(force test doodle 4)", search_provider_logos::switches::kGoogleDoodleUrl,
     "https://www.gstatic.com/chrome/ntp/doodle_test/ddljson_desktop4.json"},
#endif  // defined(OS_ANDROID)
};

#if defined(OS_ANDROID)
const FeatureEntry::FeatureParam kThirdPartyDoodlesTestSimple[] = {
    {search_provider_logos::features::kThirdPartyDoodlesOverrideUrlParam,
     "https://www.gstatic.com/chrome/ntp/doodle_test/third_party_simple.json"}};
const FeatureEntry::FeatureParam kThirdPartyDoodlesTestAnimated[] = {
    {search_provider_logos::features::kThirdPartyDoodlesOverrideUrlParam,
     "https://www.gstatic.com/chrome/ntp/doodle_test/"
     "third_party_animated.json"}};

const FeatureEntry::FeatureVariation kThirdPartyDoodlesVariations[] = {
    {"(force simple test doodle)", kThirdPartyDoodlesTestSimple,
     arraysize(kThirdPartyDoodlesTestSimple), nullptr},
    {"(force animated test doodle)", kThirdPartyDoodlesTestAnimated,
     arraysize(kThirdPartyDoodlesTestAnimated), nullptr}};
#endif  // defined(OS_ANDROID)

const FeatureEntry::FeatureParam kSpeculativePreconnectPreconnect[] = {
    {predictors::kModeParamName, predictors::kPreconnectMode}};
const FeatureEntry::FeatureParam kSpeculativePreconnectLearning[] = {
    {predictors::kModeParamName, predictors::kLearningMode}};
const FeatureEntry::FeatureParam kSpeculativePreconnectNoPreconnect[] = {
    {predictors::kModeParamName, predictors::kNoPreconnectMode}};

const FeatureEntry::FeatureVariation kSpeculativePreconnectFeatureVariations[] =
    {{"Preconnect", kSpeculativePreconnectPreconnect,
      arraysize(kSpeculativePreconnectPreconnect), nullptr},
     {"Learning", kSpeculativePreconnectLearning,
      arraysize(kSpeculativePreconnectLearning), nullptr},
     {"No preconnect", kSpeculativePreconnectNoPreconnect,
      arraysize(kSpeculativePreconnectNoPreconnect), nullptr}};

// RECORDING USER METRICS FOR FLAGS:
// -----------------------------------------------------------------------------
// The first line of the entry is the internal name.
//
// To add a new entry, add to the end of kFeatureEntries. There are two
// distinct types of entries:
// . SINGLE_VALUE: entry is either on or off. Use the SINGLE_VALUE_TYPE
//   macro for this type supplying the command line to the macro.
// . MULTI_VALUE: a list of choices, the first of which should correspond to a
//   deactivated state for this lab (i.e. no command line option).  To specify
//   this type of entry use the macro MULTI_VALUE_TYPE supplying it the
//   array of choices.
// See the documentation of FeatureEntry for details on the fields.
//
// Usage of about:flags is logged on startup via the "Launch.FlagsAtStartup"
// UMA histogram. This histogram shows the number of startups with a given flag
// enabled. If you'd like to see user counts instead, make sure to switch to
// to "count users" view on the dashboard. When adding new entries, the enum
// "LoginCustomFlags" must be updated in histograms/enums.xml. See note in
// enums.xml and don't forget to run AboutFlagsHistogramTest unit test
// to calculate and verify checksum.
//
// When adding a new choice, add it to the end of the list.
const FeatureEntry kFeatureEntries[] = {
    {"ignore-gpu-blacklist", flag_descriptions::kIgnoreGpuBlacklistName,
     flag_descriptions::kIgnoreGpuBlacklistDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kIgnoreGpuBlacklist)},
    {"enable-experimental-canvas-features",
     flag_descriptions::kExperimentalCanvasFeaturesName,
     flag_descriptions::kExperimentalCanvasFeaturesDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableExperimentalCanvasFeatures)},
    {"disable-accelerated-2d-canvas",
     flag_descriptions::kAccelerated2dCanvasName,
     flag_descriptions::kAccelerated2dCanvasDescription, kOsAll,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableAccelerated2dCanvas)},
    {"composited-layer-borders", flag_descriptions::kCompositedLayerBordersName,
     flag_descriptions::kCompositedLayerBordersDescription, kOsAll,
     SINGLE_VALUE_TYPE(cc::switches::kShowCompositedLayerBorders)},
    {"gl-composited-overlay-candidate-quad-borders",
     flag_descriptions::kGlCompositedOverlayCandidateQuadBordersName,
     flag_descriptions::kGlCompositedOverlayCandidateQuadBordersDescription,
     kOsAll,
     SINGLE_VALUE_TYPE(switches::kGlCompositedOverlayCandidateQuadBorder)},
    {"show-overdraw-feedback", flag_descriptions::kShowOverdrawFeedbackName,
     flag_descriptions::kShowOverdrawFeedbackDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kShowOverdrawFeedback)},
    {"enable-draw-occlusion", flag_descriptions::kEnableDrawOcclusionName,
     flag_descriptions::kEnableDrawOcclusionDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kShowOverdrawFeedback)},
    {"ui-disable-partial-swap", flag_descriptions::kUiPartialSwapName,
     flag_descriptions::kUiPartialSwapDescription, kOsAll,
     SINGLE_DISABLE_VALUE_TYPE(switches::kUIDisablePartialSwap)},
#if BUILDFLAG(ENABLE_WEBRTC)
    {"disable-webrtc-hw-decoding", flag_descriptions::kWebrtcHwDecodingName,
     flag_descriptions::kWebrtcHwDecodingDescription, kOsAndroid | kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableWebRtcHWDecoding)},
    {"disable-webrtc-hw-encoding", flag_descriptions::kWebrtcHwEncodingName,
     flag_descriptions::kWebrtcHwEncodingDescription, kOsAndroid | kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableWebRtcHWEncoding)},
    {"enable-webrtc-hw-h264-encoding",
     flag_descriptions::kWebrtcHwH264EncodingName,
     flag_descriptions::kWebrtcHwH264EncodingDescription, kOsAndroid | kOsCrOS,
     FEATURE_VALUE_TYPE(features::kWebRtcHWH264Encoding)},
    {"enable-webrtc-hw-vp8-encoding",
     flag_descriptions::kWebrtcHwVP8EncodingName,
     flag_descriptions::kWebrtcHwVP8EncodingDescription, kOsAndroid | kOsCrOS,
     FEATURE_VALUE_TYPE(features::kWebRtcHWVP8Encoding)},
    {"enable-webrtc-srtp-aes-gcm", flag_descriptions::kWebrtcSrtpAesGcmName,
     flag_descriptions::kWebrtcSrtpAesGcmDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableWebRtcSrtpAesGcm)},
    {"enable-webrtc-srtp-encrypted-headers",
     flag_descriptions::kWebrtcSrtpEncryptedHeadersName,
     flag_descriptions::kWebrtcSrtpEncryptedHeadersDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableWebRtcSrtpEncryptedHeaders)},
    {"enable-webrtc-stun-origin", flag_descriptions::kWebrtcStunOriginName,
     flag_descriptions::kWebrtcStunOriginDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableWebRtcStunOrigin)},
    {"WebRtcUseEchoCanceller3", flag_descriptions::kWebrtcEchoCanceller3Name,
     flag_descriptions::kWebrtcEchoCanceller3Description, kOsAll,
     FEATURE_VALUE_TYPE(features::kWebRtcUseEchoCanceller3)},
#endif  // ENABLE_WEBRTC
#if defined(OS_ANDROID)
    {"clear-old-browsing-data", flag_descriptions::kClearOldBrowsingDataName,
     flag_descriptions::kClearOldBrowsingDataDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kClearOldBrowsingData)},
    {"enable-osk-overscroll", flag_descriptions::kEnableOskOverscrollName,
     flag_descriptions::kEnableOskOverscrollDescription, kOsAndroid,
     SINGLE_VALUE_TYPE(switches::kEnableOSKOverscroll)},
    {"enable-new-photo-picker", flag_descriptions::kNewPhotoPickerName,
     flag_descriptions::kNewPhotoPickerDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kNewPhotoPicker)},
    {"enable-usermedia-screen-capturing",
     flag_descriptions::kMediaScreenCaptureName,
     flag_descriptions::kMediaScreenCaptureDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kUserMediaScreenCapturing)},
#endif  // OS_ANDROID
// Native client is compiled out if ENABLE_NACL is not set.
#if BUILDFLAG(ENABLE_NACL)
    {"enable-nacl", flag_descriptions::kNaclName,
     flag_descriptions::kNaclDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableNaCl)},
    {"enable-nacl-debug", flag_descriptions::kNaclDebugName,
     flag_descriptions::kNaclDebugDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(switches::kEnableNaClDebug)},
    {"force-pnacl-subzero", flag_descriptions::kPnaclSubzeroName,
     flag_descriptions::kPnaclSubzeroDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(switches::kForcePNaClSubzero)},
    {"nacl-debug-mask", flag_descriptions::kNaclDebugMaskName,
     flag_descriptions::kNaclDebugMaskDescription, kOsDesktop,
     MULTI_VALUE_TYPE(kNaClDebugMaskChoices)},
#endif  // ENABLE_NACL
#if BUILDFLAG(ENABLE_EXTENSIONS)
    {"extension-apis", flag_descriptions::kExperimentalExtensionApisName,
     flag_descriptions::kExperimentalExtensionApisDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(extensions::switches::kEnableExperimentalExtensionApis)},
    {"extensions-on-chrome-urls",
     flag_descriptions::kExtensionsOnChromeUrlsName,
     flag_descriptions::kExtensionsOnChromeUrlsDescription, kOsAll,
     SINGLE_VALUE_TYPE(extensions::switches::kExtensionsOnChromeURLs)},
#endif  // ENABLE_EXTENSIONS
    {"enable-fast-unload", flag_descriptions::kFastUnloadName,
     flag_descriptions::kFastUnloadDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableFastUnload)},
    {"enable-history-entry-requires-user-gesture",
     flag_descriptions::kHistoryRequiresUserGestureName,
     flag_descriptions::kHistoryRequiresUserGestureDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kHistoryEntryRequiresUserGesture)},
    {"disable-hyperlink-auditing", flag_descriptions::kHyperlinkAuditingName,
     flag_descriptions::kHyperlinkAuditingDescription, kOsAll,
     SINGLE_DISABLE_VALUE_TYPE(switches::kNoPings)},
#if defined(OS_ANDROID)
    {"contextual-search", flag_descriptions::kContextualSearchName,
     flag_descriptions::kContextualSearchDescription, kOsAndroid,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableContextualSearch,
                               switches::kDisableContextualSearch)},
    {"contextual-search-ml-tap-suppression",
     flag_descriptions::kContextualSearchMlTapSuppressionName,
     flag_descriptions::kContextualSearchMlTapSuppressionDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kContextualSearchMlTapSuppression)},
#endif  // OS_ANDROID
    {"show-autofill-type-predictions",
     flag_descriptions::kShowAutofillTypePredictionsName,
     flag_descriptions::kShowAutofillTypePredictionsDescription, kOsAll,
     FEATURE_VALUE_TYPE(autofill::features::kAutofillShowTypePredictions)},
    {"smooth-scrolling", flag_descriptions::kSmoothScrollingName,
     flag_descriptions::kSmoothScrollingDescription,
     // Mac has a separate implementation with its own setting to disable.
     kOsLinux | kOsCrOS | kOsWin | kOsAndroid,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableSmoothScrolling,
                               switches::kDisableSmoothScrolling)},
#if defined(USE_AURA)
    {"overlay-scrollbars", flag_descriptions::kOverlayScrollbarsName,
     flag_descriptions::kOverlayScrollbarsDescription,
     // Uses the system preference on Mac (a different implementation).
     // On Android, this is always enabled.
     kOsAura, FEATURE_VALUE_TYPE(features::kOverlayScrollbar)},
    {"overlay-scrollbars-flash-after-scroll-update",
     flag_descriptions::kOverlayScrollbarsFlashAfterAnyScrollUpdateName,
     flag_descriptions::kOverlayScrollbarsFlashAfterAnyScrollUpdateDescription,
     kOsAura,
     FEATURE_VALUE_TYPE(features::kOverlayScrollbarFlashAfterAnyScrollUpdate)},
    {"overlay-scrollbars-flash-when-mouse-enter",
     flag_descriptions::kOverlayScrollbarsFlashWhenMouseEnterName,
     flag_descriptions::kOverlayScrollbarsFlashWhenMouseEnterDescription,
     kOsAura,
     FEATURE_VALUE_TYPE(features::kOverlayScrollbarFlashWhenMouseEnter)},
#endif  // USE_AURA
    {   // See http://crbug.com/120416 for how to remove this flag.
     "save-page-as-mhtml", flag_descriptions::kSavePageAsMhtmlName,
     flag_descriptions::kSavePageAsMhtmlDescription, kOsMac | kOsWin | kOsLinux,
     SINGLE_VALUE_TYPE(switches::kSavePageAsMHTML)},
    {"mhtml-generator-option", flag_descriptions::kMhtmlGeneratorOptionName,
     flag_descriptions::kMhtmlGeneratorOptionDescription,
     kOsMac | kOsWin | kOsLinux,
     MULTI_VALUE_TYPE(kMHTMLGeneratorOptionChoices)},
    {"enable-quic", flag_descriptions::kQuicName,
     flag_descriptions::kQuicDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableQuic, switches::kDisableQuic)},
    {"disable-javascript-harmony-shipping",
     flag_descriptions::kJavascriptHarmonyShippingName,
     flag_descriptions::kJavascriptHarmonyShippingDescription, kOsAll,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableJavaScriptHarmonyShipping)},
    {"enable-javascript-harmony", flag_descriptions::kJavascriptHarmonyName,
     flag_descriptions::kJavascriptHarmonyDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kJavaScriptHarmony)},
    {"enable-asm-webassembly", flag_descriptions::kEnableAsmWasmName,
     flag_descriptions::kEnableAsmWasmDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kAsmJsToWebAssembly)},
    {"enable-webassembly", flag_descriptions::kEnableWasmName,
     flag_descriptions::kEnableWasmDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kWebAssembly)},
    {"enable-webassembly-streaming",
     flag_descriptions::kEnableWasmStreamingName,
     flag_descriptions::kEnableWasmStreamingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kWebAssemblyStreaming)},
    {"enable-future-v8-vm-features", flag_descriptions::kV8VmFutureName,
     flag_descriptions::kV8VmFutureDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kV8VmFuture)},
    {"disable-software-rasterizer", flag_descriptions::kSoftwareRasterizerName,
     flag_descriptions::kSoftwareRasterizerDescription,
#if BUILDFLAG(ENABLE_SWIFTSHADER)
     kOsAll,
#else   // BUILDFLAG(ENABLE_SWIFTSHADER)
     0,
#endif  // BUILDFLAG(ENABLE_SWIFTSHADER)
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableSoftwareRasterizer)},
    {"enable-gpu-rasterization", flag_descriptions::kGpuRasterizationName,
     flag_descriptions::kGpuRasterizationDescription, kOsAll,
     MULTI_VALUE_TYPE(kEnableGpuRasterizationChoices)},
    {"gpu-rasterization-msaa-sample-count",
     flag_descriptions::kGpuRasterizationMsaaSampleCountName,
     flag_descriptions::kGpuRasterizationMsaaSampleCountDescription, kOsAll,
     MULTI_VALUE_TYPE(kGpuRasterizationMSAASampleCountChoices)},
    {"enable-experimental-web-platform-features",
     flag_descriptions::kExperimentalWebPlatformFeaturesName,
     flag_descriptions::kExperimentalWebPlatformFeaturesDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableExperimentalWebPlatformFeatures)},
#if BUILDFLAG(ENABLE_EXTENSIONS)
    {"enable-ble-advertising-in-apps",
     flag_descriptions::kBleAdvertisingInExtensionsName,
     flag_descriptions::kBleAdvertisingInExtensionsDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(extensions::switches::kEnableBLEAdvertising)},
#endif  // ENABLE_EXTENSIONS
    {"enable-devtools-experiments", flag_descriptions::kDevtoolsExperimentsName,
     flag_descriptions::kDevtoolsExperimentsDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(switches::kEnableDevToolsExperiments)},
    {"silent-debugger-extension-api",
     flag_descriptions::kSilentDebuggerExtensionApiName,
     flag_descriptions::kSilentDebuggerExtensionApiDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(switches::kSilentDebuggerExtensionAPI)},
#if BUILDFLAG(ENABLE_SPELLCHECK) && defined(OS_ANDROID)
    {"enable-android-spellchecker",
     flag_descriptions::kEnableAndroidSpellcheckerDescription,
     flag_descriptions::kEnableAndroidSpellcheckerDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(spellcheck::kAndroidSpellCheckerNonLowEnd)},
#endif  // ENABLE_SPELLCHECK && OS_ANDROID
    {"enable-scroll-prediction", flag_descriptions::kScrollPredictionName,
     flag_descriptions::kScrollPredictionDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(switches::kEnableScrollPrediction)},
    {"top-chrome-md", flag_descriptions::kTopChromeMd,
     flag_descriptions::kTopChromeMdDescription, kOsDesktop,
     MULTI_VALUE_TYPE(kTopChromeMaterialDesignChoices)},
    {"enable-site-settings", flag_descriptions::kSiteSettings,
     flag_descriptions::kSiteSettingsDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(switches::kEnableSiteSettings)},
    {"secondary-ui-md", flag_descriptions::kSecondaryUiMd,
     flag_descriptions::kSecondaryUiMdDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kSecondaryUiMd)},
    {"touch-events", flag_descriptions::kTouchEventsName,
     flag_descriptions::kTouchEventsDescription, kOsDesktop,
     MULTI_VALUE_TYPE(kTouchEventFeatureDetectionChoices)},
    {"disable-touch-adjustment", flag_descriptions::kTouchAdjustmentName,
     flag_descriptions::kTouchAdjustmentDescription,
     kOsWin | kOsLinux | kOsCrOS | kOsAndroid,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableTouchAdjustment)},
#if defined(OS_CHROMEOS)
    {"network-portal-notification",
     flag_descriptions::kNetworkPortalNotificationName,
     flag_descriptions::kNetworkPortalNotificationDescription, kOsCrOS,
     ENABLE_DISABLE_VALUE_TYPE(
         chromeos::switches::kEnableNetworkPortalNotification,
         chromeos::switches::kDisableNetworkPortalNotification)},
    {"network-settings-config", flag_descriptions::kNetworkSettingsConfigName,
     flag_descriptions::kNetworkSettingsConfigDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(chromeos::switches::kNetworkSettingsConfig)},
#endif  // OS_CHROMEOS
#if BUILDFLAG(ENABLE_PLUGINS)
    {"allow-nacl-socket-api", flag_descriptions::kAllowNaclSocketApiName,
     flag_descriptions::kAllowNaclSocketApiDescription, kOsDesktop,
     SINGLE_VALUE_TYPE_AND_VALUE(switches::kAllowNaClSocketAPI, "*")},
#endif  // ENABLE_PLUGINS
#if defined(OS_CHROMEOS)
    {"ash-enable-night-light", flag_descriptions::kEnableNightLightName,
     flag_descriptions::kEnableNightLightDescription, kOsAll,
     SINGLE_VALUE_TYPE(ash::switches::kAshEnableNightLight)},
    {"allow-touchpad-three-finger-click",
     flag_descriptions::kAllowTouchpadThreeFingerClickName,
     flag_descriptions::kAllowTouchpadThreeFingerClickDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(chromeos::switches::kEnableTouchpadThreeFingerClick)},
    {"ash-enable-unified-desktop",
     flag_descriptions::kAshEnableUnifiedDesktopName,
     flag_descriptions::kAshEnableUnifiedDesktopDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(switches::kEnableUnifiedDesktop)},
    {
        "disable-boot-animation", flag_descriptions::kBootAnimationName,
        flag_descriptions::kBootAnimationDescription, kOsCrOSOwnerOnly,
        SINGLE_DISABLE_VALUE_TYPE(chromeos::switches::kDisableBootAnimation),
    },
    {"disable-easy-unlock-bluetooth-low-energy-detection",
     flag_descriptions::kEasyUnlockBluetoothLowEnergyDiscoveryName,
     flag_descriptions::kEasyUnlockBluetoothLowEnergyDiscoveryDescription,
     kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(
         proximity_auth::switches::kDisableBluetoothLowEnergyDiscovery)},
    {
        "disable-office-editing-component-app",
        flag_descriptions::kOfficeEditingComponentAppName,
        flag_descriptions::kOfficeEditingComponentAppDescription, kOsCrOS,
        SINGLE_DISABLE_VALUE_TYPE(
            chromeos::switches::kDisableOfficeEditingComponentApp),
    },
    {
        "enable-background-blur", flag_descriptions::kEnableBackgroundBlurName,
        flag_descriptions::kEnableBackgroundBlurDescription, kOsCrOS,
        FEATURE_VALUE_TYPE(app_list::features::kEnableBackgroundBlur),
    },
    {"enable-easyunlock-promotions",
     flag_descriptions::kEasyUnlockPromotionsName,
     flag_descriptions::kEasyUnlockPromotionsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kEasyUnlockPromotions)},
    {
        "enable-pinch", flag_descriptions::kPinchScaleName,
        flag_descriptions::kPinchScaleDescription, kOsLinux | kOsWin | kOsCrOS,
        ENABLE_DISABLE_VALUE_TYPE(switches::kEnablePinch,
                                  switches::kDisablePinch),
    },
    {"enable-video-player-chromecast-support",
     flag_descriptions::kVideoPlayerChromecastSupportName,
     flag_descriptions::kVideoPlayerChromecastSupportDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(
         chromeos::switches::kEnableVideoPlayerChromecastSupport)},
    {"instant-tethering", flag_descriptions::kTetherName,
     flag_descriptions::kTetherDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kInstantTethering)},
    {"multidevice", flag_descriptions::kMultideviceName,
     flag_descriptions::kMultideviceDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kMultidevice)},
    {"mus", flag_descriptions::kUseMusName,
     flag_descriptions::kUseMusDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kUseMusChoices)},
    {"show-taps", flag_descriptions::kShowTapsName,
     flag_descriptions::kShowTapsDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ash::switches::kShowTaps)},
    {"show-touch-hud", flag_descriptions::kShowTouchHudName,
     flag_descriptions::kShowTouchHudDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ash::switches::kAshTouchHud)},
#endif  // OS_CHROMEOS
    {
        "disable-accelerated-video-decode",
        flag_descriptions::kAcceleratedVideoDecodeName,
        flag_descriptions::kAcceleratedVideoDecodeDescription,
        kOsMac | kOsWin | kOsCrOS | kOsAndroid,
        SINGLE_DISABLE_VALUE_TYPE(switches::kDisableAcceleratedVideoDecode),
    },
    {"mojo-video-encode-accelerator",
     flag_descriptions::kMojoVideoEncodeAcceleratorName,
     flag_descriptions::kMojoVideoEncodeAcceleratorDescription,
     kOsMac | kOsWin | kOsCrOS | kOsAndroid,
     FEATURE_VALUE_TYPE(features::kMojoVideoEncodeAccelerator)},
#if defined(OS_WIN)
    {"enable-hdr", flag_descriptions::kEnableHDRName,
     flag_descriptions::kEnableHDRDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kHighDynamicRange)},
#endif  // OS_WIN
#if defined(OS_CHROMEOS)
    {
        "ash-debug-shortcuts", flag_descriptions::kDebugShortcutsName,
        flag_descriptions::kDebugShortcutsDescription, kOsAll,
        SINGLE_VALUE_TYPE(ash::switches::kAshDebugShortcuts),
    },
    {
        "ash-enable-mirrored-screen",
        flag_descriptions::kAshEnableMirroredScreenName,
        flag_descriptions::kAshEnableMirroredScreenDescription, kOsCrOS,
        SINGLE_VALUE_TYPE(ash::switches::kAshEnableMirroredScreen),
    },
    {"ash-shelf-color", flag_descriptions::kAshShelfColorName,
     flag_descriptions::kAshShelfColorDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kAshShelfColorChoices)},
    {"ash-shelf-color-scheme", flag_descriptions::kAshShelfColorScheme,
     flag_descriptions::kAshShelfColorSchemeDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kAshShelfColorSchemeChoices)},
    {"ash-shelf-model-synchronization",
     flag_descriptions::kAshDisableShelfModelSynchronization,
     flag_descriptions::kAshDisableShelfModelSynchronizationDescription,
     kOsCrOS,
     SINGLE_VALUE_TYPE(ash::switches::kAshDisableShelfModelSynchronization)},
    {"material-design-ink-drop-animation-speed",
     flag_descriptions::kMaterialDesignInkDropAnimationSpeedName,
     flag_descriptions::kMaterialDesignInkDropAnimationSpeedDescription,
     kOsCrOS, MULTI_VALUE_TYPE(kAshMaterialDesignInkDropAnimationSpeed)},
    {"ui-slow-animations", flag_descriptions::kUiSlowAnimationsName,
     flag_descriptions::kUiSlowAnimationsDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(switches::kUISlowAnimations)},
    {"ui-show-composited-layer-borders",
     flag_descriptions::kUiShowCompositedLayerBordersName,
     flag_descriptions::kUiShowCompositedLayerBordersDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kUiShowCompositedLayerBordersChoices)},
    {"disable-cloud-import", flag_descriptions::kCloudImportName,
     flag_descriptions::kCloudImportDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(chromeos::switches::kDisableCloudImport)},
    {"enable-request-tablet-site", flag_descriptions::kRequestTabletSiteName,
     flag_descriptions::kRequestTabletSiteDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(chromeos::switches::kEnableRequestTabletSite)},
#endif  // OS_CHROMEOS
    {"debug-packed-apps", flag_descriptions::kDebugPackedAppName,
     flag_descriptions::kDebugPackedAppDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(switches::kDebugPackedApps)},
    {"enable-password-generation", flag_descriptions::kPasswordGenerationName,
     flag_descriptions::kPasswordGenerationDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(autofill::switches::kEnablePasswordGeneration,
                               autofill::switches::kDisablePasswordGeneration)},
    {"enable-username-correction",
     flag_descriptions::kEnableUsernameCorrectionName,
     flag_descriptions::kEnableUsernameCorrectionDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(password_manager::features::kEnableUsernameCorrection)},
    {"enable-password-force-saving",
     flag_descriptions::kPasswordForceSavingName,
     flag_descriptions::kPasswordForceSavingDescription, kOsAll,
     FEATURE_VALUE_TYPE(
         password_manager::features::kEnablePasswordForceSaving)},
    {"enable-manual-password-generation",
     flag_descriptions::kManualPasswordGenerationName,
     flag_descriptions::kManualPasswordGenerationDescription, kOsAll,
     FEATURE_VALUE_TYPE(
         password_manager::features::kEnableManualPasswordGeneration)},
    {"enable-show-autofill-signatures",
     flag_descriptions::kShowAutofillSignaturesName,
     flag_descriptions::kShowAutofillSignaturesDescription, kOsAll,
     SINGLE_VALUE_TYPE(autofill::switches::kShowAutofillSignatures)},
    {"affiliation-based-matching",
     flag_descriptions::kAffiliationBasedMatchingName,
     flag_descriptions::kAffiliationBasedMatchingDescription, kOsAll,
     FEATURE_VALUE_TYPE(password_manager::features::kAffiliationBasedMatching)},
    {"wallet-service-use-sandbox",
     flag_descriptions::kWalletServiceUseSandboxName,
     flag_descriptions::kWalletServiceUseSandboxDescription,
     kOsAndroid | kOsDesktop,
     ENABLE_DISABLE_VALUE_TYPE_AND_VALUE(
         autofill::switches::kWalletServiceUseSandbox,
         "1",
         autofill::switches::kWalletServiceUseSandbox,
         "0")},
#if defined(USE_AURA)
    {"overscroll-history-navigation",
     flag_descriptions::kOverscrollHistoryNavigationName,
     flag_descriptions::kOverscrollHistoryNavigationDescription, kOsAura,
     MULTI_VALUE_TYPE(kOverscrollHistoryNavigationChoices)},
    {"overscroll-start-threshold",
     flag_descriptions::kOverscrollStartThresholdName,
     flag_descriptions::kOverscrollStartThresholdDescription, kOsAura,
     MULTI_VALUE_TYPE(kOverscrollStartThresholdChoices)},
    {"pull-to-refresh", flag_descriptions::kPullToRefreshName,
     flag_descriptions::kPullToRefreshDescription, kOsAura,
     ENABLE_DISABLE_VALUE_TYPE_AND_VALUE(switches::kPullToRefresh,
                                         "1",
                                         switches::kPullToRefresh,
                                         "0")},
#endif  // USE_AURA
    {"enable-touch-drag-drop", flag_descriptions::kTouchDragDropName,
     flag_descriptions::kTouchDragDropDescription, kOsWin | kOsCrOS,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableTouchDragDrop,
                               switches::kDisableTouchDragDrop)},
    {"touch-selection-strategy", flag_descriptions::kTouchSelectionStrategyName,
     flag_descriptions::kTouchSelectionStrategyDescription,
     kOsAndroid,  // TODO(mfomitchev): Add CrOS/Win/Linux support soon.
     MULTI_VALUE_TYPE(kTouchTextSelectionStrategyChoices)},
    {"enable-navigation-tracing",
     flag_descriptions::kEnableNavigationTracingName,
     flag_descriptions::kEnableNavigationTracingDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableNavigationTracing)},
    {"trace-upload-url", flag_descriptions::kTraceUploadUrlName,
     flag_descriptions::kTraceUploadUrlDescription, kOsAll,
     MULTI_VALUE_TYPE(kTraceUploadURL)},
    {"enable-service-worker-script-streaming",
     flag_descriptions::kServiceWorkerScriptStreamingName,
     flag_descriptions::kServiceWorkerScriptStreamingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kServiceWorkerScriptStreaming)},
    {"enable-service-worker-script-full-code-cache",
     flag_descriptions::kServiceWorkerScriptFullCodeCacheName,
     flag_descriptions::kServiceWorkerScriptFullCodeCacheDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kServiceWorkerScriptFullCodeCache)},
    {"enable-pwa-full-code-cache",
     flag_descriptions::kEnablePWAFullCodeCacheName,
     flag_descriptions::kEnablePWAFullCodeCacheDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kPWAFullCodeCache)},
    {"enable-suggestions-with-substring-match",
     flag_descriptions::kSuggestionsWithSubStringMatchName,
     flag_descriptions::kSuggestionsWithSubStringMatchDescription, kOsAll,
     SINGLE_VALUE_TYPE(
         autofill::switches::kEnableSuggestionsWithSubstringMatch)},
    {"lcd-text-aa", flag_descriptions::kLcdTextName,
     flag_descriptions::kLcdTextDescription, kOsDesktop,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableLCDText,
                               switches::kDisableLCDText)},
    {"enable-offer-store-unmasked-wallet-cards",
     flag_descriptions::kOfferStoreUnmaskedWalletCardsName,
     flag_descriptions::kOfferStoreUnmaskedWalletCardsDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(
         autofill::switches::kEnableOfferStoreUnmaskedWalletCards,
         autofill::switches::kDisableOfferStoreUnmaskedWalletCards)},
    {"enable-offline-auto-reload", flag_descriptions::kOfflineAutoReloadName,
     flag_descriptions::kOfflineAutoReloadDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableOfflineAutoReload,
                               switches::kDisableOfflineAutoReload)},
    {"enable-offline-auto-reload-visible-only",
     flag_descriptions::kOfflineAutoReloadVisibleOnlyName,
     flag_descriptions::kOfflineAutoReloadVisibleOnlyDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableOfflineAutoReloadVisibleOnly,
                               switches::kDisableOfflineAutoReloadVisibleOnly)},
    {"show-saved-copy", flag_descriptions::kShowSavedCopyName,
     flag_descriptions::kShowSavedCopyDescription, kOsAll,
     MULTI_VALUE_TYPE(kShowSavedCopyChoices)},
    {"default-tile-width", flag_descriptions::kDefaultTileWidthName,
     flag_descriptions::kDefaultTileWidthDescription, kOsAll,
     MULTI_VALUE_TYPE(kDefaultTileWidthChoices)},
    {"default-tile-height", flag_descriptions::kDefaultTileHeightName,
     flag_descriptions::kDefaultTileHeightDescription, kOsAll,
     MULTI_VALUE_TYPE(kDefaultTileHeightChoices)},
#if defined(OS_CHROMEOS)
    {"enable-virtual-keyboard", flag_descriptions::kVirtualKeyboardName,
     flag_descriptions::kVirtualKeyboardDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(keyboard::switches::kEnableVirtualKeyboard)},
    {"virtual-keyboard-overscroll",
     flag_descriptions::kVirtualKeyboardOverscrollName,
     flag_descriptions::kVirtualKeyboardOverscrollDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(
         keyboard::switches::kDisableVirtualKeyboardOverscroll)},
    {"input-view", flag_descriptions::kInputViewName,
     flag_descriptions::kInputViewDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(keyboard::switches::kDisableInputView)},
    {"disable-new-korean-ime", flag_descriptions::kNewKoreanImeName,
     flag_descriptions::kNewKoreanImeDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(chromeos::switches::kDisableNewKoreanIme)},
    {"enable-physical-keyboard-autocorrect",
     flag_descriptions::kPhysicalKeyboardAutocorrectName,
     flag_descriptions::kPhysicalKeyboardAutocorrectDescription, kOsCrOS,
     ENABLE_DISABLE_VALUE_TYPE(
         chromeos::switches::kEnablePhysicalKeyboardAutocorrect,
         chromeos::switches::kDisablePhysicalKeyboardAutocorrect)},
    {"disable-voice-input", flag_descriptions::kVoiceInputName,
     flag_descriptions::kVoiceInputDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(keyboard::switches::kDisableVoiceInput)},
    {"experimental-input-view-features",
     flag_descriptions::kExperimentalInputViewFeaturesName,
     flag_descriptions::kExperimentalInputViewFeaturesDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(
         keyboard::switches::kEnableExperimentalInputViewFeatures)},
    {"gesture-typing", flag_descriptions::kGestureTypingName,
     flag_descriptions::kGestureTypingDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(keyboard::switches::kDisableGestureTyping)},
    {"gesture-editing", flag_descriptions::kGestureEditingName,
     flag_descriptions::kGestureEditingDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(keyboard::switches::kDisableGestureEditing)},
#endif  // OS_CHROMEOS
    {"enable-simple-cache-backend", flag_descriptions::kSimpleCacheBackendName,
     flag_descriptions::kSimpleCacheBackendDescription,
     kOsWin | kOsMac | kOsLinux | kOsCrOS,
     MULTI_VALUE_TYPE(kSimpleCacheBackendChoices)},
    {"enable-tcp-fast-open", flag_descriptions::kTcpFastOpenName,
     flag_descriptions::kTcpFastOpenDescription,
     kOsLinux | kOsCrOS | kOsAndroid,
     SINGLE_VALUE_TYPE(switches::kEnableTcpFastOpen)},
#if BUILDFLAG(ENABLE_SERVICE_DISCOVERY)
    {"device-discovery-notifications",
     flag_descriptions::kDeviceDiscoveryNotificationsName,
     flag_descriptions::kDeviceDiscoveryNotificationsDescription, kOsDesktop,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableDeviceDiscoveryNotifications,
                               switches::kDisableDeviceDiscoveryNotifications)},
    {"enable-print-preview-register-promos",
     flag_descriptions::kPrintPreviewRegisterPromosName,
     flag_descriptions::kPrintPreviewRegisterPromosDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(switches::kEnablePrintPreviewRegisterPromos)},
#endif  // BUILDFLAG(ENABLE_SERVICE_DISCOVERY)
#if defined(OS_WIN)
    {"enable-cloud-print-xps", flag_descriptions::kCloudPrintXpsName,
     flag_descriptions::kCloudPrintXpsDescription, kOsWin,
     SINGLE_VALUE_TYPE(switches::kEnableCloudPrintXps)},
#endif  // OS_WIN
#if BUILDFLAG(ENABLE_SPELLCHECK)
    {"enable-spelling-feedback-field-trial",
     flag_descriptions::kSpellingFeedbackFieldTrialName,
     flag_descriptions::kSpellingFeedbackFieldTrialDescription, kOsAll,
     SINGLE_VALUE_TYPE(
         spellcheck::switches::kEnableSpellingFeedbackFieldTrial)},
#endif  // ENABLE_SPELLCHECK
    {"enable-webgl-draft-extensions",
     flag_descriptions::kWebglDraftExtensionsName,
     flag_descriptions::kWebglDraftExtensionsDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableWebGLDraftExtensions)},
#if !BUILDFLAG(ENABLE_MIRROR)
    {"account-consistency", flag_descriptions::kAccountConsistencyName,
     flag_descriptions::kAccountConsistencyDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(signin::kAccountConsistencyFeature,
                                    kAccountConsistencyFeatureVariations,
                                    "AccountConsistencyVariations")},
#endif
#if BUILDFLAG(ENABLE_APP_LIST)
    {"reset-app-list-install-state",
     flag_descriptions::kResetAppListInstallStateName,
     flag_descriptions::kResetAppListInstallStateDescription,
     kOsMac | kOsWin | kOsLinux,
     SINGLE_VALUE_TYPE(app_list::switches::kResetAppListInstallState)},
#endif  // BUILDFLAG(ENABLE_APP_LIST)
#if defined(OS_ANDROID)
    {"enable-special-locale", flag_descriptions::kEnableSpecialLocaleName,
     flag_descriptions::kEnableSpecialLocaleDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kSpecialLocaleFeature)},
    {"enable-accessibility-tab-switcher",
     flag_descriptions::kAccessibilityTabSwitcherName,
     flag_descriptions::kAccessibilityTabSwitcherDescription, kOsAndroid,
     SINGLE_VALUE_TYPE(switches::kEnableAccessibilityTabSwitcher)},
    {"enable-android-autofill-accessibility",
     flag_descriptions::kAndroidAutofillAccessibilityName,
     flag_descriptions::kAndroidAutofillAccessibilityDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kAndroidAutofillAccessibility)},
    {"enable-physical-web", flag_descriptions::kEnablePhysicalWebName,
     flag_descriptions::kEnablePhysicalWebDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kPhysicalWebFeature)},
#endif  // OS_ANDROID
    {"enable-zero-copy", flag_descriptions::kZeroCopyName,
     flag_descriptions::kZeroCopyDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableZeroCopy,
                               switches::kDisableZeroCopy)},
#if defined(OS_CHROMEOS)
    {"enable-first-run-ui-transitions",
     flag_descriptions::kFirstRunUiTransitionsName,
     flag_descriptions::kFirstRunUiTransitionsDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(chromeos::switches::kEnableFirstRunUITransitions)},
#endif  // OS_CHROMEOS
#if defined(OS_MACOSX)
    {"bookmark-apps", flag_descriptions::kNewBookmarkAppsName,
     flag_descriptions::kNewBookmarkAppsDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kBookmarkApps)},
    {"disable-hosted-apps-in-windows",
     flag_descriptions::kHostedAppsInWindowsName,
     flag_descriptions::kHostedAppsInWindowsDescription, kOsMac,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableHostedAppsInWindows,
                               switches::kDisableHostedAppsInWindows)},
    {"disable-hosted-app-shim-creation",
     flag_descriptions::kHostedAppShimCreationName,
     flag_descriptions::kHostedAppShimCreationDescription, kOsMac,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableHostedAppShimCreation)},
    {"enable-hosted-app-quit-notification",
     flag_descriptions::kHostedAppQuitNotificationName,
     flag_descriptions::kHostedAppQuitNotificationDescription, kOsMac,
     SINGLE_VALUE_TYPE(switches::kHostedAppQuitNotification)},
#endif  // OS_MACOSX
#if defined(OS_ANDROID)
    {"disable-pull-to-refresh-effect",
     flag_descriptions::kPullToRefreshEffectName,
     flag_descriptions::kPullToRefreshEffectDescription, kOsAndroid,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisablePullToRefreshEffect)},
    {"translate-compact-infobar", flag_descriptions::kTranslateCompactUIName,
     flag_descriptions::kTranslateCompactUIDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(translate::kTranslateCompactUI)},
#endif  // OS_ANDROID
#if defined(OS_MACOSX)
    {"enable-translate-new-ux", flag_descriptions::kTranslateNewUxName,
     flag_descriptions::kTranslateNewUxDescription, kOsMac,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableTranslateNewUX,
                               switches::kDisableTranslateNewUX)},
#endif  // OS_MACOSX
#if defined(OS_LINUX) || defined(OS_WIN)
    {"translate-2016q2-ui", flag_descriptions::kTranslate2016q2UiName,
     flag_descriptions::kTranslate2016q2UiDescription,
     kOsCrOS | kOsWin | kOsLinux,
     FEATURE_VALUE_TYPE(translate::kTranslateUI2016Q2)},
#endif  // OS_LINUX || OS_WIN
    {"translate-ranker-enforcement",
     flag_descriptions::kTranslateRankerEnforcementName,
     flag_descriptions::kTranslateRankerEnforcementDescription, kOsAll,
     FEATURE_VALUE_TYPE(translate::kTranslateRankerEnforcement)},
#if BUILDFLAG(ENABLE_NATIVE_NOTIFICATIONS)
    {"enable-native-notifications",
     flag_descriptions::kNotificationsNativeFlagName,
     flag_descriptions::kNotificationsNativeFlagDescription, kOsMac | kOsLinux,
     FEATURE_VALUE_TYPE(features::kNativeNotifications)},
#endif  // ENABLE_NATIVE_NOTIFICATIONS
#if defined(OS_ANDROID)
    {"reader-mode-heuristics", flag_descriptions::kReaderModeHeuristicsName,
     flag_descriptions::kReaderModeHeuristicsDescription, kOsAndroid,
     MULTI_VALUE_TYPE(kReaderModeHeuristicsChoices)},
#endif  // OS_ANDROID
#if defined(OS_ANDROID)
    {"enable-chrome-home", flag_descriptions::kChromeHomeName,
     flag_descriptions::kChromeHomeDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kChromeHomeFeature)},
    {"enable-chrome-home-promo", flag_descriptions::kChromeHomePromoName,
     flag_descriptions::kChromeHomePromoDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kChromeHomePromo)},
    {"enable-chrome-home-bottom-nav-labels",
     flag_descriptions::kChromeHomeBottomNavLabelsName,
     flag_descriptions::kChromeHomeBottomNavLabelsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kChromeHomeBottomNavLabels)},
    {"enable-chrome-home-clear-url-on-open",
     flag_descriptions::kChromeHomeClearUrlOnOpenName,
     flag_descriptions::kChromeHomeClearUrlOnOpenDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kChromeHomeClearUrlOnOpen)},
    {"enable-chrome-home-bottom-sheet-inactivity-expansion",
     flag_descriptions::kChromeHomeInactivitySheetExpansionName,
     flag_descriptions::kChromeHomeInactivitySheetExpansionDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kChromeHomeInactivitySheetExpansion)},
    {"enable-chrome-home-opt-out-snackbar",
     flag_descriptions::kChromeHomeOptOutSnackbarName,
     flag_descriptions::kChromeHomeOptOutSnackbarDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kChromeHomeOptOutSnackbar)},
    {"enable-chrome-home-personalized-omnibox-suggestions",
     flag_descriptions::kChromeHomePersonalizedOmniboxSuggestionsName,
     flag_descriptions::kChromeHomePersonalizedOmniboxSuggestionsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(omnibox::kAndroidChromeHomePersonalizedSuggestions)},
    {"chrome-home-swipe-logic", flag_descriptions::kChromeHomeSwipeLogicName,
     flag_descriptions::kChromeHomeSwipeLogicDescription, kOsAndroid,
     MULTI_VALUE_TYPE(kChromeHomeSwipeLogicChoices)},
    {"enable-chrome-home-persistent-iph",
     flag_descriptions::kChromeHomePersistentIphName,
     flag_descriptions::kChromeHomePersistentIphDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kChromeHomePersistentIph)},
    {"enable-chrome-memex", flag_descriptions::kChromeMemexName,
     flag_descriptions::kChromeMemexDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kChromeMemexFeature)},
#endif  // OS_ANDROID
#if defined(OS_ANDROID)
    {"enable-tab-modal-js-dialog-android",
     flag_descriptions::kTabModalJsDialogName,
     flag_descriptions::kTabModalJsDialogDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kTabModalJsDialog)},
#endif  // OS_ANDROID
    {"in-product-help-demo-mode-choice",
     flag_descriptions::kInProductHelpDemoModeChoiceName,
     flag_descriptions::kInProductHelpDemoModeChoiceDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         feature_engagement::kIPHDemoMode,
         feature_engagement::kIPHDemoModeChoiceVariations,
         "IPH_DemoMode")},
    {"num-raster-threads", flag_descriptions::kNumRasterThreadsName,
     flag_descriptions::kNumRasterThreadsDescription, kOsAll,
     MULTI_VALUE_TYPE(kNumRasterThreadsChoices)},
    {"enable-permission-action-reporting",
     flag_descriptions::kPermissionActionReportingName,
     flag_descriptions::kPermissionActionReportingDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnablePermissionActionReporting,
                               switches::kDisablePermissionActionReporting)},
    {"enable-permissions-blacklist",
     flag_descriptions::kPermissionsBlacklistName,
     flag_descriptions::kPermissionsBlacklistDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kPermissionsBlacklist)},
    {"enable-single-click-autofill",
     flag_descriptions::kSingleClickAutofillName,
     flag_descriptions::kSingleClickAutofillDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(
         autofill::switches::kEnableSingleClickAutofill,
         autofill::switches::kDisableSingleClickAutofill)},
    {"disable-cast-streaming-hw-encoding",
     flag_descriptions::kCastStreamingHwEncodingName,
     flag_descriptions::kCastStreamingHwEncodingDescription, kOsAll,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableCastStreamingHWEncoding)},
    {"disable-threaded-scrolling", flag_descriptions::kThreadedScrollingName,
     flag_descriptions::kThreadedScrollingDescription, kOsAll,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableThreadedScrolling)},
    {"distance-field-text", flag_descriptions::kDistanceFieldTextName,
     flag_descriptions::kDistanceFieldTextDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableDistanceFieldText,
                               switches::kDisableDistanceFieldText)},
    {"extension-content-verification",
     flag_descriptions::kExtensionContentVerificationName,
     flag_descriptions::kExtensionContentVerificationDescription, kOsDesktop,
     MULTI_VALUE_TYPE(kExtensionContentVerificationChoices)},
#if BUILDFLAG(ENABLE_EXTENSIONS)
    {"extension-active-script-permission",
     flag_descriptions::kUserConsentForExtensionScriptsName,
     flag_descriptions::kUserConsentForExtensionScriptsDescription, kOsAll,
     SINGLE_VALUE_TYPE(extensions::switches::kEnableScriptsRequireAction)},
#endif  // ENABLE_EXTENSIONS
#if BUILDFLAG(ENABLE_EXTENSIONS)
    {"enable-embedded-extension-options",
     flag_descriptions::kEmbeddedExtensionOptionsName,
     flag_descriptions::kEmbeddedExtensionOptionsDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(extensions::switches::kEnableEmbeddedExtensionOptions)},
#endif  // ENABLE_EXTENSIONS
#if !defined(OS_ANDROID)
#if defined(OS_CHROMEOS)
    {"enable-ash-sidebar", flag_descriptions::kAshSidebarName,
     flag_descriptions::kAshSidebarDescription, kOsCrOS,
     ENABLE_DISABLE_VALUE_TYPE(ash::switches::kAshSidebarEnabled,
                               ash::switches::kAshSidebarDisabled)},
#endif  // OS_CHROMEOS
    {"enable-message-center-new-style-notification",
     flag_descriptions::kMessageCenterNewStyleNotificationName,
     flag_descriptions::kMessageCenterNewStyleNotificationDescription,
     kOsDesktop,
     ENABLE_DISABLE_VALUE_TYPE(
         message_center::switches::kEnableMessageCenterNewStyleNotification,
         message_center::switches::kDisableMessageCenterNewStyleNotification)},
    {"enable-policy-tool", flag_descriptions::kEnablePolicyToolName,
     flag_descriptions::kEnablePolicyToolDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kPolicyTool)},
#endif  // !OS_ANDROID
#if defined(OS_CHROMEOS)
    {"memory-pressure-thresholds",
     flag_descriptions::kMemoryPressureThresholdName,
     flag_descriptions::kMemoryPressureThresholdDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kMemoryPressureThresholdChoices)},
    {"wake-on-wifi-packet", flag_descriptions::kWakeOnPacketsName,
     flag_descriptions::kWakeOnPacketsDescription, kOsCrOSOwnerOnly,
     SINGLE_VALUE_TYPE(chromeos::switches::kWakeOnWifiPacket)},
#endif  // OS_CHROMEOS
    {"enable-memory-coordinator", flag_descriptions::kMemoryCoordinatorName,
     flag_descriptions::kMemoryCoordinatorDescription,
     kOsAndroid | kOsCrOS | kOsLinux | kOsWin,
     FEATURE_VALUE_TYPE(features::kMemoryCoordinator)},
    {"enable-tab-audio-muting", flag_descriptions::kTabAudioMutingName,
     flag_descriptions::kTabAudioMutingDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(switches::kEnableTabAudioMuting)},
    {"reduced-referrer-granularity",
     flag_descriptions::kReducedReferrerGranularityName,
     flag_descriptions::kReducedReferrerGranularityDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kReducedReferrerGranularity)},
#if defined(OS_CHROMEOS)
    {"disable-new-zip-unpacker", flag_descriptions::kNewZipUnpackerName,
     flag_descriptions::kNewZipUnpackerDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(chromeos::switches::kDisableNewZIPUnpacker)},
#endif  // OS_CHROMEOS
#if defined(OS_ANDROID)
    {"enable-credit-card-assist", flag_descriptions::kCreditCardAssistName,
     flag_descriptions::kCreditCardAssistDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(autofill::kAutofillCreditCardAssist)},
#endif  // OS_ANDROID
#if defined(OS_CHROMEOS)
    {"disable-captive-portal-bypass-proxy",
     flag_descriptions::kCaptivePortalBypassProxyName,
     flag_descriptions::kCaptivePortalBypassProxyDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(
         chromeos::switches::kDisableCaptivePortalBypassProxy)},
#endif  // OS_CHROMEOS
#if defined(OS_CHROMEOS)
    {"enable-wifi-credential-sync", flag_descriptions::kWifiCredentialSyncName,
     flag_descriptions::kWifiCredentialSyncDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(switches::kEnableWifiCredentialSync)},
    {"enable-potentially-annoying-security-features",
     flag_descriptions::kExperimentalSecurityFeaturesName,
     flag_descriptions::kExperimentalSecurityFeaturesDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnablePotentiallyAnnoyingSecurityFeatures)},
#endif  // OS_CHROMEOS
    {"mark-non-secure-as", flag_descriptions::kMarkHttpAsName,
     flag_descriptions::kMarkHttpAsDescription, kOsAll,
     MULTI_VALUE_TYPE(kMarkHttpAsChoices)},
    {"enable-http-form-warning", flag_descriptions::kEnableHttpFormWarningName,
     flag_descriptions::kEnableHttpFormWarningDescription, kOsAll,
     FEATURE_VALUE_TYPE(security_state::kHttpFormWarningFeature)},
    {"committed-interstitials", flag_descriptions::kCommittedInterstitialsName,
     flag_descriptions::kCommittedInterstitialsDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kCommittedInterstitials)},
    {"enable-site-per-process", flag_descriptions::kSitePerProcessName,
     flag_descriptions::kSitePerProcessDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kSitePerProcess)},
    {"enable-top-document-isolation",
     flag_descriptions::kTopDocumentIsolationName,
     flag_descriptions::kTopDocumentIsolationDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kTopDocumentIsolation)},
    {"enable-use-zoom-for-dsf", flag_descriptions::kEnableUseZoomForDsfName,
     flag_descriptions::kEnableUseZoomForDsfDescription, kOsAll,
     MULTI_VALUE_TYPE(kEnableUseZoomForDSFChoices)},
#if defined(OS_MACOSX)
    {"enable-harfbuzz-rendertext", flag_descriptions::kHarfbuzzRendertextName,
     flag_descriptions::kHarfbuzzRendertextDescription, kOsMac,
     SINGLE_VALUE_TYPE(switches::kEnableHarfBuzzRenderText)},
#endif  // OS_MACOSX
    {"data-reduction-proxy-lo-fi",
     flag_descriptions::kDataReductionProxyLoFiName,
     flag_descriptions::kDataReductionProxyLoFiDescription, kOsAll,
     MULTI_VALUE_TYPE(kDataReductionProxyLoFiChoices)},
    {"enable-data-reduction-proxy-lite-page",
     flag_descriptions::kEnableDataReductionProxyLitePageName,
     flag_descriptions::kEnableDataReductionProxyLitePageDescription, kOsAll,
     SINGLE_VALUE_TYPE(
         data_reduction_proxy::switches::kEnableDataReductionProxyLitePage)},
    {"enable-data-reduction-proxy-server-experiment",
     flag_descriptions::kEnableDataReductionProxyServerExperimentName,
     flag_descriptions::kEnableDataReductionProxyServerExperimentDescription,
     kOsAll, MULTI_VALUE_TYPE(kDataReductionProxyServerExperiment)},
#if defined(OS_ANDROID)
    {"enable-data-reduction-proxy-savings-promo",
     flag_descriptions::kEnableDataReductionProxySavingsPromoName,
     flag_descriptions::kEnableDataReductionProxySavingsPromoDescription,
     kOsAndroid,
     SINGLE_VALUE_TYPE(data_reduction_proxy::switches::
                           kEnableDataReductionProxySavingsPromo)},
    {"enable-data-reduction-proxy-main-menu",
     flag_descriptions::kEnableDataReductionProxyMainMenuName,
     flag_descriptions::kEnableDataReductionProxyMainMenuDescription,
     kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         data_reduction_proxy::features::kDataReductionMainMenu,
         kDataReductionMainMenuFeatureVariations,
         "DataReductionProxyMainMenu")},
    {"enable-data-reduction-proxy-site-breakdown",
     flag_descriptions::kEnableDataReductionProxySiteBreakdownName,
     flag_descriptions::kEnableDataReductionProxySiteBreakdownDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         data_reduction_proxy::features::kDataReductionSiteBreakdown)},
    {"enable-offline-previews", flag_descriptions::kEnableOfflinePreviewsName,
     flag_descriptions::kEnableOfflinePreviewsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(previews::features::kOfflinePreviews)},
#endif  // OS_ANDROID
    {"enable-client-lo-fi", flag_descriptions::kEnableClientLoFiName,
     flag_descriptions::kEnableClientLoFiDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         previews::features::kClientLoFi,
         kClientPlaceholdersForServerLoFiFeatureVariations,
         "PreviewsClientLoFi")},
    {"enable-noscript-previews", flag_descriptions::kEnableNoScriptPreviewsName,
     flag_descriptions::kEnableNoScriptPreviewsDescription, kOsAll,
     FEATURE_VALUE_TYPE(previews::features::kNoScriptPreviews)},
    {"enable-optimization-hints",
     flag_descriptions::kEnableOptimizationHintsName,
     flag_descriptions::kEnableOptimizationHintsDescription, kOsAll,
     FEATURE_VALUE_TYPE(previews::features::kOptimizationHints)},
    {"allow-insecure-localhost", flag_descriptions::kAllowInsecureLocalhostName,
     flag_descriptions::kAllowInsecureLocalhostDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kAllowInsecureLocalhost)},
#if !defined(OS_ANDROID)
    {"enable-app-banners", flag_descriptions::kAppBannersName,
     flag_descriptions::kAppBannersDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kAppBanners)},
#endif  // !OS_ANDROID
    {"enable-experimental-app-banners",
     flag_descriptions::kExperimentalAppBannersName,
     flag_descriptions::kExperimentalAppBannersDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kExperimentalAppBanners)},
    {"bypass-app-banner-engagement-checks",
     flag_descriptions::kBypassAppBannerEngagementChecksName,
     flag_descriptions::kBypassAppBannerEngagementChecksDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kBypassAppBannerEngagementChecks)},
    {"enable-desktop-pwas", flag_descriptions::kEnableDesktopPWAsName,
     flag_descriptions::kEnableDesktopPWAsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kDesktopPWAWindowing)},
    {"use-sync-sandbox", flag_descriptions::kSyncSandboxName,
     flag_descriptions::kSyncSandboxDescription, kOsAll,
     SINGLE_VALUE_TYPE_AND_VALUE(
         switches::kSyncServiceURL,
         "https://chrome-sync.sandbox.google.com/chrome-sync/alpha")},
#if !defined(OS_ANDROID)
    {"load-media-router-component-extension",
     flag_descriptions::kLoadMediaRouterComponentExtensionName,
     flag_descriptions::kLoadMediaRouterComponentExtensionDescription,
     kOsDesktop,
     ENABLE_DISABLE_VALUE_TYPE_AND_VALUE(
         switches::kLoadMediaRouterComponentExtension,
         "1",
         switches::kLoadMediaRouterComponentExtension,
         "0")},
#endif  // !OS_ANDROID
// Since Drive Search is not available when app list is disabled, flag guard
// enable-drive-search-in-chrome-launcher flag.
#if BUILDFLAG(ENABLE_APP_LIST)
    {"enable-drive-search-in-app-launcher",
     flag_descriptions::kDriveSearchInChromeLauncherName,
     flag_descriptions::kDriveSearchInChromeLauncherDescription, kOsCrOS,
     ENABLE_DISABLE_VALUE_TYPE(
         app_list::switches::kEnableDriveSearchInChromeLauncher,
         app_list::switches::kDisableDriveSearchInChromeLauncher)},
#endif  // BUILDFLAG(ENABLE_APP_LIST)
#if defined(OS_CHROMEOS)
    {"disable-mtp-write-support", flag_descriptions::kMtpWriteSupportName,
     flag_descriptions::kMtpWriteSupportDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(chromeos::switches::kDisableMtpWriteSupport)},
#endif  // OS_CHROMEOS
#if defined(OS_CHROMEOS)
    {"enable-datasaver-prompt", flag_descriptions::kDatasaverPromptName,
     flag_descriptions::kDatasaverPromptDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kDataSaverPromptChoices)},
#endif  // OS_CHROMEOS
#if defined(OS_ANDROID)
    {"autofill-keyboard-accessory-view",
     flag_descriptions::kAutofillAccessoryViewName,
     flag_descriptions::kAutofillAccessoryViewDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(autofill::kAutofillKeyboardAccessory,
                                    kAutofillKeyboardAccessoryFeatureVariations,
                                    "AutofillKeyboardAccessoryVariations")},
#endif  // OS_ANDROID
#if defined(OS_WIN)
    {"try-supported-channel-layouts",
     flag_descriptions::kTrySupportedChannelLayoutsName,
     flag_descriptions::kTrySupportedChannelLayoutsDescription, kOsWin,
     SINGLE_VALUE_TYPE(switches::kTrySupportedChannelLayouts)},
#endif  // OS_WIN
#if defined(OS_MACOSX)
    {"app-info-dialog", flag_descriptions::kAppInfoDialogName,
     flag_descriptions::kAppInfoDialogDescription, kOsMac,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableAppInfoDialogMac,
                               switches::kDisableAppInfoDialogMac)},
    {"mac-v2-sandbox", flag_descriptions::kMacV2SandboxName,
     flag_descriptions::kMacV2SandboxDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kMacV2Sandbox)},
    {"mac-views-native-app-windows",
     flag_descriptions::kMacViewsNativeAppWindowsName,
     flag_descriptions::kMacViewsNativeAppWindowsDescription, kOsMac,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableMacViewsNativeAppWindows,
                               switches::kDisableMacViewsNativeAppWindows)},
    {"mac-views-profile-chooser",
     flag_descriptions::kMacViewsProfileChooserName,
     flag_descriptions::kMacViewsProfileChooserDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kViewsProfileChooser)},
    {"mac-views-task-manager", flag_descriptions::kMacViewsTaskManagerName,
     flag_descriptions::kMacViewsTaskManagerDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kViewsTaskManager)},
    {"show-all-dialogs-with-views-toolkit",
     flag_descriptions::kShowAllDialogsWithViewsToolkitName,
     flag_descriptions::kShowAllDialogsWithViewsToolkitDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kShowAllDialogsWithViewsToolkit)},
    {"app-window-cycling", flag_descriptions::kAppWindowCyclingName,
     flag_descriptions::kAppWindowCyclingDescription, kOsMac,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableAppWindowCycling,
                               switches::kDisableAppWindowCycling)},
#endif  // OS_MACOSX
    {"enable-gamepad-extensions", flag_descriptions::kGamepadExtensionsName,
     flag_descriptions::kGamepadExtensionsDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kGamepadExtensions)},
    // TODO(bajones): When adding a flag for the "2.0" API (crbug.com/670510),
    // use FEATURE_VALUE_TYPE, just use "webvr" without "enable-", and update
    // the description to indicate this is the older version of the API ("1.1").
    {"enable-webvr", flag_descriptions::kWebvrName,
     flag_descriptions::kWebvrDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableWebVR)},
    {"webvr-experimental-rendering",
     flag_descriptions::kWebVrExperimentalRenderingName,
     flag_descriptions::kWebVrExperimentalRenderingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kWebVrExperimentalRendering)},
#if BUILDFLAG(ENABLE_VR)
    {"webvr-vsync-align", flag_descriptions::kWebVrVsyncAlignName,
     flag_descriptions::kWebVrVsyncAlignDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kWebVrVsyncAlign)},
#if defined(OS_ANDROID)
    // TODO(crbug.com/731802): Use #if BUILDFLAG(ENABLE_VR_BROWSING) instead.
    {"vr-browsing", flag_descriptions::kVrBrowsingName,
     flag_descriptions::kVrBrowsingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kVrBrowsing)},
    {"vr-browser-keyboard", flag_descriptions::kVrBrowserKeyboardName,
     flag_descriptions::kVrBrowserKeyboardDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kVrBrowserKeyboard)},
    {"vr-browsing-experimental-features",
     flag_descriptions::kVrBrowsingExperimentalFeaturesName,
     flag_descriptions::kVrBrowsingExperimentalFeaturesDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kVrBrowsingExperimentalFeatures)},
    {"vr-browsing-experimental-rendering",
     flag_descriptions::kVrBrowsingExperimentalRenderingName,
     flag_descriptions::kVrBrowsingExperimentalRenderingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kVrBrowsingExperimentalRendering)},
    // TODO(crbug.com/731802): Only these should be #if defined(OS_ANDROID).
    {"vr-browsing-in-custom-tab", flag_descriptions::kVrBrowsingInCustomTabName,
     flag_descriptions::kVrBrowsingInCustomTabDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kVrBrowsingInCustomTab)},
    {"vr-launch-intents", flag_descriptions::kVrLaunchIntentsName,
     flag_descriptions::kVrLaunchIntentsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kVrLaunchIntents)},
    {"webvr-autopresent-from-intent",
     flag_descriptions::kWebVrAutopresentFromIntentName,
     flag_descriptions::kWebVrAutopresentFromIntentDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kWebVrAutopresentFromIntent)},
#endif  // OS_ANDROID
#if BUILDFLAG(ENABLE_OPENVR)
    {"openvr", flag_descriptions::kOpenVRName,
     flag_descriptions::kOpenVRDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kOpenVR)},
#endif  // ENABLE_OPENVR
#endif  // ENABLE_VR
#if defined(OS_CHROMEOS)
    {"disable-accelerated-mjpeg-decode",
     flag_descriptions::kAcceleratedMjpegDecodeName,
     flag_descriptions::kAcceleratedMjpegDecodeDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableAcceleratedMjpegDecode)},
#endif  // OS_CHROMEOS
    {"v8-cache-options", flag_descriptions::kV8CacheOptionsName,
     flag_descriptions::kV8CacheOptionsDescription, kOsAll,
     MULTI_VALUE_TYPE(kV8CacheOptionsChoices)},
    {"simplified-fullscreen-ui", flag_descriptions::kSimplifiedFullscreenUiName,
     flag_descriptions::kSimplifiedFullscreenUiDescription,
     kOsWin | kOsLinux | kOsCrOS,
     FEATURE_VALUE_TYPE(features::kSimplifiedFullscreenUI)},
    {"experimental-keyboard-lock-ui",
     flag_descriptions::kExperimentalKeyboardLockUiName,
     flag_descriptions::kExperimentalKeyboardLockUiDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kExperimentalKeyboardLockUI)},
#if defined(OS_ANDROID)
    {"progress-bar-throttle", flag_descriptions::kProgressBarThrottleName,
     flag_descriptions::kProgressBarThrottleDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kProgressBarThrottleFeature)},
    {"progress-bar-completion", flag_descriptions::kProgressBarCompletionName,
     flag_descriptions::kProgressBarCompletionDescription, kOsAndroid,
     MULTI_VALUE_TYPE(kProgressBarCompletionChoices)},
#endif  // OS_ANDROID
    {"save-previous-document-resources-until",
     flag_descriptions::kSavePreviousDocumentResourcesName,
     flag_descriptions::kSavePreviousDocumentResourcesDescription, kOsAll,
     MULTI_VALUE_TYPE(kSavePreviousDocumentResourcesChoices)},
#if defined(OS_ANDROID)
    {"offline-bookmarks", flag_descriptions::kOfflineBookmarksName,
     flag_descriptions::kOfflineBookmarksDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(offline_pages::kOfflineBookmarksFeature)},
    {"offline-pages-async-download",
     flag_descriptions::kOfflinePagesAsyncDownloadName,
     flag_descriptions::kOfflinePagesAsyncDownloadDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(offline_pages::kOfflinePagesAsyncDownloadFeature)},
    {"offline-pages-load-signal-collecting",
     flag_descriptions::kOfflinePagesLoadSignalCollectingName,
     flag_descriptions::kOfflinePagesLoadSignalCollectingDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         offline_pages::kOfflinePagesLoadSignalCollectingFeature)},
    {"offline-pages-sharing", flag_descriptions::kOfflinePagesSharingName,
     flag_descriptions::kOfflinePagesSharingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(offline_pages::kOfflinePagesSharingFeature)},
    {"offline-pages-prefetching",
     flag_descriptions::kOfflinePagesPrefetchingName,
     flag_descriptions::kOfflinePagesPrefetchingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(offline_pages::kPrefetchingOfflinePagesFeature)},
    {"offline-pages-prefetching-ui",
     flag_descriptions::kOfflinePagesPrefetchingUIName,
     flag_descriptions::kOfflinePagesPrefetchingUIDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(offline_pages::kOfflinePagesPrefetchingUIFeature)},
    {"background-loader-for-downloads",
     flag_descriptions::kBackgroundLoaderForDownloadsName,
     flag_descriptions::kBackgroundLoaderForDownloadsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(offline_pages::kBackgroundLoaderForDownloadsFeature)},
    {"offline-pages-resource-based-snapshot",
     flag_descriptions::kOfflinePagesResourceBasedSnapshotName,
     flag_descriptions::kOfflinePagesResourceBasedSnapshotDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         offline_pages::kOfflinePagesResourceBasedSnapshotFeature)},
    {"offline-pages-renovations",
     flag_descriptions::kOfflinePagesRenovationsName,
     flag_descriptions::kOfflinePagesRenovationsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(offline_pages::kOfflinePagesRenovationsFeature)},
#endif  // OS_ANDROID
    {"disallow-doc-written-script-loads",
     flag_descriptions::kDisallowDocWrittenScriptsUiName,
     flag_descriptions::kDisallowDocWrittenScriptsUiDescription, kOsAll,
     // NOTE: if we want to add additional experiment entries for other
     // features controlled by kBlinkSettings, we'll need to add logic to
     // merge the flag values.
     ENABLE_DISABLE_VALUE_TYPE_AND_VALUE(
         switches::kBlinkSettings,
         "disallowFetchForDocWrittenScriptsInMainFrame=true",
         switches::kBlinkSettings,
         "disallowFetchForDocWrittenScriptsInMainFrame=false")},
#if defined(OS_ANDROID)
    {"enable-ntp-popular-sites", flag_descriptions::kNtpPopularSitesName,
     flag_descriptions::kNtpPopularSitesDescription, kOsAndroid,
     ENABLE_DISABLE_VALUE_TYPE(ntp_tiles::switches::kEnableNTPPopularSites,
                               ntp_tiles::switches::kDisableNTPPopularSites)},
#endif  // OS_ANDROID
#if defined(OS_WIN)
    {"trace-export-events-to-etw",
     flag_descriptions::kTraceExportEventsToEtwName,
     flag_descriptions::kTraceExportEventsToEtwDesription, kOsWin,
     SINGLE_VALUE_TYPE(switches::kTraceExportEventsToETW)},
    {"merge-key-char-events", flag_descriptions::kMergeKeyCharEventsName,
     flag_descriptions::kMergeKeyCharEventsDescription, kOsWin,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableMergeKeyCharEvents,
                               switches::kDisableMergeKeyCharEvents)},
    {"use-winrt-midi-api", flag_descriptions::kUseWinrtMidiApiName,
     flag_descriptions::kUseWinrtMidiApiDescription, kOsWin,
     FEATURE_VALUE_TYPE(midi::features::kMidiManagerWinrt)},
#endif  // OS_WIN
#if BUILDFLAG(ENABLE_BACKGROUND)
    {"enable-push-api-background-mode",
     flag_descriptions::kPushApiBackgroundModeName,
     flag_descriptions::kPushApiBackgroundModeDescription,
     kOsMac | kOsWin | kOsLinux,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnablePushApiBackgroundMode,
                               switches::kDisablePushApiBackgroundMode)},
#endif  // BUILDFLAG(ENABLE_BACKGROUND)
#if defined(OS_CHROMEOS)
    {"cros-regions-mode", flag_descriptions::kCrosRegionsModeName,
     flag_descriptions::kCrosRegionsModeDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kCrosRegionsModeChoices)},
#endif  // OS_CHROMEOS
#if defined(OS_WIN)
    {"enable-appcontainer", flag_descriptions::kEnableAppcontainerName,
     flag_descriptions::kEnableAppcontainerDescription, kOsWin,
     ENABLE_DISABLE_VALUE_TYPE(
         service_manager::switches::kEnableAppContainer,
         service_manager::switches::kDisableAppContainer)},
#endif  // OS_WIN
#if defined(TOOLKIT_VIEWS) || defined(OS_ANDROID)
    {"enable-autofill-credit-card-upload",
     flag_descriptions::kAutofillCreditCardUploadName,
     flag_descriptions::kAutofillCreditCardUploadDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(
         autofill::switches::kEnableOfferUploadCreditCards,
         autofill::switches::kDisableOfferUploadCreditCards)},
#endif  // TOOLKIT_VIEWS || OS_ANDROID
#if defined(OS_ANDROID)
    {"tab-management-experiment-type",
     flag_descriptions::kHerbPrototypeChoicesName,
     flag_descriptions::kHerbPrototypeChoicesDescription, kOsAndroid,
     MULTI_VALUE_TYPE(kHerbPrototypeChoices)},
#endif  // OS_ANDROID
#if defined(OS_MACOSX)
    {"mac-md-download-shelf",
     flag_descriptions::kEnableMacMaterialDesignDownloadShelfName,
     flag_descriptions::kEnableMacMaterialDesignDownloadShelfDescription,
     kOsMac, FEATURE_VALUE_TYPE(features::kMacMaterialDesignDownloadShelf)},
#endif  // OS_MACOSX
    {"enable-md-bookmarks",
     flag_descriptions::kEnableMaterialDesignBookmarksName,
     flag_descriptions::kEnableMaterialDesignBookmarksDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kMaterialDesignBookmarks)},
    {"enable-md-incognito-ntp",
     flag_descriptions::kMaterialDesignIncognitoNTPName,
     flag_descriptions::kMaterialDesignIncognitoNTPDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kMaterialDesignIncognitoNTP)},
    {"safe-search-url-reporting",
     flag_descriptions::kSafeSearchUrlReportingName,
     flag_descriptions::kSafeSearchUrlReportingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kSafeSearchUrlReporting)},
    {"force-ui-direction", flag_descriptions::kForceUiDirectionName,
     flag_descriptions::kForceUiDirectionDescription, kOsAll,
     MULTI_VALUE_TYPE(kForceUIDirectionChoices)},
    {"force-text-direction", flag_descriptions::kForceTextDirectionName,
     flag_descriptions::kForceTextDirectionDescription, kOsAll,
     MULTI_VALUE_TYPE(kForceTextDirectionChoices)},
#if BUILDFLAG(ENABLE_EXTENSIONS)
    {"enable-md-extensions",
     flag_descriptions::kEnableMaterialDesignExtensionsName,
     flag_descriptions::kEnableMaterialDesignExtensionsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kMaterialDesignExtensions)},
#endif  // ENABLE_EXTENSIONS
#if defined(OS_WIN) || defined(OS_LINUX)
    {"enable-input-ime-api", flag_descriptions::kEnableInputImeApiName,
     flag_descriptions::kEnableInputImeApiDescription, kOsWin | kOsLinux,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableInputImeAPI,
                               switches::kDisableInputImeAPI)},
#endif  // OS_WIN || OS_LINUX
    {"enable-origin-trials", flag_descriptions::kOriginTrialsName,
     flag_descriptions::kOriginTrialsDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kOriginTrials)},
    {"enable-brotli", flag_descriptions::kEnableBrotliName,
     flag_descriptions::kEnableBrotliDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kBrotliEncoding)},
    {"enable-image-capture-api", flag_descriptions::kEnableImageCaptureAPIName,
     flag_descriptions::kEnableImageCaptureAPIDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kImageCaptureAPI)},
#if defined(OS_ANDROID)
    {"force-show-update-menu-item", flag_descriptions::kUpdateMenuItemName,
     flag_descriptions::kUpdateMenuItemDescription, kOsAndroid,
     SINGLE_VALUE_TYPE(switches::kForceShowUpdateMenuItem)},
    {"update-menu-item-custom-summary",
     flag_descriptions::kUpdateMenuItemCustomSummaryName,
     flag_descriptions::kUpdateMenuItemCustomSummaryDescription, kOsAndroid,
     SINGLE_VALUE_TYPE_AND_VALUE(
         switches::kForceShowUpdateMenuItemCustomSummary,
         "Custom Summary")},
    {"force-show-update-menu-badge", flag_descriptions::kUpdateMenuBadgeName,
     flag_descriptions::kUpdateMenuBadgeDescription, kOsAndroid,
     SINGLE_VALUE_TYPE(switches::kForceShowUpdateMenuBadge)},
    {"set-market-url-for-testing",
     flag_descriptions::kSetMarketUrlForTestingName,
     flag_descriptions::kSetMarketUrlForTestingDescription, kOsAndroid,
     SINGLE_VALUE_TYPE_AND_VALUE(
         switches::kMarketUrlForTesting,
         "https://play.google.com/store/apps/details?id=com.android.chrome")},
#endif  // OS_ANDROID
#if defined(OS_WIN) || defined(OS_MACOSX)
    {"automatic-tab-discarding", flag_descriptions::kAutomaticTabDiscardingName,
     flag_descriptions::kAutomaticTabDiscardingDescription, kOsWin | kOsMac,
     FEATURE_VALUE_TYPE(features::kAutomaticTabDiscarding)},
#endif  // OS_WIN || OS_MACOSX
    {"tls13-variant", flag_descriptions::kTLS13VariantName,
     flag_descriptions::kTLS13VariantDescription, kOsAll,
     MULTI_VALUE_TYPE(kTLS13VariantChoices)},
    {"enable-token-binding", flag_descriptions::kEnableTokenBindingName,
     flag_descriptions::kEnableTokenBindingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kTokenBinding)},
    {"enable-scroll-anchoring", flag_descriptions::kEnableScrollAnchoringName,
     flag_descriptions::kEnableScrollAnchoringDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kScrollAnchoring)},
    {"disable-audio-support-for-desktop-share",
     flag_descriptions::kDisableAudioForDesktopShareName,
     flag_descriptions::kDisableAudioForDesktopShareDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kDisableAudioSupportForDesktopShare)},
#if BUILDFLAG(ENABLE_EXTENSIONS)
    {"tab-for-desktop-share", flag_descriptions::kDisableTabForDesktopShareName,
     flag_descriptions::kDisableTabForDesktopShareDescription, kOsAll,
     SINGLE_VALUE_TYPE(extensions::switches::kDisableTabForDesktopShare)},
#endif  // ENABLE_EXTENSIONS
#if defined(OS_ANDROID)
    {"keep-prefetched-content-suggestions",
     flag_descriptions::kKeepPrefetchedContentSuggestionsName,
     flag_descriptions::kKeepPrefetchedContentSuggestionsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(ntp_snippets::kKeepPrefetchedContentSuggestions)},
    {"content-suggestions-category-order",
     flag_descriptions::kContentSuggestionsCategoryOrderName,
     flag_descriptions::kContentSuggestionsCategoryOrderDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         ntp_snippets::kCategoryOrder,
         kContentSuggestionsCategoryOrderFeatureVariations,
         ntp_snippets::kCategoryOrder.name)},
    {"content-suggestions-category-ranker",
     flag_descriptions::kContentSuggestionsCategoryRankerName,
     flag_descriptions::kContentSuggestionsCategoryRankerDescription,
     kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         ntp_snippets::kCategoryRanker,
         kContentSuggestionsCategoryRankerFeatureVariations,
         ntp_snippets::kCategoryRanker.name)},
    {"content-suggestions-debug-log",
     flag_descriptions::kContentSuggestionsDebugLogName,
     flag_descriptions::kContentSuggestionsDebugLogDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(ntp_snippets::kContentSuggestionsDebugLog)},
    {"enable-breaking-news-push",
     flag_descriptions::kEnableBreakingNewsPushName,
     flag_descriptions::kEnableBreakingNewsPushDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(ntp_snippets::kBreakingNewsPushFeature)},
    {"enable-ntp-snippets-increased-visibility",
     flag_descriptions::kEnableNtpSnippetsVisibilityName,
     flag_descriptions::kEnableNtpSnippetsVisibilityDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(ntp_snippets::kIncreasedVisibility)},
    {"enable-contextual-suggestions-carousel",
     flag_descriptions::kContextualSuggestionsCarouselName,
     flag_descriptions::kContextualSuggestionsCarouselDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kContextualSuggestionsCarousel)},
    {"enable-content-suggestions-new-favicon-server",
     flag_descriptions::kEnableContentSuggestionsNewFaviconServerName,
     flag_descriptions::kEnableContentSuggestionsNewFaviconServerDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(ntp_snippets::kPublisherFaviconsFromNewServerFeature)},
    {"enable-content-suggestions-thumbnail-dominant-color",
     flag_descriptions::kEnableContentSuggestionsThumbnailDominantColorName,
     flag_descriptions::
         kEnableContentSuggestionsThumbnailDominantColorDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         chrome::android::kContentSuggestionsThumbnailDominantColor)},
    {"enable-content-suggestions-settings",
     flag_descriptions::kEnableContentSuggestionsSettingsName,
     flag_descriptions::kEnableContentSuggestionsSettingsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kContentSuggestionsSettings)},
    {"enable-favicons-from-web-manifest",
     flag_descriptions::kEnableFaviconsFromWebManifestName,
     flag_descriptions::kEnableFaviconsFromWebManifestDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(favicon::kFaviconsFromWebManifest)},
    {"enable-ntp-remote-suggestions",
     flag_descriptions::kEnableNtpRemoteSuggestionsName,
     flag_descriptions::kEnableNtpRemoteSuggestionsDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         ntp_snippets::kArticleSuggestionsFeature,
         kRemoteSuggestionsFeatureVariations,
         ntp_snippets::kArticleSuggestionsFeature.name)},
    {"enable-ntp-asset-download-suggestions",
     flag_descriptions::kEnableNtpAssetDownloadSuggestionsName,
     flag_descriptions::kEnableNtpAssetDownloadSuggestionsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(features::kAssetDownloadSuggestionsFeature)},
    {"enable-ntp-offline-page-download-suggestions",
     flag_descriptions::kEnableNtpOfflinePageDownloadSuggestionsName,
     flag_descriptions::kEnableNtpOfflinePageDownloadSuggestionsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(features::kOfflinePageDownloadSuggestionsFeature)},
    {"enable-ntp-bookmark-suggestions",
     flag_descriptions::kEnableNtpBookmarkSuggestionsName,
     flag_descriptions::kEnableNtpBookmarkSuggestionsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(ntp_snippets::kBookmarkSuggestionsFeature)},
    {"enable-ntp-foreign-sessions-suggestions",
     flag_descriptions::kEnableNtpForeignSessionsSuggestionsName,
     flag_descriptions::kEnableNtpForeignSessionsSuggestionsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(ntp_snippets::kForeignSessionsSuggestionsFeature)},
    {"enable-ntp-suggestions-notifications",
     flag_descriptions::kEnableNtpSuggestionsNotificationsName,
     flag_descriptions::kEnableNtpSuggestionsNotificationsDescription,
     kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         ntp_snippets::kNotificationsFeature,
         kContentSuggestionsNotificationsFeatureVariations,
         ntp_snippets::kNotificationsFeature.name)},
    {"ntp-condensed-layout", flag_descriptions::kNtpCondensedLayoutName,
     flag_descriptions::kNtpCondensedLayoutDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kNTPCondensedLayoutFeature)},
    {"ntp-condensed-tile-layout",
     flag_descriptions::kNtpCondensedTileLayoutName,
     flag_descriptions::kNtpCondensedTileLayoutDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         chrome::android::kNTPCondensedTileLayoutFeature,
         kNTPCondensedTileLayoutFeatureVariations,
         chrome::android::kNTPCondensedTileLayoutFeature.name)},
    {"ntp-modern-layout", flag_descriptions::kNtpModernLayoutName,
     flag_descriptions::kNtpModernLayoutDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kNTPModernLayoutFeature)},
    {"enable-site-exploration-ui", flag_descriptions::kSiteExplorationUiName,
     flag_descriptions::kSiteExplorationUiDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(ntp_tiles::kSiteExplorationUiFeature)},
    {"ntp-google-g-in-omnibox", flag_descriptions::kNtpGoogleGInOmniboxName,
     flag_descriptions::kNtpGoogleGInOmniboxDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::NTPShowGoogleGInOmniboxFeature)},
#endif  // OS_ANDROID
    {"user-activation-v2", flag_descriptions::kUserActivationV2Name,
     flag_descriptions::kUserActivationV2Description, kOsAll,
     FEATURE_VALUE_TYPE(features::kUserActivationV2)},
#if BUILDFLAG(ENABLE_WEBRTC) && BUILDFLAG(RTC_USE_H264) && \
    !defined(MEDIA_DISABLE_FFMPEG)
    {"enable-webrtc-h264-with-openh264-ffmpeg",
     flag_descriptions::kWebrtcH264WithOpenh264FfmpegName,
     flag_descriptions::kWebrtcH264WithOpenh264FfmpegDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(content::kWebRtcH264WithOpenH264FFmpeg)},
#endif  // ENABLE_WEBRTC && BUILDFLAG(RTC_USE_H264) && !MEDIA_DISABLE_FFMPEG
#if defined(OS_ANDROID)
    {"offline-pages-ntp", flag_descriptions::kNtpOfflinePagesName,
     flag_descriptions::kNtpOfflinePagesDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kNTPOfflinePagesFeature)},
    {"offlining-recent-pages", flag_descriptions::kOffliningRecentPagesName,
     flag_descriptions::kOffliningRecentPagesDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(offline_pages::kOffliningRecentPagesFeature)},
    {"offline-pages-ct", flag_descriptions::kOfflinePagesCtName,
     flag_descriptions::kOfflinePagesCtDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(offline_pages::kOfflinePagesCTFeature)},
    {"offline-pages-ct-v2", flag_descriptions::kOfflinePagesCtV2Name,
     flag_descriptions::kOfflinePagesCtV2Description, kOsAndroid,
     FEATURE_VALUE_TYPE(offline_pages::kOfflinePagesCTV2Feature)},
#endif  // OS_ANDROID
    {"protect-sync-credential", flag_descriptions::kProtectSyncCredentialName,
     flag_descriptions::kProtectSyncCredentialDescription, kOsAll,
     FEATURE_VALUE_TYPE(password_manager::features::kProtectSyncCredential)},
    {"protect-sync-credential-on-reauth",
     flag_descriptions::kProtectSyncCredentialOnReauthName,
     flag_descriptions::kProtectSyncCredentialOnReauthDescription, kOsAll,
     FEATURE_VALUE_TYPE(
         password_manager::features::kProtectSyncCredentialOnReauth)},
    {"password-export", flag_descriptions::kPasswordExportName,
     flag_descriptions::kPasswordExportDescription,
     kOsWin | kOsMac | kOsCrOS | kOsLinux,
     FEATURE_VALUE_TYPE(password_manager::features::kPasswordExport)},
    {"password-import", flag_descriptions::kPasswordImportName,
     flag_descriptions::kPasswordImportDescription,
     kOsWin | kOsMac | kOsCrOS | kOsLinux,
     FEATURE_VALUE_TYPE(password_manager::features::kPasswordImport)},
#if defined(OS_CHROMEOS)
    {"enable-experimental-accessibility-features",
     flag_descriptions::kExperimentalAccessibilityFeaturesName,
     flag_descriptions::kExperimentalAccessibilityFeaturesDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(
         chromeos::switches::kEnableExperimentalAccessibilityFeatures)},
    {"opt-in-ime-menu", flag_descriptions::kEnableImeMenuName,
     flag_descriptions::kEnableImeMenuDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kOptInImeMenu)},
    {"disable-system-timezone-automatic-detection",
     flag_descriptions::kDisableSystemTimezoneAutomaticDetectionName,
     flag_descriptions::kDisableSystemTimezoneAutomaticDetectionDescription,
     kOsCrOS,
     SINGLE_VALUE_TYPE(
         chromeos::switches::kDisableSystemTimezoneAutomaticDetectionPolicy)},
    {"enable-bulk-printers", flag_descriptions::kBulkPrintersName,
     flag_descriptions::kBulkPrintersDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kBulkPrinters)},
    {"enable-cros-component", flag_descriptions::kCrOSComponentName,
     flag_descriptions::kCrOSComponentDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kCrOSComponent)},
    {"enable-cros-container", flag_descriptions::kCrOSContainerName,
     flag_descriptions::kCrOSContainerDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kCrOSContainer)},
    {"enable-encryption-migration",
     flag_descriptions::kEnableEncryptionMigrationName,
     flag_descriptions::kEnableEncryptionMigrationDescription, kOsCrOS,
     ENABLE_DISABLE_VALUE_TYPE(
         chromeos::switches::kEnableEncryptionMigration,
         chromeos::switches::kDisableEncryptionMigration)},
#endif  // OS_CHROMEOS
#if !defined(OS_ANDROID) && defined(GOOGLE_CHROME_BUILD)
    {"enable-google-branded-context-menu",
     flag_descriptions::kGoogleBrandedContextMenuName,
     flag_descriptions::kGoogleBrandedContextMenuDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(switches::kEnableGoogleBrandedContextMenu)},
#endif  // !OS_ANDROID && GOOGLE_CHROME_BUILD
#if defined(OS_MACOSX)
    {"enable-fullscreen-in-tab-detaching",
     flag_descriptions::kTabDetachingInFullscreenName,
     flag_descriptions::kTabDetachingInFullscreenDescription, kOsMac,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableFullscreenTabDetaching,
                               switches::kDisableFullscreenTabDetaching)},
    {"enable-content-fullscreen", flag_descriptions::kContentFullscreenName,
     flag_descriptions::kContentFullscreenDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kContentFullscreen)},
    {"enable-fullscreen-toolbar-reveal",
     flag_descriptions::kFullscreenToolbarRevealName,
     flag_descriptions::kFullscreenToolbarRevealDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kFullscreenToolbarReveal)},
#endif  // OS_MACOSX
    {"important-sites-in-cbd", flag_descriptions::kImportantSitesInCbdName,
     flag_descriptions::kImportantSitesInCbdDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kImportantSitesInCbd)},
    {"tabs-in-cbd", flag_descriptions::kTabsInCbdName,
     flag_descriptions::kTabsInCbdDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kTabsInCbd)},
    {"passive-listener-default",
     flag_descriptions::kPassiveEventListenerDefaultName,
     flag_descriptions::kPassiveEventListenerDefaultDescription, kOsAll,
     MULTI_VALUE_TYPE(kPassiveListenersChoices)},
    {"document-passive-event-listeners",
     flag_descriptions::kPassiveDocumentEventListenersName,
     flag_descriptions::kPassiveDocumentEventListenersDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kPassiveDocumentEventListeners)},
    {"passive-event-listeners-due-to-fling",
     flag_descriptions::kPassiveEventListenersDueToFlingName,
     flag_descriptions::kPassiveEventListenersDueToFlingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kPassiveEventListenersDueToFling)},
    {"enable-font-cache-scaling", flag_descriptions::kFontCacheScalingName,
     flag_descriptions::kFontCacheScalingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFontCacheScaling)},
    {"enable-framebusting-needs-sameorigin-or-usergesture",
     flag_descriptions::kFramebustingName,
     flag_descriptions::kFramebustingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFramebustingNeedsSameOriginOrUserGesture)},
    {"vibrate-requires-user-gesture",
     flag_descriptions::kVibrateRequiresUserGestureName,
     flag_descriptions::kVibrateRequiresUserGestureDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kVibrateRequiresUserGesture)},
    {"web-payments", flag_descriptions::kWebPaymentsName,
     flag_descriptions::kWebPaymentsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kWebPayments)},
    {"web-payments-modifiers", flag_descriptions::kWebPaymentsModifiersName,
     flag_descriptions::kWebPaymentsModifiersDescription,
     kOsAndroid | kOsDesktop,
     FEATURE_VALUE_TYPE(payments::features::kWebPaymentsModifiers)},
    {"service-worker-payment-apps",
     flag_descriptions::kServiceWorkerPaymentAppsName,
     flag_descriptions::kServiceWorkerPaymentAppsDescription,
     kOsAndroid | kOsDesktop,
     FEATURE_VALUE_TYPE(features::kServiceWorkerPaymentApps)},
#if defined(OS_ANDROID)
    {"enable-android-pay-integration-v1",
     flag_descriptions::kEnableAndroidPayIntegrationV1Name,
     flag_descriptions::kEnableAndroidPayIntegrationV1Description, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAndroidPayIntegrationV1)},
    {"enable-android-pay-integration-v2",
     flag_descriptions::kEnableAndroidPayIntegrationV2Name,
     flag_descriptions::kEnableAndroidPayIntegrationV2Description, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAndroidPayIntegrationV2)},
    {"enable-web-payments-method-section-order-v2",
     flag_descriptions::kEnableWebPaymentsMethodSectionOrderV2Name,
     flag_descriptions::kEnableWebPaymentsMethodSectionOrderV2Description,
     kOsAndroid,
     FEATURE_VALUE_TYPE(payments::features::kWebPaymentsMethodSectionOrderV2)},
    {"enable-web-payments-single-app-ui-skip",
     flag_descriptions::kEnableWebPaymentsSingleAppUiSkipName,
     flag_descriptions::kEnableWebPaymentsSingleAppUiSkipDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kWebPaymentsSingleAppUiSkip)},
    {"android-payment-apps", flag_descriptions::kAndroidPaymentAppsName,
     flag_descriptions::kAndroidPaymentAppsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAndroidPaymentApps)},
    {"pay-with-google-v1", flag_descriptions::kPayWithGoogleV1Name,
     flag_descriptions::kPayWithGoogleV1Description, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kPayWithGoogleV1)},
#endif  // OS_ANDROID
#if defined(OS_CHROMEOS)
    {"disable-eol-notification", flag_descriptions::kEolNotificationName,
     flag_descriptions::kEolNotificationDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(chromeos::switches::kDisableEolNotification)},
#endif  // OS_CHROMEOS
    {"fill-on-account-select", flag_descriptions::kFillOnAccountSelectName,
     flag_descriptions::kFillOnAccountSelectDescription, kOsAll,
     FEATURE_VALUE_TYPE(password_manager::features::kFillOnAccountSelect)},
    {"new-audio-rendering-mixing-strategy",
     flag_descriptions::kNewAudioRenderingMixingStrategyName,
     flag_descriptions::kNewAudioRenderingMixingStrategyDescription,
     kOsWin | kOsMac | kOsLinux | kOsAndroid,
     FEATURE_VALUE_TYPE(media::kNewAudioRenderingMixingStrategy)},
    {"disable-background-video-track",
     flag_descriptions::kBackgroundVideoTrackOptimizationName,
     flag_descriptions::kBackgroundVideoTrackOptimizationDescription, kOsAll,
     FEATURE_VALUE_TYPE(media::kBackgroundVideoTrackOptimization)},
    {"enable-new-remote-playback-pipeline",
     flag_descriptions::kNewRemotePlaybackPipelineName,
     flag_descriptions::kNewRemotePlaybackPipelineDescription, kOsAll,
     FEATURE_VALUE_TYPE(media::kNewRemotePlaybackPipeline)},
    {"enable-surfaces-for-videos",
     flag_descriptions::kUseSurfaceLayerForVideoName,
     flag_descriptions::kUseSurfaceLayerForVideoDescription, kOsAll,
     FEATURE_VALUE_TYPE(media::kUseSurfaceLayerForVideo)},
#if defined(OS_CHROMEOS)
    {"quick-unlock-pin", flag_descriptions::kQuickUnlockPinName,
     flag_descriptions::kQuickUnlockPinDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kQuickUnlockPin)},
    {"quick-unlock-fingerprint", flag_descriptions::kQuickUnlockFingerprint,
     flag_descriptions::kQuickUnlockFingerprintDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kQuickUnlockFingerprint)},
#endif  // OS_CHROMEOS
    {"browser-task-scheduler", flag_descriptions::kBrowserTaskSchedulerName,
     flag_descriptions::kBrowserTaskSchedulerDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableBrowserTaskScheduler,
                               switches::kDisableBrowserTaskScheduler)},
#if defined(OS_ANDROID)
    {"no-credit-card-abort", flag_descriptions::kNoCreditCardAbort,
     flag_descriptions::kNoCreditCardAbortDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kNoCreditCardAbort)},
#endif  // OS_ANDROID
    {"enable-feature-policy", flag_descriptions::kFeaturePolicyName,
     flag_descriptions::kFeaturePolicyDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFeaturePolicy)},
#if defined(OS_CHROMEOS)
    {"enable-emoji-handwriting-voice-on-ime-menu",
     flag_descriptions::kEnableEhvInputName,
     flag_descriptions::kEnableEhvInputDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kEHVInputOnImeMenu)},
#endif  // OS_CHROMEOS
#if defined(OS_CHROMEOS)
    {"arc-boot-completed-broadcast", flag_descriptions::kArcBootCompleted,
     flag_descriptions::kArcBootCompletedDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kBootCompletedBroadcastFeature)},
    {"arc-native-bridge-experiment",
     flag_descriptions::kArcNativeBridgeExperimentName,
     flag_descriptions::kArcNativeBridgeExperimentDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kNativeBridgeExperimentFeature)},
    {"arc-vpn", flag_descriptions::kArcVpnName,
     flag_descriptions::kArcVpnDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kVpnFeature)},
#endif  // OS_CHROMEOS
    {"enable-generic-sensor", flag_descriptions::kEnableGenericSensorName,
     flag_descriptions::kEnableGenericSensorDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kGenericSensor)},
    {"enable-generic-sensor-extra-classes",
     flag_descriptions::kEnableGenericSensorExtraClassesName,
     flag_descriptions::kEnableGenericSensorExtraClassesDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kGenericSensorExtraClasses)},
    {"expensive-background-timer-throttling",
     flag_descriptions::kExpensiveBackgroundTimerThrottlingName,
     flag_descriptions::kExpensiveBackgroundTimerThrottlingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kExpensiveBackgroundTimerThrottling)},
#if defined(OS_CHROMEOS)
    {"enumerate-audio-devices",
     flag_descriptions::kEnableEnumeratingAudioDevicesName,
     flag_descriptions::kEnableEnumeratingAudioDevicesDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kEnumerateAudioDevices)},
#endif  // OS_CHROMEOS
#if !defined(OS_ANDROID)
    {"enable-audio-focus", flag_descriptions::kEnableAudioFocusName,
     flag_descriptions::kEnableAudioFocusDescription, kOsDesktop,
     MULTI_VALUE_TYPE(kEnableAudioFocusChoices)},
#endif  // !OS_ANDROID
#if defined(OS_ANDROID)
    {"modal-permission-prompts", flag_descriptions::kModalPermissionPromptsName,
     flag_descriptions::kModalPermissionPromptsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kModalPermissionPrompts)},
#endif
#if BUILDFLAG(ENABLE_PRINT_PREVIEW)
    {"enable-new-print-preview", flag_descriptions::kEnableNewPrintPreview,
     flag_descriptions::kEnableNewPrintPreviewDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kNewPrintPreview)},
#endif
#if BUILDFLAG(ENABLE_PRINT_PREVIEW) && !defined(OS_WIN) && !defined(OS_MACOSX)
    {"print-pdf-as-image", flag_descriptions::kPrintPdfAsImageName,
     flag_descriptions::kPrintPdfAsImageDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kPrintPdfAsImage)},
#endif
#if defined(OS_ANDROID)
    {"concurrent-background-loading-on-svelte",
     flag_descriptions::kOfflinePagesSvelteConcurrentLoadingName,
     flag_descriptions::kOfflinePagesSvelteConcurrentLoadingDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         offline_pages::kOfflinePagesSvelteConcurrentLoadingFeature)},
#endif  // !defined(OS_ANDROID)
    {"cross-process-guests",
     flag_descriptions::kCrossProcessGuestViewIsolationName,
     flag_descriptions::kCrossProcessGuestViewIsolationDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kGuestViewCrossProcessFrames)},
#if !defined(OS_ANDROID)
    {"media-remoting", flag_descriptions::kMediaRemotingName,
     flag_descriptions::kMediaRemotingDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kMediaRemoting)},
#endif
#if defined(OS_ANDROID)
    {"video-fullscreen-orientation-lock",
     flag_descriptions::kVideoFullscreenOrientationLockName,
     flag_descriptions::kVideoFullscreenOrientationLockDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(media::kVideoFullscreenOrientationLock)},
    {"video-rotate-to-fullscreen",
     flag_descriptions::kVideoRotateToFullscreenName,
     flag_descriptions::kVideoRotateToFullscreenDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(media::kVideoRotateToFullscreen)},
#endif
    {"enable-nostate-prefetch", flag_descriptions::kNostatePrefetchName,
     flag_descriptions::kNostatePrefetchDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(prerender::kNoStatePrefetchFeature,
                                    kNoStatePrefetchFeatureVariations,
                                    "NoStatePrefetchValidation")},
#if defined(OS_CHROMEOS)
    {"enable-android-wallpapers-app",
     flag_descriptions::kEnableAndroidWallpapersAppName,
     flag_descriptions::kEnableAndroidWallpapersAppDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(chromeos::switches::kEnableAndroidWallpapersApp)},
#endif  // defined(OS_CHROMEOS)

#if defined(OS_ANDROID)
    {"enable-expanded-autofill-credit-card-popup",
     flag_descriptions::kEnableExpandedAutofillCreditCardPopupLayoutName,
     flag_descriptions::kEnableExpandedAutofillCreditCardPopupLayoutDescription,
     kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         autofill::kAutofillCreditCardPopupLayout,
         kAutofillCreditCardPopupLayoutFeatureVariations,
         // Must be AutofillCreditCardDropdownVariations to prevent DCHECK crash
         // when the flag is manually enabled in a local build.
         "AutofillCreditCardDropdownVariations")},
#endif  // OS_ANDROID

#if defined(OS_CHROMEOS)
    {ui_devtools::kEnableUiDevTools, flag_descriptions::kUiDevToolsName,
     flag_descriptions::kUiDevToolsDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ui_devtools::kEnableUiDevTools)},
#endif  // defined(OS_CHROMEOS)

    {"enable-autofill-credit-card-last-used-date-display",
     flag_descriptions::kEnableAutofillCreditCardLastUsedDateDisplayName,
     flag_descriptions::kEnableAutofillCreditCardLastUsedDateDisplayDescription,
     kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         autofill::kAutofillCreditCardLastUsedDateDisplay,
         kAutofillCreditCardLastUsedDateFeatureVariations,
         // Must be AutofillCreditCardDropdownVariations to prevent DCHECK crash
         // when the flag is manually enabled in a local build.
         "AutofillCreditCardDropdownVariations")},
    {"enable-autofill-credit-card-upload-cvc-prompt",
     flag_descriptions::kEnableAutofillCreditCardUploadCvcPromptName,
     flag_descriptions::kEnableAutofillCreditCardUploadCvcPromptDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(autofill::kAutofillUpstreamRequestCvcIfMissing)},
    {"enable-autofill-credit-card-upload-google-logo",
     flag_descriptions::kEnableAutofillCreditCardUploadGoogleLogoName,
     flag_descriptions::kEnableAutofillCreditCardUploadGoogleLogoDescription,
     kOsDesktop, FEATURE_VALUE_TYPE(autofill::kAutofillUpstreamShowGoogleLogo)},
    {"enable-autofill-credit-card-upload-new-ui",
     flag_descriptions::kEnableAutofillCreditCardUploadNewUiName,
     flag_descriptions::kEnableAutofillCreditCardUploadNewUiDescription,
     kOsDesktop, FEATURE_VALUE_TYPE(autofill::kAutofillUpstreamShowNewUi)},
    {"enable-autofill-credit-card-ablation-experiment",
     flag_descriptions::kEnableAutofillCreditCardAblationExperimentDisplayName,
     flag_descriptions::kEnableAutofillCreditCardAblationExperimentDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(autofill::kAutofillCreditCardAblationExperiment)},
    {"enable-autofill-credit-card-bank-name-display",
     flag_descriptions::kEnableAutofillCreditCardBankNameDisplayName,
     flag_descriptions::kEnableAutofillCreditCardBankNameDisplayDescription,
     kOsAll, FEATURE_VALUE_TYPE(autofill::kAutofillCreditCardBankNameDisplay)},
    {"enable-autofill-send-billing-customer-number",
     flag_descriptions::kEnableAutofillSendBillingCustomerNumberName,
     flag_descriptions::kEnableAutofillSendBillingCustomerNumberDescription,
     kOsAll, FEATURE_VALUE_TYPE(autofill::kAutofillSendBillingCustomerNumber)},
    {"enable-macviews-credit-card-dialogs",
     flag_descriptions::kEnableAutofillToolkitViewsCreditCardDialogsMac,
     flag_descriptions::
         kEnableAutofillToolkitViewsCreditCardDialogsMacDescription,
     kOsMac,
     FEATURE_VALUE_TYPE(autofill::kAutofillToolkitViewsCreditCardDialogsMac)},

#if defined(OS_WIN)
    {"windows10-custom-titlebar",
     flag_descriptions::kWindows10CustomTitlebarName,
     flag_descriptions::kWindows10CustomTitlebarDescription, kOsWin,
     SINGLE_VALUE_TYPE(switches::kWindows10CustomTitlebar)},
#endif  // OS_WIN

#if defined(OS_ANDROID)
    {"lsd-permission-prompt", flag_descriptions::kLsdPermissionPromptName,
     flag_descriptions::kLsdPermissionPromptDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kLsdPermissionPrompt)},
#endif

#if defined(OS_CHROMEOS)
    {"enable-touchscreen-calibration",
     flag_descriptions::kTouchscreenCalibrationName,
     flag_descriptions::kTouchscreenCalibrationDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(chromeos::switches::kEnableTouchCalibrationSetting)},
#endif  // defined(OS_CHROMEOS)
#if defined(OS_CHROMEOS)
    {"team-drives", flag_descriptions::kTeamDrivesName,
     flag_descriptions::kTeamDrivesDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(google_apis::kEnableTeamDrives)},
    {"file-manager-touch-mode", flag_descriptions::kFileManagerTouchModeName,
     flag_descriptions::kFileManagerTouchModeDescription, kOsCrOS,
     ENABLE_DISABLE_VALUE_TYPE(
         chromeos::switches::kEnableFileManagerTouchMode,
         chromeos::switches::kDisableFileManagerTouchMode)},
#endif  // OS_CHROMEOS

#if defined(OS_WIN)
    {"gdi-text-printing", flag_descriptions::kGdiTextPrinting,
     flag_descriptions::kGdiTextPrintingDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kGdiTextPrinting)},
    {"postscript-printing", flag_descriptions::kDisablePostscriptPrinting,
     flag_descriptions::kDisablePostscriptPrintingDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kDisablePostScriptPrinting)},
#endif  // defined(OS_WIN)

#if defined(OS_CHROMEOS)
    {"force-enable-stylus-tools",
     flag_descriptions::kForceEnableStylusToolsName,
     flag_descriptions::kForceEnableStylusToolsDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ash::switches::kAshForceEnableStylusTools)},
#endif  // defined(OS_CHROMEOS)

#if defined(OS_ANDROID)
    {"enable-midi-manager-dynamic-instantiation",
     flag_descriptions::kEnableMidiManagerDynamicInstantiationName,
     flag_descriptions::kEnableMidiManagerDynamicInstantiationDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(midi::features::kMidiManagerDynamicInstantiation)},
#endif  // defined(OS_ANDROID)

#if defined(OS_WIN)
    {"new-usb-backend", flag_descriptions::kNewUsbBackendName,
     flag_descriptions::kNewUsbBackendDescription, kOsWin,
     FEATURE_VALUE_TYPE(device::kNewUsbBackend)},
    {"enable-desktop-ios-promotions",
     flag_descriptions::kEnableDesktopIosPromotionsName,
     flag_descriptions::kEnableDesktopIosPromotionsDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kDesktopIOSPromotion)},
#endif  // defined(OS_WIN)

    {"enable-zero-suggest-redirect-to-chrome",
     flag_descriptions::kEnableZeroSuggestRedirectToChromeName,
     flag_descriptions::kEnableZeroSuggestRedirectToChromeDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kZeroSuggestRedirectToChrome)},
    {"left-to-right-urls", flag_descriptions::kLeftToRightUrlsName,
     flag_descriptions::kLeftToRightUrlsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kLeftToRightUrls)},

#if defined(OS_LINUX) || defined(OS_MACOSX) || defined(OS_WIN)
    {"omnibox-entity-suggestions",
     flag_descriptions::kOmniboxEntitySuggestionsName,
     flag_descriptions::kOmniboxEntitySuggestionsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kOmniboxEntitySuggestions)},
    {"omnibox-tail-suggestions", flag_descriptions::kOmniboxTailSuggestionsName,
     flag_descriptions::kOmniboxTailSuggestionsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kOmniboxTailSuggestions)},
    {"omnibox-tab-switch-suggestions",
     flag_descriptions::kOmniboxTabSwitchSuggestionsName,
     flag_descriptions::kOmniboxTabSwitchSuggestionsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kOmniboxTabSwitchSuggestions)},
    {"enable-new-app-menu-icon", flag_descriptions::kEnableNewAppMenuIconName,
     flag_descriptions::kEnableNewAppMenuIconDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kAnimatedAppMenuIcon)},
#endif  // defined(OS_LINUX) || defined(OS_MACOSX) || defined(OS_WIN)

#if defined(OS_ANDROID)
    {"enable-custom-feedback-ui",
     flag_descriptions::kEnableCustomFeedbackUiName,
     flag_descriptions::kEnableCustomFeedbackUiDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kCustomFeedbackUi)},
#endif  // OS_ANDROID

    {"enable-resource-prefetch", flag_descriptions::kSpeculativePrefetchName,
     flag_descriptions::kSpeculativePrefetchDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         predictors::kSpeculativeResourcePrefetchingFeature,
         kSpeculativeResourcePrefetchingFeatureVariations,
         "SpeculativeResourcePrefetchingValidation")},

    {"enable-off-main-thread-fetch", flag_descriptions::kOffMainThreadFetchName,
     flag_descriptions::kOffMainThreadFetchDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kOffMainThreadFetch)},

    {"enable-speculative-service-worker-start-on-query-input",
     flag_descriptions::kSpeculativeServiceWorkerStartOnQueryInputName,
     flag_descriptions::kSpeculativeServiceWorkerStartOnQueryInputDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kSpeculativeServiceWorkerStartOnQueryInput)},

#if defined(OS_MACOSX)
    {"tab-strip-keyboard-focus", flag_descriptions::kTabStripKeyboardFocusName,
     flag_descriptions::kTabStripKeyboardFocusDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kTabStripKeyboardFocus)},
#endif

#if defined(OS_CHROMEOS)
    {"enable-chromevox-arc-support",
     flag_descriptions::kEnableChromevoxArcSupportName,
     flag_descriptions::kEnableChromevoxArcSupportDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(chromeos::switches::kEnableChromeVoxArcSupport)},
#endif  // defined(OS_CHROMEOS)

    {"enable-mojo-loading", flag_descriptions::kMojoLoadingName,
     flag_descriptions::kMojoLoadingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kLoadingWithMojo)},

    {"enable-fetch-keepalive-timeout-setting",
     flag_descriptions::kFetchKeepaliveTimeoutSettingName,
     flag_descriptions::kFetchKeepaliveTimeoutSettingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFetchKeepaliveTimeoutSetting)},

    {"enable-renderer-side-resource-scheduler",
     flag_descriptions::kRendererSideResourceSchedulerName,
     flag_descriptions::kRendererSideResourceSchedulerDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kRendererSideResourceScheduler)},

#if defined(OS_CHROMEOS)
    {"force-tablet-mode", flag_descriptions::kUiModeName,
     flag_descriptions::kUiModeDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kAshUiModeChoices)},
#endif  // OS_CHROMEOS

    {"memory-ablation", flag_descriptions::kMemoryAblationName,
     flag_descriptions::kMemoryAblationDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(kMemoryAblationFeature,
                                    kMemoryAblationFeatureVariations,
                                    "MemoryAblation")},

#if defined(OS_ANDROID)
    {"enable-custom-context-menu",
     flag_descriptions::kEnableCustomContextMenuName,
     flag_descriptions::kEnableCustomContextMenuDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kCustomContextMenu)},
#endif  // OS_ANDROID

#if defined(OS_LINUX) || defined(OS_MACOSX) || defined(OS_WIN)
    {pausetabs::kFeatureName, flag_descriptions::kPauseBackgroundTabsName,
     flag_descriptions::kPauseBackgroundTabsDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(pausetabs::kFeature,
                                    kPauseBackgroundTabsVariations,
                                    "PauseBackgroundTabs")},
#endif

#if defined(OS_CHROMEOS)
    {"ash-disable-smooth-screen-rotation",
     flag_descriptions::kAshDisableSmoothScreenRotationName,
     flag_descriptions::kAshDisableSmoothScreenRotationDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(ash::switches::kAshDisableSmoothScreenRotation)},
#endif  // OS_CHROMEOS

#if defined(OS_CHROMEOS)
    {"enable-zip-archiver-packer",
     flag_descriptions::kEnableZipArchiverPackerName,
     flag_descriptions::kEnableZipArchiverPackerDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(chromeos::switches::kEnableZipArchiverPacker)},
    {"enable-zip-archiver-unpacker",
     flag_descriptions::kZipArchiverUnpackerName,
     flag_descriptions::kZipArchiverUnpackerDescription, kOsCrOS,
     ENABLE_DISABLE_VALUE_TYPE(
         chromeos::switches::kEnableZipArchiverUnpacker,
         chromeos::switches::kDisableZipArchiverUnpacker)},
#endif  // OS_CHROMEOS

#if defined(OS_ANDROID)
    {"enable-copyless-paste", flag_descriptions::kEnableCopylessPasteName,
     flag_descriptions::kEnableCopylessPasteDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kCopylessPaste)},
#endif

    {"omnibox-display-title-for-current-url",
     flag_descriptions::kOmniboxDisplayTitleForCurrentUrlName,
     flag_descriptions::kOmniboxDisplayTitleForCurrentUrlDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kDisplayTitleForCurrentUrl)},

    {"force-color-profile", flag_descriptions::kForceColorProfileName,
     flag_descriptions::kForceColorProfileDescription, kOsAll,
     MULTI_VALUE_TYPE(kForceColorProfileChoices)},

#if defined(OS_CHROMEOS)
    {"use-monitor-color-space", flag_descriptions::kUseMonitorColorSpaceName,
     flag_descriptions::kUseMonitorColorSpaceDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(switches::kUseMonitorColorSpace)},

    {"quick-unlock-pin-signin", flag_descriptions::kQuickUnlockPinSignin,
     flag_descriptions::kQuickUnlockPinSigninDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kQuickUnlockPinSignin)},
#endif  // OS_CHROMEOS

#if defined(OS_ANDROID)
    {"enable-webnfc", flag_descriptions::kEnableWebNfcName,
     flag_descriptions::kEnableWebNfcDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kWebNfc)},
#endif

#if defined(OS_ANDROID)
    {"enable-clipboard-provider",
     flag_descriptions::kEnableOmniboxClipboardProviderName,
     flag_descriptions::kEnableOmniboxClipboardProviderDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(omnibox::kEnableClipboardProvider)},
#endif

    {"autoplay-policy", flag_descriptions::kAutoplayPolicyName,
     flag_descriptions::kAutoplayPolicyDescription, kOsAll,
     MULTI_VALUE_TYPE(kAutoplayPolicyChoices)},

    {"force-effective-connection-type",
     flag_descriptions::kForceEffectiveConnectionTypeName,
     flag_descriptions::kForceEffectiveConnectionTypeDescription, kOsAll,
     MULTI_VALUE_TYPE(kForceEffectiveConnectionTypeChoices)},

    {"enable-heap-profiling", flag_descriptions::kEnableHeapProfilingName,
     flag_descriptions::kEnableHeapProfilingDescription, kOsAll,
     MULTI_VALUE_TYPE(kEnableHeapProfilingChoices)},

    {"memlog", flag_descriptions::kEnableOutOfProcessHeapProfilingName,
     flag_descriptions::kEnableOutOfProcessHeapProfilingDescription, kOsAll,
     MULTI_VALUE_TYPE(kEnableOutOfProcessHeapProfilingChoices)},

    {"omnibox-ui-elide-suggestion-url-after-host",
     flag_descriptions::kOmniboxUIElideSuggestionUrlAfterHostName,
     flag_descriptions::kOmniboxUIElideSuggestionUrlAfterHostDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kUIExperimentElideSuggestionUrlAfterHost)},

    {"omnibox-ui-hide-suggestion-url-scheme",
     flag_descriptions::kOmniboxUIHideSuggestionUrlSchemeName,
     flag_descriptions::kOmniboxUIHideSuggestionUrlSchemeDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kUIExperimentHideSuggestionUrlScheme)},

    {"omnibox-ui-hide-suggestion-url-trivial-subdomains",
     flag_descriptions::kOmniboxUIHideSuggestionUrlTrivialSubdomainsName,
     flag_descriptions::kOmniboxUIHideSuggestionUrlTrivialSubdomainsDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         omnibox::kUIExperimentHideSuggestionUrlTrivialSubdomains)},

    {"omnibox-ui-show-suggestion-favicons",
     flag_descriptions::kOmniboxUIShowSuggestionFaviconsName,
     flag_descriptions::kOmniboxUIShowSuggestionFaviconsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kUIExperimentShowSuggestionFavicons)},

    {"omnibox-ui-max-autocomplete-matches",
     flag_descriptions::kOmniboxUIMaxAutocompleteMatchesName,
     flag_descriptions::kOmniboxUIMaxAutocompleteMatchesDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         omnibox::kUIExperimentMaxAutocompleteMatches,
         kOmniboxUIMaxAutocompleteMatchesVariations,
         "OmniboxUIMaxAutocompleteVariations")},

    {"omnibox-ui-vertical-layout",
     flag_descriptions::kOmniboxUIVerticalLayoutName,
     flag_descriptions::kOmniboxUIVerticalLayoutDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kUIExperimentVerticalLayout)},

    {"omnibox-ui-narrow-dropdown",
     flag_descriptions::kOmniboxUINarrowDropdownName,
     flag_descriptions::kOmniboxUINarrowDropdownDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kUIExperimentNarrowDropdown)},

    {"omnibox-ui-vertical-margin",
     flag_descriptions::kOmniboxUIVerticalMarginName,
     flag_descriptions::kOmniboxUIVerticalMarginDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kUIExperimentVerticalMargin,
                                    kOmniboxUIVerticalMarginVariations,
                                    "OmniboxUIVerticalMarginVariations")},

    {"omnibox-ui-swap-title-and-url",
     flag_descriptions::kOmniboxUISwapTitleAndUrlName,
     flag_descriptions::kOmniboxUISwapTitleAndUrlDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kUIExperimentSwapTitleAndUrl)},

    {"use-suggestions-even-if-few",
     flag_descriptions::kUseSuggestionsEvenIfFewFeatureName,
     flag_descriptions::kUseSuggestionsEvenIfFewFeatureDescription, kOsAll,
     FEATURE_VALUE_TYPE(suggestions::kUseSuggestionsEvenIfFewFeature)},

    {"use-new-accept-language-header",
     flag_descriptions::kUseNewAcceptLanguageHeaderName,
     flag_descriptions::kUseNewAcceptLanguageHeaderDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kUseNewAcceptLanguageHeader)},

#if defined(OS_WIN)
    {"enable-d3d-vsync", flag_descriptions::kEnableD3DVsync,
     flag_descriptions::kEnableD3DVsyncDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kD3DVsync)},
#endif  // defined(OS_WIN)

#if !defined(OS_ANDROID)
    {"use-google-local-ntp", flag_descriptions::kUseGoogleLocalNtpName,
     flag_descriptions::kUseGoogleLocalNtpDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kUseGoogleLocalNtp)},

    {"one-google-bar-on-local-ntp",
     flag_descriptions::kOneGoogleBarOnLocalNtpName,
     flag_descriptions::kOneGoogleBarOnLocalNtpDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kOneGoogleBarOnLocalNtp)},
#endif  // !defined(OS_ANDROID)

#if defined(OS_MACOSX)
    {"mac-rtl", flag_descriptions::kMacRTLName,
     flag_descriptions::kMacRTLDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kMacRTL)},
#endif  // defined(OS_MACOSX)

#if defined(OS_CHROMEOS)
    {"enable-floating-virtual-keyboard",
     flag_descriptions::kEnableFloatingVirtualKeyboardName,
     flag_descriptions::kEnableFloatingVirtualKeyboardDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kEnableFloatingVirtualKeyboard)},
    {"enable-per-user-timezone", flag_descriptions::kEnablePerUserTimezoneName,
     flag_descriptions::kEnablePerUserTimezoneDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(chromeos::switches::kDisablePerUserTimezone)},
#endif  // OS_CHROMEOS

#if !defined(OS_ANDROID)
    {"enable-picture-in-picture",
     flag_descriptions::kEnablePictureInPictureName,
     flag_descriptions::kEnablePictureInPictureDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(switches::kEnablePictureInPicture)},
#endif  // !defined(OS_ANDROID)
    {"navigation-mojo-response", flag_descriptions::kNavigationMojoResponseName,
     flag_descriptions::kNavigationMojoResponseDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kNavigationMojoResponse)},

#if defined(OS_MACOSX)
    {"mac-touchbar", flag_descriptions::kMacTouchBarName,
     flag_descriptions::kMacTouchBarDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kBrowserTouchBar)},
    {"dialog-touchbar", flag_descriptions::kDialogTouchBarName,
     flag_descriptions::kDialogTouchBarDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kDialogTouchBar)},
    {"credit-card-autofill-touchbar",
     flag_descriptions::kCreditCardAutofillTouchBarName,
     flag_descriptions::kCreditCardAutofillTouchBarDescription, kOsMac,
     FEATURE_VALUE_TYPE(autofill::kCreditCardAutofillTouchBar)},
#endif  // defined(OS_MACOSX)

#if defined(TOOLKIT_VIEWS)
    {"enable-experimental-fullscreen-exit-ui",
     flag_descriptions::kExperimentalFullscreenExitUIName,
     flag_descriptions::kExperimentalFullscreenExitUIDescription,
     kOsWin | kOsLinux | kOsCrOS,
     SINGLE_VALUE_TYPE(switches::kEnableExperimentalFullscreenExitUI)},
#endif  // defined(TOOLKIT_VIEWS)

    {"network-service", flag_descriptions::kEnableNetworkServiceName,
     flag_descriptions::kEnableNetworkServiceDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kNetworkService)},

    {"network-service-in-process",
     flag_descriptions::kEnableNetworkServiceInProcessName,
     flag_descriptions::kEnableNetworkServiceInProcessDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kNetworkServiceInProcess)},

    {"out-of-blink-cors", flag_descriptions::kEnableOutOfBlinkCORSName,
     flag_descriptions::kEnableOutOfBlinkCORSDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kOutOfBlinkCORS)},

    {"keep-alive-renderer-for-keepalive-requests",
     flag_descriptions::kKeepAliveRendererForKeepaliveRequestsName,
     flag_descriptions::kKeepAliveRendererForKeepaliveRequestsDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(features::kKeepAliveRendererForKeepaliveRequests)},

    {"use-ddljson-api", flag_descriptions::kUseDdljsonApiName,
     flag_descriptions::kUseDdljsonApiDescription, kOsAll,
     MULTI_VALUE_TYPE(kUseDdljsonApiChoices)},

#if defined(OS_ANDROID)
    {"spannable-inline-autocomplete",
     flag_descriptions::kSpannableInlineAutocompleteName,
     flag_descriptions::kSpannableInlineAutocompleteDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kSpannableInlineAutocomplete)},
#endif  // defined(OS_ANDROID)

    {"enable-resource-load-scheduler",
     flag_descriptions::kResourceLoadSchedulerName,
     flag_descriptions::kResourceLoadSchedulerDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kResourceLoadScheduler)},

#if defined(OS_ANDROID)
    {"omnibox-spare-renderer", flag_descriptions::kOmniboxSpareRendererName,
     flag_descriptions::kOmniboxSpareRendererDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kOmniboxSpareRenderer)},
#endif

    {"enable-async-image-decoding", flag_descriptions::kAsyncImageDecodingName,
     flag_descriptions::kAsyncImageDecodingDescription, kOsAll,
     MULTI_VALUE_TYPE(kAsyncImageDecodingChoices)},

    {"capture-thumbnail-on-navigating-away",
     flag_descriptions::kCaptureThumbnailOnNavigatingAwayName,
     flag_descriptions::kCaptureThumbnailOnNavigatingAwayDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(features::kCaptureThumbnailOnNavigatingAway)},

#if defined(OS_CHROMEOS)
    {"disable-lock-screen-apps", flag_descriptions::kDisableLockScreenAppsName,
     flag_descriptions::kDisableLockScreenAppsDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(chromeos::switches::kDisableLockScreenApps)},
#endif  // defined(OS_CHROMEOS)

#if defined(OS_ANDROID)
    {"dont-prefetch-libraries", flag_descriptions::kDontPrefetchLibrariesName,
     flag_descriptions::kDontPrefetchLibrariesDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kDontPrefetchLibraries)},
#endif  // defined(OS_ANDROID)

#if defined(OS_ANDROID)
    {"enable-reader-mode-in-cct", flag_descriptions::kReaderModeInCCTName,
     flag_descriptions::kReaderModeInCCTDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kReaderModeInCCT)},
#endif  // !defined(OS_ANDROID)

#if defined(OS_ANDROID)
    {"enable-android-signin-promos",
     flag_descriptions::kAndroidSigninPromosName,
     flag_descriptions::kAndroidSigninPromosDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAndroidSigninPromos)},
#endif  // OS_ANDROID

#if defined(OS_ANDROID)
    {"pwa-improved-splash-screen",
     flag_descriptions::kPwaImprovedSplashScreenName,
     flag_descriptions::kPwaImprovedSplashScreenDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kPwaImprovedSplashScreen)},
#endif  // OS_ANDROID

#if defined(OS_ANDROID)
    {"pwa-persistent-notification",
     flag_descriptions::kPwaPersistentNotificationName,
     flag_descriptions::kPwaPersistentNotificationDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kPwaPersistentNotification)},
#endif  // OS_ANDROID
#if defined(OS_ANDROID)
    {"view-passwords", flag_descriptions::kAndroidViewPasswordsName,
     flag_descriptions::kAndroidViewPasswordsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(password_manager::features::kViewPasswords)},
#endif  // OS_ANDROID

    {"enable-manual-fallbacks-filling",
     flag_descriptions::kEnableManualFallbacksFillingName,
     flag_descriptions::kEnableManualFallbacksFillingDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(
         password_manager::features::kEnableManualFallbacksFilling)},

#if !defined(OS_ANDROID)
    {"voice-search-on-local-ntp", flag_descriptions::kVoiceSearchOnLocalNtpName,
     flag_descriptions::kVoiceSearchOnLocalNtpDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kVoiceSearchOnLocalNtp)},
#endif  // !defined(OS_ANDROID)

    {"pwa-minimal-ui", flag_descriptions::kPwaMinimalUiName,
     flag_descriptions::kPwaMinimalUiDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kPwaMinimalUi)},

    {"click-to-open-pdf", flag_descriptions::kClickToOpenPDFName,
     flag_descriptions::kClickToOpenPDFDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kClickToOpenPDFPlaceholder)},

#if defined(OS_WIN)
    {"direct-manipulation-stylus",
     flag_descriptions::kDirectManipulationStylusName,
     flag_descriptions::kDirectManipulationStylusDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kDirectManipulationStylus)},
#endif  // defined(OS_WIN)

    {"enable-manual-password-saving",
     flag_descriptions::kManualPasswordSavingName,
     flag_descriptions::kManualPasswordSavingDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(password_manager::features::kEnableManualSaving)},

#if !defined(OS_ANDROID)
    {"remove-deprecared-gaia-signin-endpoint",
     flag_descriptions::kRemoveUsageOfDeprecatedGaiaSigninEndpointName,
     flag_descriptions::kRemoveUsageOfDeprecatedGaiaSigninEndpointDescription,
     kOsWin | kOsMac | kOsLinux,
     FEATURE_VALUE_TYPE(features::kRemoveUsageOfDeprecatedGaiaSigninEndpoint)},
#endif

#if defined(OS_ANDROID)
    {"third-party-doodles", flag_descriptions::kThirdPartyDoodlesName,
     flag_descriptions::kThirdPartyDoodlesDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         search_provider_logos::features::kThirdPartyDoodles,
         kThirdPartyDoodlesVariations,
         "NTPThirdPartyDoodles")},
#endif  // defined(OS_ANDROID)

#if !defined(OS_ANDROID)
    {"doodles-on-local-ntp", flag_descriptions::kDoodlesOnLocalNtpName,
     flag_descriptions::kDoodlesOnLocalNtpDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kDoodlesOnLocalNtp)},
#endif  // !defined(OS_ANDROID)

    {"sound-content-setting", flag_descriptions::kSoundContentSettingName,
     flag_descriptions::kSoundContentSettingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kSoundContentSetting)},

#if DCHECK_IS_ON() && defined(SYZYASAN)
    {"dcheck-is-fatal", flag_descriptions::kSyzyAsanDcheckIsFatalName,
     flag_descriptions::kSyzyAsanDcheckIsFatalDescription, kOsWin,
     FEATURE_VALUE_TYPE(base::kSyzyAsanDCheckIsFatalFeature)},
#endif  // DCHECK_IS_ON() && defined(SYZYASAN)

#if defined(OS_CHROMEOS)
    {"sys-internals", flag_descriptions::kSysInternalsName,
     flag_descriptions::kSysInternalsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kSysInternals)},
#endif  // defined(OS_CHROMEOS)

    {"enable-module-scripts-dynamic-import",
     flag_descriptions::kModuleScriptsDynamicImportName,
     flag_descriptions::kModuleScriptsDynamicImportDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kModuleScriptsDynamicImport)},

    {"enable-v8-context-snapshot", flag_descriptions::kV8ContextSnapshotName,
     flag_descriptions::kV8ContextSnapshotDescription,
     kOsDesktop,  // TODO(peria): Add Android support.
     FEATURE_VALUE_TYPE(features::kV8ContextSnapshot)},

#if defined(OS_CHROMEOS)
    {"enable-pixel-canvas-recording",
     flag_descriptions::kEnablePixelCanvasRecordingName,
     flag_descriptions::kEnablePixelCanvasRecordingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kEnablePixelCanvasRecording)},

    {"disable-tablet-autohide-titlebars",
     flag_descriptions::kDisableTabletAutohideTitlebarsName,
     flag_descriptions::kDisableTabletAutohideTitlebarsDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(
         ash::switches::kAshDisableTabletAutohideTitlebars)},

    {"enable-tablet-splitview", flag_descriptions::kEnableTabletSplitViewName,
     flag_descriptions::kEnableTabletSplitViewDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ash::switches::kAshEnableTabletSplitView)},
#endif  // defined(OS_CHROMEOS)

    {"enable-parallel-downloading", flag_descriptions::kParallelDownloadingName,
     flag_descriptions::kParallelDownloadingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kParallelDownloading)},

    {"enable-password-selection", flag_descriptions::kPasswordSelectionName,
     flag_descriptions::kPasswordSelectionDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(password_manager::features::kEnablePasswordSelection)},

    {"enable-html-base-username-detector",
     flag_descriptions::kHtmlBasedUsernameDetectorName,
     flag_descriptions::kHtmlBasedUsernameDetectorDescription, kOsAll,
     FEATURE_VALUE_TYPE(
         password_manager::features::kEnableHtmlBasedUsernameDetector)},
#if defined(OS_MACOSX)
    {"mac-system-share-menu", flag_descriptions::kMacSystemShareMenuName,
     flag_descriptions::kMacSystemShareMenuDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kMacSystemShareMenu)},
#endif  // defined(OS_MACOSX)

    {"enable-new-preconnect", flag_descriptions::kSpeculativePreconnectName,
     flag_descriptions::kSpeculativePreconnectDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(predictors::kSpeculativePreconnectFeature,
                                    kSpeculativePreconnectFeatureVariations,
                                    "SpeculativePreconnectValidation")},

#if defined(OS_ANDROID)
    {"enable-async-dns", flag_descriptions::kAsyncDnsName,
     flag_descriptions::kAsyncDnsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kAsyncDns)},
#endif  // defined(OS_ANDROID)

    {"enable-overflow-icons-for-media-controls",
     flag_descriptions::kOverflowIconsForMediaControlsName,
     flag_descriptions::kOverflowIconsForMediaControlsDescription, kOsAll,
     FEATURE_VALUE_TYPE(media::kOverflowIconsForMediaControls)},

#if defined(OS_ANDROID)
    {"allow-reader-for-accessibility",
     flag_descriptions::kAllowReaderForAccessibilityName,
     flag_descriptions::kAllowReaderForAccessibilityDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAllowReaderForAccessibility)},
#endif  // defined(OS_ANDROID)

#if defined(OS_ANDROID)
    {"enable-downloads-foreground", flag_descriptions::kDownloadsForegroundName,
     flag_descriptions::kDownloadsForegroundDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kDownloadsForeground)},
#endif  // defined(OS_ANDROID)

#if defined(OS_ANDROID)
    // TODO(csharrison): Make this available on all platforms when the desktop
    // UI is finished.
    {"enable-block-tab-unders", flag_descriptions::kBlockTabUndersName,
     flag_descriptions::kBlockTabUndersDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(TabUnderNavigationThrottle::kBlockTabUnders)},
#endif  // defined(OS_ANDROID)

    {"top-sites-from-site-engagement",
     flag_descriptions::kTopSitesFromSiteEngagementName,
     flag_descriptions::kTopSitesFromSiteEngagementDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kTopSitesFromSiteEngagement)},

#if defined(OS_POSIX)
    {"enable-ntlm-v2", flag_descriptions::kNtlmV2EnabledName,
     flag_descriptions::kNtlmV2EnabledDescription,
     kOsMac | kOsLinux | kOsCrOS | kOsAndroid,
     FEATURE_VALUE_TYPE(features::kNtlmV2Enabled)},
#endif  // defined(OS_POSIX)

    {"enable-module-scripts-import-meta-url",
     flag_descriptions::kModuleScriptsImportMetaUrlName,
     flag_descriptions::kModuleScriptsImportMetaUrlDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kModuleScriptsImportMetaUrl)},

    {"stop-loading-in-background",
     flag_descriptions::kStopLoadingInBackgroundName,
     flag_descriptions::kStopLoadingInBackgroundDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kStopLoadingInBackground)},

#if defined(TOOLKIT_VIEWS)
    {"experimental-tab-controller",
     flag_descriptions::kExperimentalTabControllerName,
     flag_descriptions::kExperimentalTabControllerDescription,
     kOsWin | kOsLinux, FEATURE_VALUE_TYPE(kExperimentalTabControllerFeature)},
#endif  // defined(OS_WIN)

#if defined(OS_CHROMEOS)
    {"ash-enable-keyboard-shortcut-viewer",
     flag_descriptions::kAshEnableKeyboardShortcutViewerName,
     flag_descriptions::kAshEnableKeyboardShortcutViewerDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ash::switches::kAshEnableKeyboardShortcutViewer)},

    {"ash-disable-login-dim-and-blur",
     flag_descriptions::kAshDisableLoginDimAndBlurName,
     flag_descriptions::kAshDisableLoginDimAndBlurDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(ash::switches::kAshDisableLoginDimAndBlur)},
#endif  // OS_CHROMEOS

    {"clipboard-content-setting",
     flag_descriptions::kClipboardContentSettingName,
     flag_descriptions::kClipboardContentSettingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kClipboardContentSetting)},

#if defined(OS_CHROMEOS)
    {"native-smb", flag_descriptions::kNativeSmbName,
     flag_descriptions::kNativeSmbDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kNativeSmb)},
#endif  // defined(OS_CHROMEOS)

    {"enable-modern-media-controls",
     flag_descriptions::kUseModernMediaControlsName,
     flag_descriptions::kUseModernMediaControlsDescription, kOsAll,
     FEATURE_VALUE_TYPE(media::kUseModernMediaControls)},

    {"enable-network-logging-to-file",
     flag_descriptions::kEnableNetworkLoggingToFileName,
     flag_descriptions::kEnableNetworkLoggingToFileDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kLogNetLog)},

#if defined(OS_CHROMEOS)
    {"enable-multi-mirroring", flag_descriptions::kEnableMultiMirroringName,
     flag_descriptions::kEnableMultiMirroringDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(switches::kEnableMultiMirroring)},
#endif  // defined(OS_CHROMEOS)

#if BUILDFLAG(ENABLE_AV1_DECODER)
    {"enable-av1-decoder", flag_descriptions::kAv1DecoderName,
     flag_descriptions::kAv1DecoderDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(media::kAv1Decoder)},
#endif  // ENABLE_AV1_DECODER

    {"enable-wheel-scroll-latching",
     flag_descriptions::kEnableTouchpadAndWheelScrollLatchingName,
     flag_descriptions::kEnableTouchpadAndWheelScrollLatchingDescription,
     kOsAll, FEATURE_VALUE_TYPE(features::kTouchpadAndWheelScrollLatching)},

    // NOTE: Adding a new flag requires adding a corresponding entry to enum
    // "LoginCustomFlags" in tools/metrics/histograms/enums.xml. See "Flag
    // Histograms" in tools/metrics/histograms/README.md (run the
    // AboutFlagsHistogramTest unit test to verify this process).
};

class FlagsStateSingleton {
 public:
  FlagsStateSingleton()
      : flags_state_(kFeatureEntries, arraysize(kFeatureEntries)) {}
  ~FlagsStateSingleton() {}

  static FlagsStateSingleton* GetInstance() {
    return base::Singleton<FlagsStateSingleton>::get();
  }

  static flags_ui::FlagsState* GetFlagsState() {
    return &GetInstance()->flags_state_;
  }

 private:
  flags_ui::FlagsState flags_state_;

  DISALLOW_COPY_AND_ASSIGN(FlagsStateSingleton);
};

bool SkipConditionalFeatureEntry(const FeatureEntry& entry) {
  version_info::Channel channel = chrome::GetChannel();
#if defined(OS_CHROMEOS)
  // Don't expose --mash/--mus outside of Canary or developer builds.
  if (!strcmp("mus", entry.internal_name) &&
      channel != version_info::Channel::DEV &&
      channel != version_info::Channel::UNKNOWN) {
    return true;
  }

  // enable-ui-devtools is only available on for non Stable channels.
  if (!strcmp(ui_devtools::kEnableUiDevTools, entry.internal_name) &&
      channel == version_info::Channel::STABLE) {
    return true;
  }
#endif  // defined(OS_CHROMEOS)

  // data-reduction-proxy-lo-fi and enable-data-reduction-proxy-lite-page
  // are only available for Chromium builds and the Canary/Dev/Beta channels.
  if ((!strcmp("data-reduction-proxy-lo-fi", entry.internal_name) ||
       !strcmp("enable-data-reduction-proxy-lite-page", entry.internal_name)) &&
      channel != version_info::Channel::BETA &&
      channel != version_info::Channel::DEV &&
      channel != version_info::Channel::CANARY &&
      channel != version_info::Channel::UNKNOWN) {
    return true;
  }

#if defined(OS_WIN)
  // HDR mode works, but displays everything horribly wrong prior to windows 10.
  if (!strcmp("enable-hdr", entry.internal_name) &&
      base::win::GetVersion() < base::win::Version::VERSION_WIN10) {
    return true;
  }
#endif  // OS_WIN

  return false;
}

// Records a set of feature switches (prefixed with "--").
void ReportAboutFlagsHistogramSwitches(const std::string& uma_histogram_name,
                                       const std::set<std::string>& switches) {
  for (const std::string& flag : switches) {
    int uma_id = about_flags::testing::kBadSwitchFormatHistogramId;
    if (base::StartsWith(flag, "--", base::CompareCase::SENSITIVE)) {
      // Skip '--' before switch name.
      std::string switch_name(flag.substr(2));

      // Kill value, if any.
      const size_t value_pos = switch_name.find('=');
      if (value_pos != std::string::npos)
        switch_name.resize(value_pos);

      uma_id = GetSwitchUMAId(switch_name);
    } else {
      NOTREACHED() << "ReportAboutFlagsHistogram(): flag '" << flag
                   << "' has incorrect format.";
    }
    DVLOG(1) << "ReportAboutFlagsHistogram(): histogram='" << uma_histogram_name
             << "' '" << flag << "', uma_id=" << uma_id;

    // Sparse histogram macro does not cache the histogram, so it's safe
    // to use macro with non-static histogram name here.
    UMA_HISTOGRAM_SPARSE_SLOWLY(uma_histogram_name, uma_id);
  }
}

// Records a set of FEATURE_VALUE_TYPE features (suffixed with ":enabled" or
// "disabled", depending on their state).
void ReportAboutFlagsHistogramFeatures(const std::string& uma_histogram_name,
                                       const std::set<std::string>& features) {
  for (const std::string& feature : features) {
    int uma_id = GetSwitchUMAId(feature);
    DVLOG(1) << "ReportAboutFlagsHistogram(): histogram='" << uma_histogram_name
             << "' '" << feature << "', uma_id=" << uma_id;

    // Sparse histogram macro does not cache the histogram, so it's safe
    // to use macro with non-static histogram name here.
    UMA_HISTOGRAM_SPARSE_SLOWLY(uma_histogram_name, uma_id);
  }
}

}  // namespace

void ConvertFlagsToSwitches(flags_ui::FlagsStorage* flags_storage,
                            base::CommandLine* command_line,
                            flags_ui::SentinelsMode sentinels) {
  if (command_line->HasSwitch(switches::kNoExperiments))
    return;

  FlagsStateSingleton::GetFlagsState()->ConvertFlagsToSwitches(
      flags_storage, command_line, sentinels, switches::kEnableFeatures,
      switches::kDisableFeatures);
}

std::vector<std::string> RegisterAllFeatureVariationParameters(
    flags_ui::FlagsStorage* flags_storage,
    base::FeatureList* feature_list) {
  return FlagsStateSingleton::GetFlagsState()
      ->RegisterAllFeatureVariationParameters(flags_storage, feature_list);
}

bool AreSwitchesIdenticalToCurrentCommandLine(
    const base::CommandLine& new_cmdline,
    const base::CommandLine& active_cmdline,
    std::set<base::CommandLine::StringType>* out_difference) {
  const char* extra_flag_sentinel_begin_flag_name = nullptr;
  const char* extra_flag_sentinel_end_flag_name = nullptr;
#if defined(OS_CHROMEOS)
  // Put the flags between --policy-switches--begin and --policy-switches-end on
  // ChromeOS.
  extra_flag_sentinel_begin_flag_name =
      chromeos::switches::kPolicySwitchesBegin;
  extra_flag_sentinel_end_flag_name = chromeos::switches::kPolicySwitchesEnd;
#endif  // OS_CHROMEOS
  return flags_ui::FlagsState::AreSwitchesIdenticalToCurrentCommandLine(
      new_cmdline, active_cmdline, out_difference,
      extra_flag_sentinel_begin_flag_name, extra_flag_sentinel_end_flag_name);
}

void GetFlagFeatureEntries(flags_ui::FlagsStorage* flags_storage,
                           flags_ui::FlagAccess access,
                           base::ListValue* supported_entries,
                           base::ListValue* unsupported_entries) {
  FlagsStateSingleton::GetFlagsState()->GetFlagFeatureEntries(
      flags_storage, access, supported_entries, unsupported_entries,
      base::Bind(&SkipConditionalFeatureEntry));
}

bool IsRestartNeededToCommitChanges() {
  return FlagsStateSingleton::GetFlagsState()->IsRestartNeededToCommitChanges();
}

void SetFeatureEntryEnabled(flags_ui::FlagsStorage* flags_storage,
                            const std::string& internal_name,
                            bool enable) {
  FlagsStateSingleton::GetFlagsState()->SetFeatureEntryEnabled(
      flags_storage, internal_name, enable);
}

void RemoveFlagsSwitches(base::CommandLine::SwitchMap* switch_list) {
  FlagsStateSingleton::GetFlagsState()->RemoveFlagsSwitches(switch_list);
}

void ResetAllFlags(flags_ui::FlagsStorage* flags_storage) {
  FlagsStateSingleton::GetFlagsState()->ResetAllFlags(flags_storage);
}

void RecordUMAStatistics(flags_ui::FlagsStorage* flags_storage) {
  const std::set<std::string> switches =
      FlagsStateSingleton::GetFlagsState()->GetSwitchesFromFlags(flags_storage);
  const std::set<std::string> features =
      FlagsStateSingleton::GetFlagsState()->GetFeaturesFromFlags(flags_storage);
  ReportAboutFlagsHistogram("Launch.FlagsAtStartup", switches, features);
}

base::HistogramBase::Sample GetSwitchUMAId(const std::string& switch_name) {
  return static_cast<base::HistogramBase::Sample>(
      base::HashMetricName(switch_name));
}

void ReportAboutFlagsHistogram(const std::string& uma_histogram_name,
                               const std::set<std::string>& switches,
                               const std::set<std::string>& features) {
  ReportAboutFlagsHistogramSwitches(uma_histogram_name, switches);
  ReportAboutFlagsHistogramFeatures(uma_histogram_name, features);
}

namespace testing {

const base::HistogramBase::Sample kBadSwitchFormatHistogramId = 0;

const FeatureEntry* GetFeatureEntries(size_t* count) {
  *count = arraysize(kFeatureEntries);
  return kFeatureEntries;
}

}  // namespace testing

}  // namespace about_flags