// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/nqe/network_quality_store.h"

#include "base/strings/string_number_conversions.h"
#include "base/test/simple_test_tick_clock.h"
#include "base/time/time.h"
#include "net/base/network_change_notifier.h"
#include "net/nqe/cached_network_quality.h"
#include "net/nqe/effective_connection_type.h"
#include "net/nqe/network_id.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace net {

namespace {

TEST(NetworkQualityStoreTest, TestCaching) {
  nqe::internal::NetworkQualityStore network_quality_store;
  base::SimpleTestTickClock tick_clock;

  // Cached network quality for network with NetworkID (2G, "test1").
  const nqe::internal::CachedNetworkQuality cached_network_quality_2g_test1(
      tick_clock.NowTicks(),
      nqe::internal::NetworkQuality(base::TimeDelta::FromSeconds(1),
                                    base::TimeDelta::FromSeconds(1), 1),
      EFFECTIVE_CONNECTION_TYPE_2G);

  {
    // When ECT is UNKNOWN, then the network quality is not cached.
    nqe::internal::CachedNetworkQuality cached_network_quality_unknown(
        tick_clock.NowTicks(),
        nqe::internal::NetworkQuality(base::TimeDelta::FromSeconds(1),
                                      base::TimeDelta::FromSeconds(1), 1),
        EFFECTIVE_CONNECTION_TYPE_UNKNOWN);

    // Entry should not be added.
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                        "test1", 0);
    nqe::internal::CachedNetworkQuality read_network_quality;
    network_quality_store.Add(network_id, cached_network_quality_unknown);
    EXPECT_FALSE(
        network_quality_store.GetById(network_id, &read_network_quality));
  }

  {
    // Entry will be added for (2G, "test1").
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                        "test1", 0);
    nqe::internal::CachedNetworkQuality read_network_quality;
    network_quality_store.Add(network_id, cached_network_quality_2g_test1);
    EXPECT_TRUE(
        network_quality_store.GetById(network_id, &read_network_quality));
    EXPECT_EQ(cached_network_quality_2g_test1.network_quality(),
              read_network_quality.network_quality());
  }

  {
    // Entry will be added for (2G, "test2").
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                        "test2", 0);
    nqe::internal::CachedNetworkQuality read_network_quality;
    nqe::internal::CachedNetworkQuality cached_network_quality(
        tick_clock.NowTicks(),
        nqe::internal::NetworkQuality(base::TimeDelta::FromSeconds(2),
                                      base::TimeDelta::FromSeconds(2), 2),
        EFFECTIVE_CONNECTION_TYPE_2G);
    network_quality_store.Add(network_id, cached_network_quality);
    EXPECT_TRUE(
        network_quality_store.GetById(network_id, &read_network_quality));
    EXPECT_EQ(read_network_quality.network_quality(),
              cached_network_quality.network_quality());
  }

  {
    // Entry will be added for (3G, "test3").
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_3G,
                                        "test3", 0);
    nqe::internal::CachedNetworkQuality read_network_quality;
    nqe::internal::CachedNetworkQuality cached_network_quality(
        tick_clock.NowTicks(),
        nqe::internal::NetworkQuality(base::TimeDelta::FromSeconds(3),
                                      base::TimeDelta::FromSeconds(3), 3),
        EFFECTIVE_CONNECTION_TYPE_3G);
    network_quality_store.Add(network_id, cached_network_quality);
    EXPECT_TRUE(
        network_quality_store.GetById(network_id, &read_network_quality));
    EXPECT_EQ(read_network_quality.network_quality(),
              cached_network_quality.network_quality());
  }

  {
    // Entry will not be added for (Unknown, "").
    nqe::internal::NetworkID network_id(
        NetworkChangeNotifier::CONNECTION_UNKNOWN, "", 0);
    nqe::internal::CachedNetworkQuality read_network_quality;
    nqe::internal::CachedNetworkQuality set_network_quality(
        tick_clock.NowTicks(),
        nqe::internal::NetworkQuality(base::TimeDelta::FromSeconds(4),
                                      base::TimeDelta::FromSeconds(4), 4),
        EFFECTIVE_CONNECTION_TYPE_4G);
    network_quality_store.Add(network_id, set_network_quality);
    EXPECT_FALSE(
        network_quality_store.GetById(network_id, &read_network_quality));
  }

  {
    // Existing entry will be read for (2G, "test1").
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                        "test1", 0);
    nqe::internal::CachedNetworkQuality read_network_quality;
    EXPECT_TRUE(
        network_quality_store.GetById(network_id, &read_network_quality));
    EXPECT_EQ(cached_network_quality_2g_test1.network_quality(),
              read_network_quality.network_quality());
  }

  {
    // Existing entry will be overwritten for (2G, "test1").
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                        "test1", 0);
    nqe::internal::CachedNetworkQuality read_network_quality;
    const nqe::internal::CachedNetworkQuality cached_network_quality(
        tick_clock.NowTicks(),
        nqe::internal::NetworkQuality(base::TimeDelta::FromSeconds(5),
                                      base::TimeDelta::FromSeconds(5), 5),
        EFFECTIVE_CONNECTION_TYPE_4G);
    network_quality_store.Add(network_id, cached_network_quality);
    EXPECT_TRUE(
        network_quality_store.GetById(network_id, &read_network_quality));
    EXPECT_EQ(cached_network_quality.network_quality(),
              read_network_quality.network_quality());
  }

  {
    // No entry should exist for (2G, "test4").
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                        "test4", 0);
    nqe::internal::CachedNetworkQuality read_network_quality;
    EXPECT_FALSE(
        network_quality_store.GetById(network_id, &read_network_quality));
  }
}

