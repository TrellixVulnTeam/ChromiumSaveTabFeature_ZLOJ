// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_PREVIEWS_CORE_PREVIEWS_LOGGER_H_
#define COMPONENTS_PREVIEWS_CORE_PREVIEWS_LOGGER_H_

#include <list>
#include <string>
#include <unordered_map>

#include "base/macros.h"
#include "base/memory/ptr_util.h"
#include "base/observer_list.h"
#include "base/sequence_checker.h"
#include "base/time/time.h"
#include "components/previews/core/previews_black_list.h"
#include "components/previews/core/previews_experiments.h"
#include "url/gurl.h"

namespace previews {

// Get the human readable description of the log event for InfoBar messages
// based on the |type| of Previews.
std::string GetDescriptionForInfoBarDescription(previews::PreviewsType type);

class PreviewsLoggerObserver;

// Records information about previews and interventions events. The class only
// keeps the recent event logs.
class PreviewsLogger {
 public:
  // Information needed for a log message. This information will be used to
  // display log messages on chrome://interventions-internals.
  // TODO(thanhdle): Add PreviewType to this struct, and display that
  // information on the page as a separate column. crbug.com/774252.
  struct MessageLog {
    MessageLog(const std::string& event_type,
               const std::string& event_description,
               const GURL& url,
               base::Time time);

    MessageLog(const MessageLog& other);

    // The type of event associated with the log.
    const std::string event_type;

    // Human readable description of the event.
    const std::string event_description;

    // The url associated with the log.
    const GURL url;

    // The time of when the event happened.
    const base::Time time;
  };

  PreviewsLogger();
  virtual ~PreviewsLogger();

  // Add a observer to the list. This observer will be notified when new a log
  // message is added to the logger. Observers must remove themselves with
  // RemoveObserver.
  void AddAndNotifyObserver(PreviewsLoggerObserver* observer);

  // Removes a observer from the observers list. Virtualized in testing.
  virtual void RemoveObserver(PreviewsLoggerObserver* observer);

  // Add MessageLog using the given information. Pop out the oldest log if the
  // size of |log_messages_| grows larger than a threshold. Virtualized in
  // testing.
  virtual void LogMessage(const std::string& event_type,
                          const std::string& event_description,
                          const GURL& url,
                          base::Time time);

  // Convert |navigation| to a MessageLog, and add that message to
  // |log_messages_|. Virtualized in testing.
  virtual void LogPreviewNavigation(const GURL& url,
                                    PreviewsType type,
                                    bool opt_out,
                                    base::Time time);

  // Add a MessageLog for the a decision that was made about the state of
  // previews and blacklist. |passed_reasons| is an ordered list of
  // PreviewsEligibilityReasons that got pass the decision. The method takes
  // ownership of |passed_reasons|. Virtualized in testing.
  virtual void LogPreviewDecisionMade(
      PreviewsEligibilityReason reason,
      const GURL& url,
      base::Time time,
      PreviewsType type,
      std::vector<PreviewsEligibilityReason>&& passed_reasons);

  // Notify observers that |host| is blacklisted at |time|. Virtualized in
  // testing.
  virtual void OnNewBlacklistedHost(const std::string& host, base::Time time);

  // Notify observers that user blacklisted state has changed to |blacklisted|.
  // Virtualized in testing.
  virtual void OnUserBlacklistedStatusChange(bool blacklisted);

  // Notify observers that the blacklist is cleared at |time|. Virtualized in
  // testing.
  virtual void OnBlacklistCleared(base::Time time);

  // Notify observers that the status of whether blacklist decisions are ignored
  // or not. Virtualized in testing.
  virtual void OnIgnoreBlacklistDecisionStatusChanged(bool ignored);

 private:
  // Keeping track of all blacklisted host to notify new observers.
  std::unordered_map<std::string, base::Time> blacklisted_hosts_;

  // The current user blacklisted status.
  bool user_blacklisted_status_;

  // The current status of whether PreviewsBlackList decisions are ignored or
  // not.
  bool blacklist_ignored_;

  // Collection of recorded navigation log messages.
  std::list<MessageLog> navigations_logs_;

  // Collection of recorded decision log messages.
  std::list<MessageLog> decisions_logs_;

  // A list of observers listening to the logger.
  base::ObserverList<PreviewsLoggerObserver> observer_list_;

  SEQUENCE_CHECKER(sequence_checker_);

  DISALLOW_COPY_AND_ASSIGN(PreviewsLogger);
};

}  // namespace previews

#endif  // COMPONENTS_PREVIEWS_CORE_PREVIEWS_LOGGER_H_
