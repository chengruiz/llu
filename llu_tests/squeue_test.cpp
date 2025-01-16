#include <gtest/gtest.h>

#include <llu/squeue.h>

TEST(LLU_SQUEUE_TEST, LLU_SQUEUE_SIZE_TEST) {
  llu::StaticQueue<int> q(3);
  ASSERT_TRUE(q.is_empty()) << "Queue should be empty";
  ASSERT_FALSE(q.is_full()) << "Queue should not be full";
  ASSERT_EQ(q.size(), 0) << "Size should be 0";
  q.push_back(1);
  ASSERT_FALSE(q.is_empty()) << "Queue should not be empty";
  ASSERT_FALSE(q.is_full()) << "Queue should not be full";
  ASSERT_EQ(q.size(), 1) << "Size should be 1";
  q.push_back(2);
  ASSERT_FALSE(q.is_empty()) << "Queue should not be empty";
  ASSERT_FALSE(q.is_full()) << "Queue should not be full";
  ASSERT_EQ(q.size(), 2) << "Size should be 2";
  q.push_back(3);
  ASSERT_FALSE(q.is_empty()) << "Queue should not be empty";
  ASSERT_TRUE(q.is_full()) << "Queue should be full";
  ASSERT_EQ(q.size(), 3) << "Size should be 3";
  q.push_back(4);
  ASSERT_FALSE(q.is_empty()) << "Queue should not be empty";
  ASSERT_TRUE(q.is_full()) << "Queue should be full";
  ASSERT_EQ(q.size(), 3) << "Size should be 3";
  q.clear();
  ASSERT_TRUE(q.is_empty()) << "Queue should be empty";
  ASSERT_FALSE(q.is_full()) << "Queue should not be full";
  ASSERT_EQ(q.size(), 0) << "Size should be 0";
}

TEST(LLU_SQUEUE_TEST, LLU_SQUEUE_DATA_TEST) {
  llu::StaticQueue<int> q(3);
  try {
    auto front = q.front();
    ASSERT_TRUE(false) << "EmptyQueue exception should be thrown";
  } catch (llu::StaticQueue<int>::EmptyQueue &e) {}
  try {
    auto back = q.back();
    ASSERT_TRUE(false) << "EmptyQueue exception should be thrown";
  } catch (llu::StaticQueue<int>::EmptyQueue &e) {}
  try {
    auto item = q.at(0);
    ASSERT_TRUE(false) << "IndexOutOfRange exception should be thrown";
  } catch (llu::StaticQueue<int>::IndexOutOfRange &e) {}
  try {
    auto item = q.get_padded(1);
    ASSERT_TRUE(false) << "EmptyQueue exception should be thrown";
  } catch (llu::StaticQueue<int>::EmptyQueue &e) {}

  q.push_back(0);
  q.push_back(1);
  q.emplace_back(2);
  q.emplace_back(3);
  q.push_back(4);
  ASSERT_EQ(q.front(), 2) << "Front should be 2";
  ASSERT_EQ(q.back(), 4) << "Back should be 4";
  ASSERT_EQ(q.at(0), 2) << "At(0) should be 2";
  ASSERT_EQ(q.at(1), 3) << "At(1) should be 3";
  ASSERT_EQ(q.at(2), 4) << "At(2) should be 4";
  ASSERT_EQ(q.at(-1), 4) << "At(-1) should be 4";
  ASSERT_EQ(q.at(-2), 3) << "At(-2) should be 3";
  ASSERT_EQ(q.at(-3), 2) << "At(-3) should be 2";
  try {
    auto item = q.at(3);
    ASSERT_TRUE(false) << "IndexOutOfRange exception should be thrown";
  } catch (llu::StaticQueue<int>::IndexOutOfRange &e) {}
  try {
    auto item = q.at(-4);
    ASSERT_TRUE(false) << "IndexOutOfRange exception should be thrown";
  } catch (llu::StaticQueue<int>::IndexOutOfRange &e) {}

  ASSERT_EQ(q.get(0, -1), 2) << "Get(0, -1) should be 2";
  ASSERT_EQ(q.get(1, -1), 3) << "Get(1, -1) should be 3";
  ASSERT_EQ(q.get(2, -1), 4) << "Get(2, -1) should be 4";
  ASSERT_EQ(q.get(3, -1), -1) << "Get(3, -1) should be -1";
  ASSERT_EQ(q.get(4, -1), -1) << "Get(4, -1) should be -1";
  ASSERT_EQ(q.get(-1, -1), 4) << "Get(-1, -1) should be 4";
  ASSERT_EQ(q.get(-2, -1), 3) << "Get(-2, -1) should be 3";
  ASSERT_EQ(q.get(-3, -1), 2) << "Get(-3, -1) should be 2";
  ASSERT_EQ(q.get(-4, -1), -1) << "Get(-4, -1) should be -1";
  ASSERT_EQ(q.get(-5, -1), -1) << "Get(-5, -1) should be -1";

  ASSERT_EQ(q.get_padded(0), 2) << "GetPadded(0) should be 2";
  ASSERT_EQ(q.get_padded(1), 3) << "GetPadded(1) should be 3";
  ASSERT_EQ(q.get_padded(2), 4) << "GetPadded(2) should be 4";
  ASSERT_EQ(q.get_padded(3), 4) << "GetPadded(3) should be 4";
  ASSERT_EQ(q.get_padded(4), 4) << "GetPadded(4) should be 4";
  ASSERT_EQ(q.get_padded(-1), 4) << "GetPadded(-1) should be 4";
  ASSERT_EQ(q.get_padded(-2), 3) << "GetPadded(-2) should be 3";
  ASSERT_EQ(q.get_padded(-3), 2) << "GetPadded(-3) should be 2";
  ASSERT_EQ(q.get_padded(-4), 2) << "GetPadded(-4) should be 2";
  ASSERT_EQ(q.get_padded(-5), 2) << "GetPadded(-5) should be 2";
}
