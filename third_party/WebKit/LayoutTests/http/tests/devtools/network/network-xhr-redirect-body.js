// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

(async function() {
  await TestRunner.addResult(`Tests that XHR redirects preserve request body.`);
  await TestRunner.loadModule('network_test_runner');
  await TestRunner.showPanel('network');
  await TestRunner.loadHTML(`<p>Tests that XHR redirects preserve request body.</p>`);

  var offset;

  NetworkTestRunner.recordNetwork();
  offset = NetworkTestRunner.networkRequests().length;
  NetworkTestRunner.makeSimpleXHRWithPayload('POST', 'resources/redirect.cgi?status=301&ttl=1', true, 'LOST', step2);

  function step2() {
    NetworkTestRunner.networkRequests()[offset].requestContent().then(step3);
  }

  function step3() {
    NetworkTestRunner.makeSimpleXHRWithPayload(
        'POST', 'resources/redirect.cgi?status=307&ttl=1', true, 'PRESERVED', step4);
  }


  function step4() {
    NetworkTestRunner.networkRequests()[offset + 1].requestContent().then(step5);
  }
  function step5() {
    var requests = NetworkTestRunner.networkRequests();
    for (var i = 0; i < requests.length; ++i) {
      var request = requests[i];
      var requestMethod = request.requestMethod;
      var actualMethod = request.responseHeaderValue('request-method');
      var body = '[' + (request.requestFormData || '') + ']';
      TestRunner.addResult(requestMethod + ' ' + request.url());
      TestRunner.addResult('  actual http method was: ' + actualMethod);
      TestRunner.addResult('  request body: ' + body);
      TestRunner.addResult('');
    }
    TestRunner.completeTest();
  }
})();