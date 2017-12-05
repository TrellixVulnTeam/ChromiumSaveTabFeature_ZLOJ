// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

(async function() {
  TestRunner.addResult(`Tests that Styles sidebar DOM rebuilds are throttled during consecutive updates. Bug 78086.\n`);
  await TestRunner.loadModule('elements_test_runner');
  await TestRunner.showPanel('elements');
  await TestRunner.loadHTML(`
      <p>
      Tests that Styles sidebar DOM rebuilds are throttled during consecutive updates. <a href="https://bugs.webkit.org/show_bug.cgi?id=78086">Bug 78086</a>.
      </p>

      <div id="inspected"></div>
    `);

  var UPDATE_COUNT = 5;
  var rebuildCount = 0;

  ElementsTestRunner.selectNodeAndWaitForStyles('inspected', selectCallback);
  function selectCallback() {
    TestRunner.addSniffer(Elements.StylesSidebarPane.prototype, '_innerRebuildUpdate', sniffRebuild, true);
    var stylesPane = UI.panels.elements._stylesWidget;
    for (var i = 0; i < UPDATE_COUNT; ++i)
      UI.context.setFlavor(SDK.DOMNode, stylesPane.node());

    TestRunner.deprecatedRunAfterPendingDispatches(completeCallback);
  }

  function completeCallback() {
    if (rebuildCount >= UPDATE_COUNT)
      TestRunner.addResult('ERROR: got ' + rebuildCount + ' rebuilds for ' + UPDATE_COUNT + ' consecutive updates');
    else
      TestRunner.addResult('OK: rebuilds throttled');
    TestRunner.completeTest();
  }

  function sniffRebuild() {
    ++rebuildCount;
  }
})();