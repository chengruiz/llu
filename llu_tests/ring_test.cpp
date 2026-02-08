#include <gtest/gtest.h>

#include <llu/ring.h>

TEST(LLU_RING_TEST, LLU_RING_SIZE_TEST) {
  llu::RingBuffer<int> q(3);
  ASSERT_TRUE(q.empty());
  ASSERT_FALSE(q.full());
  ASSERT_EQ(q.size(), 0);
  q.push_back(1);
  ASSERT_FALSE(q.empty());
  ASSERT_FALSE(q.full());
  ASSERT_EQ(q.size(), 1);
  q.push_back(2);
  ASSERT_FALSE(q.empty());
  ASSERT_FALSE(q.full());
  ASSERT_EQ(q.size(), 2);
  q.push_back(3);
  ASSERT_FALSE(q.empty());
  ASSERT_TRUE(q.full());
  ASSERT_EQ(q.size(), 3);
  q.push_back(4);
  ASSERT_FALSE(q.empty());
  ASSERT_TRUE(q.full());
  ASSERT_EQ(q.size(), 3);
  q.clear();
  ASSERT_TRUE(q.empty());
  ASSERT_FALSE(q.full());
  ASSERT_EQ(q.size(), 0);
}

TEST(LLU_RING_TEST, LLU_RING_DATA_TEST) {
  llu::RingBuffer<int> q(3);
  try {
    auto front = q.front();
    ASSERT_TRUE(false) << "EmptyQueue exception should be thrown";
  } catch (llu::RingBuffer<int>::EmptyQueue &e) {}
  try {
    auto back = q.back();
    ASSERT_TRUE(false) << "EmptyQueue exception should be thrown";
  } catch (llu::RingBuffer<int>::EmptyQueue &e) {}
  try {
    auto item = q.at(0);
    ASSERT_TRUE(false) << "IndexOutOfRange exception should be thrown";
  } catch (llu::RingBuffer<int>::IndexOutOfRange &e) {}

  q.push_back(0);
  q.push_back(1);
  q.emplace_back(2);
  q.emplace_back(3);
  q.push_back(4);
  ASSERT_EQ(q.front(), 2);
  ASSERT_EQ(q.back(), 4);
  ASSERT_EQ(q.at(0), 2);
  ASSERT_EQ(q.at(1), 3);
  ASSERT_EQ(q.at(2), 4);
  ASSERT_EQ(q.at(-1), 4);
  ASSERT_EQ(q.at(-2), 3);
  ASSERT_EQ(q.at(-3), 2);
  try {
    auto item = q.at(3);
    ASSERT_TRUE(false) << "IndexOutOfRange exception should be thrown";
  } catch (llu::RingBuffer<int>::IndexOutOfRange &e) {}
  try {
    auto item = q.at(-4);
    ASSERT_TRUE(false) << "IndexOutOfRange exception should be thrown";
  } catch (llu::RingBuffer<int>::IndexOutOfRange &e) {}

  ASSERT_EQ(q.at(0, -1), 2);
  ASSERT_EQ(q.at(1, -1), 3);
  ASSERT_EQ(q.at(2, -1), 4);
  ASSERT_EQ(q.at(3, -1), -1);
  ASSERT_EQ(q.at(4, -1), -1);
  ASSERT_EQ(q.at(-1, -1), 4);
  ASSERT_EQ(q.at(-2, -1), 3);
  ASSERT_EQ(q.at(-3, -1), 2);
  ASSERT_EQ(q.at(-4, -1), -1);
  ASSERT_EQ(q.at(-5, -1), -1);
}

TEST(LLU_RING_TEST, LLU_RING_ALLOCATE_TEST) {
  llu::RingBuffer<int> q;
  q.allocate(3);
  q.push_back(1);
  q.push_back(2);
  q.push_back(3);
  q.push_back(4);

  q.allocate(2);
  ASSERT_EQ(q.size(), 2);
  ASSERT_EQ(q.front(), 3);
  ASSERT_EQ(q.back(), 4);
}

TEST(LLU_RING_TEST, LLU_RING_MODIFIER_TEST) {
  llu::RingBuffer<int> q(3);
  q.push_back(1);
  q.push_back(2);
  q.push_back(3);

  q.pop_front();
  ASSERT_EQ(q.size(), 2);
  ASSERT_EQ(q.front(), 2);
  ASSERT_EQ(q.back(), 3);

  q.push_front(0);
  ASSERT_EQ(q.size(), 3);
  ASSERT_EQ(q.front(), 0);
  ASSERT_EQ(q.back(), 3);

  q.pop_back();
  ASSERT_EQ(q.size(), 2);
  ASSERT_EQ(q.front(), 0);
  ASSERT_EQ(q.back(), 2);

  q.push_front(5);
  q.push_front(6);
  ASSERT_EQ(q.size(), 3);
  ASSERT_EQ(q.front(), 6);
  ASSERT_EQ(q.back(), 0);
}

TEST(LLU_RING_TEST, LLU_RING_ITERATOR_TEST) {
  llu::RingBuffer<int> q(3);
  q.push_back(1);
  q.push_back(2);
  q.push_back(3);

  std::vector<int> v;
  for (auto x : q) {
    v.push_back(x);
  }
  ASSERT_EQ(v.size(), 3);
  ASSERT_EQ(v[0], 1);
  ASSERT_EQ(v[1], 2);
  ASSERT_EQ(v[2], 3);
}

TEST(LLU_RING_TEST, LLU_RING_CONST_ITERATOR_TEST) {
  llu::RingBuffer<int> q(3);
  q.push_back(1);
  q.push_back(2);
  q.push_back(3);

  std::vector<int> v;
  for (auto it = q.cbegin(); it != q.cend(); ++it) {
    v.push_back(*it);
  }
  ASSERT_EQ(v.size(), 3);
  ASSERT_EQ(v[0], 1);
  ASSERT_EQ(v[1], 2);
  ASSERT_EQ(v[2], 3);
}

TEST(LLU_RING_TEST, LLU_RING_REVERSE_ITERATOR_TEST) {
  llu::RingBuffer<int> q(3);
  q.push_back(1);
  q.push_back(2);
  q.push_back(3);

  std::vector<int> v;
  for (auto it = q.rbegin(); it != q.rend(); ++it) {
    v.push_back(*it);
  }
  ASSERT_EQ(v.size(), 3);
  ASSERT_EQ(v[0], 3);
  ASSERT_EQ(v[1], 2);
  ASSERT_EQ(v[2], 1);

  v.clear();
  for (auto it = q.crbegin(); it != q.crend(); ++it) {
    v.push_back(*it);
  }
  ASSERT_EQ(v.size(), 3);
  ASSERT_EQ(v[0], 3);
  ASSERT_EQ(v[1], 2);
  ASSERT_EQ(v[2], 1);
}
