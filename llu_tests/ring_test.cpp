#include <gtest/gtest.h>

#include <vector>

#include <llu/ring.h>

namespace {
std::vector<int> toVector(const llu::RingBuffer<int> &buffer) {
  std::vector<int> values;
  for (int value : buffer) {
    values.push_back(value);
  }
  return values;
}

std::vector<int> toReverseVector(const llu::RingBuffer<int> &buffer) {
  std::vector<int> values;
  for (auto it = buffer.crbegin(); it != buffer.crend(); ++it) {
    values.push_back(*it);
  }
  return values;
}
}  // namespace

TEST(LLU_RING_TEST, UnallocatedOperationsThrowMeaningfulExceptions) {
  llu::RingBuffer<int> buffer;

  EXPECT_THROW(buffer.push_back(1), llu::NotAllocatedError);
  EXPECT_THROW(buffer.push_front(1), llu::NotAllocatedError);
  EXPECT_THROW(buffer.fill(1), llu::NotAllocatedError);
}

TEST(LLU_RING_TEST, SizeStateTracksPushBacksAndOverwriteBehavior) {
  llu::RingBuffer<int> buffer(3);

  EXPECT_TRUE(buffer.empty());
  EXPECT_FALSE(buffer.full());
  EXPECT_EQ(buffer.size(), 0u);

  buffer.push_back(1);
  buffer.push_back(2);
  EXPECT_FALSE(buffer.empty());
  EXPECT_FALSE(buffer.full());
  EXPECT_EQ(buffer.size(), 2u);

  buffer.push_back(3);
  EXPECT_TRUE(buffer.full());
  EXPECT_EQ(buffer.size(), 3u);

  buffer.push_back(4);
  EXPECT_TRUE(buffer.full());
  EXPECT_EQ(buffer.size(), 3u);
  EXPECT_EQ(toVector(buffer), (std::vector<int>{2, 3, 4}));

  buffer.clear();
  EXPECT_TRUE(buffer.empty());
  EXPECT_FALSE(buffer.full());
  EXPECT_EQ(buffer.size(), 0u);
}

TEST(LLU_RING_TEST, FrontBackAndAtSupportPositiveAndNegativeIndices) {
  llu::RingBuffer<int> buffer(3);

  EXPECT_THROW(static_cast<void>(buffer.front()), llu::UnderflowError);
  EXPECT_THROW(static_cast<void>(buffer.back()), llu::UnderflowError);
  EXPECT_THROW(static_cast<void>(buffer.at(0)), llu::IndexError);

  buffer.push_back(0);
  buffer.push_back(1);
  buffer.emplace_back(2);
  buffer.emplace_back(3);
  buffer.push_back(4);

  EXPECT_EQ(buffer.front(), 2);
  EXPECT_EQ(buffer.back(), 4);
  EXPECT_EQ(buffer.at(0), 2);
  EXPECT_EQ(buffer.at(1), 3);
  EXPECT_EQ(buffer.at(2), 4);
  EXPECT_EQ(buffer.at(-1), 4);
  EXPECT_EQ(buffer.at(-2), 3);
  EXPECT_EQ(buffer.at(-3), 2);
  EXPECT_THROW(static_cast<void>(buffer.at(3)), llu::IndexError);
  EXPECT_THROW(static_cast<void>(buffer.at(-4)), llu::IndexError);

  EXPECT_EQ(buffer.at(0, -1), 2);
  EXPECT_EQ(buffer.at(1, -1), 3);
  EXPECT_EQ(buffer.at(2, -1), 4);
  EXPECT_EQ(buffer.at(3, -1), -1);
  EXPECT_EQ(buffer.at(-1, -1), 4);
  EXPECT_EQ(buffer.at(-3, -1), 2);
  EXPECT_EQ(buffer.at(-5, -1), -1);
}

TEST(LLU_RING_TEST, AllocateKeepsTheMostRecentValuesAndResetDropsCapacity) {
  llu::RingBuffer<int> buffer;
  buffer.allocate(3);
  buffer.push_back(1);
  buffer.push_back(2);
  buffer.push_back(3);
  buffer.push_back(4);

  buffer.allocate(2);
  EXPECT_EQ(buffer.size(), 2u);
  EXPECT_EQ(toVector(buffer), (std::vector<int>{3, 4}));

  buffer.reset();
  EXPECT_EQ(buffer.capacity(), 0u);
  EXPECT_TRUE(buffer.empty());
  EXPECT_THROW(buffer.push_back(5), llu::NotAllocatedError);
}

TEST(LLU_RING_TEST, PushFrontAndPopOperationsUpdateBothEnds) {
  llu::RingBuffer<int> buffer(3);
  buffer.push_back(1);
  buffer.push_back(2);
  buffer.push_back(3);

  buffer.pop_front();
  EXPECT_EQ(toVector(buffer), (std::vector<int>{2, 3}));

  buffer.push_front(0);
  EXPECT_EQ(toVector(buffer), (std::vector<int>{0, 2, 3}));

  buffer.pop_back();
  EXPECT_EQ(toVector(buffer), (std::vector<int>{0, 2}));

  buffer.push_front(5);
  buffer.push_front(6);
  EXPECT_EQ(buffer.front(), 6);
  EXPECT_EQ(buffer.back(), 0);
  EXPECT_EQ(toVector(buffer), (std::vector<int>{6, 5, 0}));
}

TEST(LLU_RING_TEST, FillPopulatesTheEntireAllocatedBuffer) {
  llu::RingBuffer<int> buffer(4);

  buffer.fill(7);

  EXPECT_TRUE(buffer.full());
  EXPECT_EQ(buffer.size(), 4u);
  EXPECT_EQ(toVector(buffer), (std::vector<int>{7, 7, 7, 7}));
}

TEST(LLU_RING_TEST, IteratorsTraverseForwardAndReverseAndSupportRandomAccess) {
  llu::RingBuffer<int> buffer(3);
  buffer.push_back(1);
  buffer.push_back(2);
  buffer.push_back(3);

  auto it = buffer.begin();
  EXPECT_EQ(*it, 1);
  EXPECT_EQ(it[1], 2);
  EXPECT_EQ(*(it + 2), 3);
  EXPECT_EQ(buffer.end() - buffer.begin(), 3);

  const auto &const_buffer = buffer;
  auto cit = const_buffer.cbegin();
  EXPECT_EQ(*cit, 1);
  EXPECT_EQ(cit[2], 3);

  EXPECT_EQ(toVector(buffer), (std::vector<int>{1, 2, 3}));
  EXPECT_EQ(toReverseVector(buffer), (std::vector<int>{3, 2, 1}));
}
