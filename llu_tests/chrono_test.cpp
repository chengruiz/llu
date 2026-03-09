#include <gtest/gtest.h>

#include <string>
#include <thread>

#include <llu/chrono.h>

TEST(LLU_CHRONO_TEST, GetElapsedTimeCastsToRequestedUnit) {
  const auto start = llu::SteadyClock::now() - llu::MSec(7);

  EXPECT_GE(llu::getElapsedTime<llu::MSec>(start), 7);
  EXPECT_GE(llu::getElapsedTime<llu::USec>(start), 7000);
}

TEST(LLU_CHRONO_TEST, RateRejectsZeroFrequency) {
  EXPECT_THROW(llu::Rate(0), std::invalid_argument);
}

TEST(LLU_CHRONO_TEST, RateSleepWaitsForConfiguredCycle) {
  llu::Rate rate(200);
  const auto start = llu::SteadyClock::now();

  rate.sleep();

  EXPECT_GE(std::chrono::duration_cast<llu::USec>(llu::SteadyClock::now() - start).count(), 4000);
}

TEST(LLU_CHRONO_TEST, TimerAccumulatesCountTotalAndMean) {
  llu::Timer timer;

  timer.start();
  std::this_thread::sleep_for(llu::MSec(2));
  timer.stop();

  {
    llu::TimerContext timer_context(timer);
    std::this_thread::sleep_for(llu::MSec(1));
  }

  EXPECT_EQ(timer.count(), 2u);
  EXPECT_GT(timer.total<llu::USec>().count(), 0);
  EXPECT_GT(timer.mean<llu::USec>().count(), 0);
  EXPECT_GE(timer.total<llu::USec>().count(), timer.mean<llu::USec>().count());
}

TEST(LLU_CHRONO_TEST, TimerClearResetsAccumulatedStatistics) {
  llu::Timer timer;

  {
    llu::TimerContext timer_context(timer);
    std::this_thread::sleep_for(llu::MSec(1));
  }

  timer.clear();

  EXPECT_EQ(timer.count(), 0u);
  EXPECT_EQ(timer.total(), llu::Duration::zero());
  EXPECT_EQ(timer.mean(), llu::Duration::zero());
}

TEST(LLU_CHRONO_TEST, IntervalStatsTracksIntervalsAndReportsSummary) {
  llu::IntervalStats<llu::USec> stats;

  stats.tick();
  std::this_thread::sleep_for(llu::MSec(1));
  stats.tick();
  std::this_thread::sleep_for(llu::MSec(1));
  stats.tick();

  EXPECT_EQ(stats.count(), 2u);
  EXPECT_NE(stats.infoStr().find("mean ="), std::string::npos);
  EXPECT_NE(stats.infoStr().find("stddev ="), std::string::npos);

  stats.clear();

  EXPECT_EQ(stats.count(), 0u);
}
