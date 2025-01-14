// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

(async function() {
  TestRunner.addResult(`Tests the Timeline events for Animation Frame feature\n`);
  await TestRunner.loadModule('performance_test_runner');
  await TestRunner.showPanel('timeline');
  await TestRunner.evaluateInPagePromise(`
      function performActions()
      {
          var callback;
          var promise = new Promise((fulfill) => callback = fulfill);
          var requestId = window.requestAnimationFrame(animationFrameCallback);
          function animationFrameCallback()
          {
              window.cancelAnimationFrame(requestId);
              if (callback)
                  callback();
          }
          return promise;
      }
  `);

  PerformanceTestRunner.invokeAsyncWithTimeline('performActions', finish);

  function finish() {
    PerformanceTestRunner.printTimelineRecordsWithDetails('RequestAnimationFrame');
    PerformanceTestRunner.printTimelineRecordsWithDetails('FireAnimationFrame');
    PerformanceTestRunner.printTimelineRecordsWithDetails('CancelAnimationFrame');
    TestRunner.completeTest();
  }
})();
