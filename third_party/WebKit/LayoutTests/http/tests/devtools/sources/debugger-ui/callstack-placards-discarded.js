// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

(async function() {
  TestRunner.addResult(`Tests that RawSourceCode listeners count won't grow on each script pause. Bug 70996\n`);
  await TestRunner.loadModule('sources_test_runner');
  await TestRunner.showPanel('sources');
  await TestRunner.loadHTML(`
      <p>
      Tests that RawSourceCode listeners count won't grow on each script pause. <a href="https://bugs.webkit.org/show_bug.cgi?id=70996">Bug 70996</a>
      </p>
    `);
  await TestRunner.evaluateInPagePromise(`
      function testFunction()
      {
          debugger;
      }
  `);

  SourcesTestRunner.runDebuggerTestSuite([
    function testCallStackPlacardsDiscarded(next) {
      TestRunner.debuggerModel.addEventListener(SDK.DebuggerModel.Events.DebuggerPaused, didPause, this);
      var previousCount = undefined;
      function didPause(event) {
        TestRunner.addResult('Received DebuggerPaused event.');
        var callFrame = event.data.callFrames[0];
        TestRunner.addResult('Function name: ' + callFrame.functionName);
        var count = liveLocationsCount();
        if (previousCount !== undefined && count !== previousCount)
          TestRunner.addResult('FAILED: Live locations count has changed!');
        previousCount = count;
      }

      SourcesTestRunner.showScriptSource('callstack-placards-discarded.js', didShowScriptSource);
      function didShowScriptSource(sourceFrame) {
        TestRunner.addResult('Script source was shown.');
        SourcesTestRunner.runTestFunctionAndWaitUntilPaused(didPause1);
      }
      function didPause1() {
        SourcesTestRunner.resumeExecution(didResume1);
      }
      function didResume1() {
        SourcesTestRunner.runTestFunctionAndWaitUntilPaused(didPause2);
      }
      function didPause2() {
        SourcesTestRunner.resumeExecution(didResume2);
      }
      function didResume2() {
        next();
      }
    },
  ]);

  function liveLocationsCount() {
    var count = 0;
    var infos = Object.values(TestRunner.debuggerModel.scripts)
                    .map(script => script[Bindings.DebuggerWorkspaceBinding._ScriptInfoSymbol])
                    .filter(info => !!info);
    infos.forEach(function(info) {
      count += info._locations ? info._locations.size : 0;
    });
    return count;
  }
})();