TEST(NetworkQualityStoreTest, TestCachingClosestSignalStrength) {
  nqe::internal::NetworkQualityStore network_quality_store;
  base::SimpleTestTickClock tick_clock;

  // Cached network quality for network with NetworkID (2G, "test1").
  const nqe::internal::CachedNetworkQuality cached_network_quality_strength_1(
      tick_clock.NowTicks(),
      nqe::internal::NetworkQuality(base::TimeDelta::FromSeconds(1),
                                    base::TimeDelta::FromSeconds(1), 1),
      EFFECTIVE_CONNECTION_TYPE_2G);

  const nqe::internal::CachedNetworkQuality cached_network_quality_strength_3(
      tick_clock.NowTicks(),
      nqe::internal::NetworkQuality(base::TimeDelta::FromSeconds(3),
                                    base::TimeDelta::FromSeconds(3), 3),
      EFFECTIVE_CONNECTION_TYPE_2G);

  {
    // Entry will be added for (2G, "test1") with signal strength value of 1.
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                        "test1", 1);
    nqe::internal::CachedNetworkQuality read_network_quality;
    network_quality_store.Add(network_id, cached_network_quality_strength_1);
    EXPECT_TRUE(
        network_quality_store.GetById(network_id, &read_network_quality));
    EXPECT_EQ(cached_network_quality_strength_1.network_quality(),
              read_network_quality.network_quality());
  }

  {
    // Entry will be added for (2G, "test1") with signal strength value of 3.
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                        "test1", 3);
    nqe::internal::CachedNetworkQuality read_network_quality;
    network_quality_store.Add(network_id, cached_network_quality_strength_3);
    EXPECT_TRUE(
        network_quality_store.GetById(network_id, &read_network_quality));
    EXPECT_EQ(cached_network_quality_strength_3.network_quality(),
              read_network_quality.network_quality());
  }

  {
    // Now with cached entries for signal strengths 1 and 3, verify across the
    // range of strength values that the closest value match will be returned
    // when looking up (2G, "test1", signal_strength).
    for (int32_t signal_strength = 0; signal_strength <= 4; ++signal_strength) {
      nqe::internal::CachedNetworkQuality expected_cached_network_quality =
          signal_strength <= 2 ? cached_network_quality_strength_1
                               : cached_network_quality_strength_3;
      nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                          "test1", signal_strength);
      nqe::internal::CachedNetworkQuality read_network_quality;
      EXPECT_TRUE(
          network_quality_store.GetById(network_id, &read_network_quality));
      EXPECT_EQ(expected_cached_network_quality.network_quality(),
                read_network_quality.network_quality());
    }
  }

  {
    // Existing entry will be read for (2G, "test1", INT32_MIN).
    // The first entry in the store matching (2G, "test1", *) would be returned.
    int32_t signal_strength = INT32_MIN;
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                        "test1", signal_strength);
    nqe::internal::CachedNetworkQuality read_network_quality;
    EXPECT_TRUE(
        network_quality_store.GetById(network_id, &read_network_quality));
    EXPECT_TRUE((cached_network_quality_strength_1.network_quality() ==
                 read_network_quality.network_quality()) ||
                (cached_network_quality_strength_3.network_quality() ==
                 read_network_quality.network_quality()));
  }

  {
    // No entry should exist for (2G, "test4").
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                        "test4", 0);
    nqe::internal::CachedNetworkQuality read_network_quality;
    EXPECT_FALSE(
        network_quality_store.GetById(network_id, &read_network_quality));
  }
}
// Tests if the cache size remains bounded. Also, ensure that the cache is
// LRU.
TEST(NetworkQualityStoreTest, TestLRUCacheMaximumSize) {
  nqe::internal::NetworkQualityStore network_quality_store;
  base::SimpleTestTickClock tick_clock;

  // Add more networks than the maximum size of the cache.
  const size_t network_count = 11;

  nqe::internal::CachedNetworkQuality read_network_quality(
      tick_clock.NowTicks(),
      nqe::internal::NetworkQuality(base::TimeDelta::FromSeconds(0),
                                    base::TimeDelta::FromSeconds(0), 0),
      EFFECTIVE_CONNECTION_TYPE_2G);

  for (size_t i = 0; i < network_count; ++i) {
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                        "test" + base::IntToString(i), 0);

    const nqe::internal::CachedNetworkQuality network_quality(
        tick_clock.NowTicks(),
        nqe::internal::NetworkQuality(base::TimeDelta::FromSeconds(1),
                                      base::TimeDelta::FromSeconds(1), 1),
        EFFECTIVE_CONNECTION_TYPE_2G);
    network_quality_store.Add(network_id, network_quality);
    tick_clock.Advance(base::TimeDelta::FromSeconds(1));
  }

  base::TimeTicks earliest_last_update_time = tick_clock.NowTicks();
  size_t cache_match_count = 0;
  for (size_t i = 0; i < network_count; ++i) {
    nqe::internal::NetworkID network_id(NetworkChangeNotifier::CONNECTION_2G,
                                        "test" + base::IntToString(i), 0);

    nqe::internal::CachedNetworkQuality read_network_quality(
        tick_clock.NowTicks(),
        nqe::internal::NetworkQuality(base::TimeDelta::FromSeconds(0),
                                      base::TimeDelta::FromSeconds(0), 0),
        EFFECTIVE_CONNECTION_TYPE_2G);
    if (network_quality_store.GetById(network_id, &read_network_quality)) {
      cache_match_count++;
      earliest_last_update_time = std::min(
          earliest_last_update_time, read_network_quality.last_update_time());
    }
  }

  // Ensure that the number of entries in cache are fewer than |network_count|.
  EXPECT_LT(cache_match_count, network_count);
  EXPECT_GT(cache_match_count, 0u);

  // Ensure that only LRU entries are cached by comparing the
  // |earliest_last_update_time|.
  EXPECT_EQ(
      tick_clock.NowTicks() - base::TimeDelta::FromSeconds(cache_match_count),
      earliest_last_update_time);
}

}  // namespace

}  // namespace net
