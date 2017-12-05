// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

(async function() {
  TestRunner.addResult(`Tests that the execution context is changed to match new selected node.\n`);
  await TestRunner.loadModule('elements_test_runner');
  await TestRunner.showPanel('elements');
  await TestRunner.loadHTML(`
      <iframe id="iframe-per-se" src="resources/set-outer-html-body-iframe.html" onload="runTest()"></iframe>
          <div id="element"></div>
    `);

  var mainContext;

  TestRunner.runTestSuite([
    function initialize(next) {
      ElementsTestRunner.expandElementsTree(onExpanded);

      function onExpanded() {
        mainContext = UI.context.flavor(SDK.ExecutionContext);
        dumpContextAndNext(next);
      }
    },

    function selectIframeInnerNode(next) {
      ElementsTestRunner.selectNodeWithId('head', dumpContextAndNext.bind(null, next));
    },

    function selectMainFrameNode(next) {
      ElementsTestRunner.selectNodeWithId('element', dumpContextAndNext.bind(null, next));
    },

    function selectIframeNode(next) {
      ElementsTestRunner.selectNodeWithId('iframe-per-se', dumpContextAndNext.bind(null, next));
    },

    function selectIframeImmediateChild(next) {
      var iframe = UI.context.flavor(SDK.DOMNode);
      var child = iframe.firstChild;
      ElementsTestRunner.selectNode(child).then(dumpContextAndNext.bind(null, next));
    },
  ]);

  function dumpContextAndNext(next) {
    var context = UI.context.flavor(SDK.ExecutionContext);
    var node = UI.context.flavor(SDK.DOMNode);
    var contextName = context === mainContext ? 'main' : 'iframe';
    var matchesNode = context.frameId === node.frameId();
    TestRunner.addResult('Execution Context: ' + contextName);
    TestRunner.addResult('          matches: ' + matchesNode);
    next();
  }
})();