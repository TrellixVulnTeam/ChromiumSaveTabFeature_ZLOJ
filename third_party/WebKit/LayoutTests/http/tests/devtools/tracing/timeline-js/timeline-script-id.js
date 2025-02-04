// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

(async function() {
  TestRunner.addResult(
      `Test that checks location resolving mechanics for TimerInstall TimerRemove and FunctionCall events with scriptId.
       It expects two FunctionCall for InjectedScript, two TimerInstall events, two FunctionCall events and one TimerRemove event to be logged with performActions.js script name and some line number.\n`);
  await TestRunner.loadModule('performance_test_runner');
  await TestRunner.showPanel('timeline');

  function performActions() {
    var callback;
    var promise = new Promise((fulfill) => callback = fulfill);
    var timerOne = setTimeout('1 + 1', 10);
    var timerTwo = setInterval(intervalTimerWork, 20);
    var iteration = 0;
    return promise;

    function intervalTimerWork() {
      if (++iteration < 2)
        return;
      clearInterval(timerTwo);
      callback();
    }
  }

  var source = performActions.toString();
  source += '\n//# sourceURL=performActions.js';
  TestRunner.evaluateInPage(source, startTimeline);

  function startTimeline() {
    PerformanceTestRunner.invokeAsyncWithTimeline('performActions', finish);
  }

  var linkifier = new Components.Linkifier();

  var recordTypes = new Set(['TimerInstall', 'TimerRemove', 'FunctionCall']);
  function formatter(event) {
    if (!recordTypes.has(event.name))
      return;

    var detailsText = Timeline.TimelineUIUtils.buildDetailsTextForTraceEvent(
        event, PerformanceTestRunner.timelineModel().targetByEvent(event));
    TestRunner.addResult('detailsTextContent for ' + event.name + ' event: \'' + detailsText + '\'');

    var details = Timeline.TimelineUIUtils.buildDetailsNodeForTraceEvent(
        event, PerformanceTestRunner.timelineModel().targetByEvent(event), linkifier);
    if (!details)
      return;
    TestRunner.addResult(
        'details.textContent for ' + event.name + ' event: \'' + details.textContent.replace(/VM[\d]+/, 'VM') + '\'');
  }

  function finish() {
    PerformanceTestRunner.walkTimelineEventTree(formatter);
    TestRunner.completeTest();
  }
})();
