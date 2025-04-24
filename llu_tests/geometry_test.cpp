#include <gtest/gtest.h>

#include <Eigen/Geometry>

#include <llu/const.h>
#include <llu/geometry.h>

void testQuat(Eigen::Quaterniond q0, llu::Quatd q1) {
  q0.normalize();
  ASSERT_TRUE(q1.isApprox(llu::Quatd(q0), llu::kEPS)) << "Quaternions should be equal";
  llu::Mat3d m00 = q0.matrix();
  llu::Mat3d m01 = q1.matrix();
  ASSERT_TRUE(m00.isApprox(m01, llu::kEPS)) << "Quaternion to rotation matrix conversion failed";
  ASSERT_TRUE(llu::Quatd::fromMatrix(m01).isApprox(q1, llu::kEPS)) << "Rotation matrix to quaternion conversion failed";
  ASSERT_TRUE(q1.inverse().isApprox(llu::Quatd(q0.inverse()), llu::kEPS)) << "Quaternion inverse failed";
  ASSERT_TRUE((q1 * q1.inverse()).isApprox(llu::Quatd{}, llu::kEPS)) << "Quaternion inverse failed";
  ASSERT_TRUE((q1.inverse() * q1).isApprox(llu::Quatd{}, llu::kEPS)) << "Quaternion inverse failed";
  ASSERT_TRUE(llu::Quatd::fromEulerAngles(q1.eulerAngles()).isApprox(q1, llu::kEPS)) <<
    "Euler angles to quaternion conversion failed";
  llu::Vec3d v{1., 2., 3.};
  ASSERT_TRUE((q0 * v).isApprox(q1 * v, llu::kEPS)) << "Quaternion rotation failed";
  auto q2 = Eigen::Quaterniond(0.7, 0.6, 0.5, 0.4).normalized();
  ASSERT_TRUE((q1 * llu::Quatd{q2}).isApprox(llu::Quatd{q0 * q2}, llu::kEPS)) << "Quaternion multiplication failed";
}

TEST(LLU_GEOMETRY_TEST, LLU_QUATERNION_TEST) {
  auto q00 = Eigen::Quaterniond(0.1, 0.2, 0.3, 0.4).normalized();
  llu::Quatd q01 = llu::Quatd(0.1, 0.2, 0.3, 0.4);
  testQuat(q00, q01);

  auto q10 = Eigen::Quaterniond(-0.1, 0.2, -0.3, 0.4).normalized();
  llu::Quatd q11 = llu::Quatd(-0.1, 0.2, -0.3, 0.4);
  testQuat(q10, q11);

  auto q20 = Eigen::Quaterniond(-0.9, 0.3, -0.0, 0.1).normalized();
  llu::Quatd q21 = llu::Quatd(-0.9, 0.3, -0.0, 0.1);
  testQuat(q20, q21);

  ASSERT_TRUE(q01.isApprox(q01, llu::kEPS)) << "Quaternions should not be equal";
  ASSERT_TRUE(q11.isApprox(q11, llu::kEPS)) << "Quaternions should not be equal";
  ASSERT_TRUE(q21.isApprox(q21, llu::kEPS)) << "Quaternions should not be equal";
  ASSERT_FALSE(q01.isApprox(q11, llu::kEPS)) << "Quaternions should not be equal";
  ASSERT_FALSE(q01.isApprox(q21, llu::kEPS)) << "Quaternions should not be equal";
  ASSERT_FALSE(q11.isApprox(q21, llu::kEPS)) << "Quaternions should not be equal";
  ASSERT_FALSE(q11.isApprox(q01, llu::kEPS)) << "Quaternions should not be equal";
  ASSERT_FALSE(q21.isApprox(q01, llu::kEPS)) << "Quaternions should not be equal";
  ASSERT_FALSE(q21.isApprox(q11, llu::kEPS)) << "Quaternions should not be equal";

  llu::Quatd q02{q00};
  llu::Vec4d q00_coeffs = q00.coeffs();
  ASSERT_NEAR(q02.w(), q00.w(), llu::kEPS) << "Quaternions should be equal";
  ASSERT_NEAR(q02.x(), q00.x(), llu::kEPS) << "Quaternions should be equal";
  ASSERT_NEAR(q02.y(), q00.y(), llu::kEPS) << "Quaternions should be equal";
  ASSERT_NEAR(q02.z(), q00.z(), llu::kEPS) << "Quaternions should be equal";
  llu::Quatd q03{q00_coeffs};
  ASSERT_NEAR(q03.w(), q00_coeffs[0], llu::kEPS) << "Quaternions should be equal";
  ASSERT_NEAR(q03.x(), q00_coeffs[1], llu::kEPS) << "Quaternions should be equal";
  ASSERT_NEAR(q03.y(), q00_coeffs[2], llu::kEPS) << "Quaternions should be equal";
  ASSERT_NEAR(q03.z(), q00_coeffs[3], llu::kEPS) << "Quaternions should be equal";
}
