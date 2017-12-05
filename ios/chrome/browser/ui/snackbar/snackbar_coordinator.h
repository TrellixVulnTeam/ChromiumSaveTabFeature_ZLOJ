// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_UI_SNACKBAR_SNACKBAR_COORDINATOR_H_
#define IOS_CHROME_BROWSER_UI_SNACKBAR_SNACKBAR_COORDINATOR_H_

#import <Foundation/Foundation.h>

@class CommandDispatcher;

// Coodinator that handles commands to show snackbars.
@interface SnackbarCoordinator : NSObject

// The dispatcher used to register commands.
@property(nonatomic, weak) CommandDispatcher* dispatcher;

// Starts the coordinator.
- (void)start;

// Stops the coordinator.
- (void)stop;

@end

#endif  // IOS_CHROME_BROWSER_UI_SNACKBAR_SNACKBAR_COORDINATOR_H_