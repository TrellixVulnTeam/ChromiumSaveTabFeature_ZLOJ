// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform/heap/Persistent.h"

#include <memory>
#include "platform/CrossThreadFunctional.h"
#include "platform/heap/Handle.h"
#include "platform/heap/HeapTestUtilities.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace blink {
namespace {

class Receiver : public GarbageCollected<Receiver> {
 public:
  void Increment(int* counter) { ++*counter; }

  void Trace(blink::Visitor* visitor) {}
};

TEST(PersistentTest, BindCancellation) {
  Receiver* receiver = new Receiver;
  int counter = 0;
  WTF::RepeatingClosure function =
      WTF::BindRepeating(&Receiver::Increment, WrapWeakPersistent(receiver),
                         WTF::Unretained(&counter));

  function.Run();
  EXPECT_EQ(1, counter);

  receiver = nullptr;
  PreciselyCollectGarbage();
  function.Run();
  EXPECT_EQ(1, counter);
}

TEST(PersistentTest, CrossThreadBindCancellation) {
  Receiver* receiver = new Receiver;
  int counter = 0;
  CrossThreadClosure function = blink::CrossThreadBind(
      &Receiver::Increment, WrapCrossThreadWeakPersistent(receiver),
      WTF::CrossThreadUnretained(&counter));

  receiver = nullptr;
  PreciselyCollectGarbage();
  std::move(function).Run();
  EXPECT_EQ(0, counter);
}

}  // namespace
}  // namespace blink
