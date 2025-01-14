// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

'use strict';

function requestProcessList() {
  chrome.send('requestProcessList');
}

function saveDump() {
  chrome.send('saveDump');
}

function reportProcess(pid) {
  chrome.send('reportProcess', [pid]);
}

// celltype should either be "td" or "th". The contents of the |cols| will be
// added as children of each table cell if they are non-null.
function addListRow(table, celltype, cols) {
  let tr = document.createElement('tr');
  for (let col of cols) {
    let cell = document.createElement(celltype);
    if (col)
      cell.appendChild(col);
    tr.appendChild(cell);
  }
  table.appendChild(tr);
}

function setSaveDumpMessage(data) {
  $('save_dump_text').innerText = data;
}

function returnProcessList(data) {
  $('message').innerText = data['message'];

  let proclist = $('proclist');
  proclist.innerText = '';  // Clear existing contents.

  let processes = data['processes'];
  if (processes.length == 0)
    return;  // No processes to dump, don't make the table and refresh button.

  // Add the refresh and save-dump buttons.
  let commandsDiv = document.createElement('div');
  commandsDiv.className = 'commands';

  let refreshButton = document.createElement('button');
  refreshButton.innerText = '\u21ba Refresh process list';
  refreshButton.onclick = () => requestProcessList();
  commandsDiv.appendChild(refreshButton);
  let saveDumpButton = document.createElement('button');
  saveDumpButton.innerText = '\u21e9 Save dump';
  saveDumpButton.onclick = () => saveDump();
  commandsDiv.appendChild(saveDumpButton);
  let saveDumpText = document.createElement('div');
  saveDumpText.id = 'save_dump_text';
  commandsDiv.appendChild(saveDumpText);

  proclist.appendChild(commandsDiv);

  let table = document.createElement('table');

  // Heading.
  addListRow(table, 'th', [
    null, document.createTextNode('Process ID'), document.createTextNode('Name')
  ]);

  for (let proc of processes) {
    let procId = proc[0];

    let reportButton = document.createElement('button');
    reportButton.innerText = '\uD83D\uDC1E Report';
    reportButton.onclick = () => reportProcess(procId);

    let procIdText = document.createTextNode(procId.toString());
    let description = document.createTextNode(proc[1]);

    addListRow(table, 'td', [reportButton, procIdText, description]);
  }

  proclist.appendChild(table);
}

// Get data and have it displayed upon loading.
document.addEventListener('DOMContentLoaded', requestProcessList);

/* For manual testing.
function fakeResults() {
  returnProcessList([
    [ 11234, "Process 11234 [Browser]" ],
    [ 11235, "Process 11235 [Renderer]" ],
    [ 11236, "Process 11236 [Renderer]" ]]);
}
document.addEventListener('DOMContentLoaded', fakeResults);
*/
