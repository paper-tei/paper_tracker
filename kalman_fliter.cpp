//
// Created by JellyfishKnight on 25-3-3.
//
#include <iostream>

#include "kalman_filter.hpp"
/*
 * PaperTracker - 面部追踪应用程序
 * Copyright (C) 2025 PAPER TRACKER
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * This file contains code from projectbabble:
 * Copyright 2023 Sameer Suri
 * Licensed under the Apache License, Version 2.0
 */
KalmanFilter::KalmanFilter(const std::function<cv::Mat(const cv::Mat&)>& TransMat,
                                     const std::function<cv::Mat(const cv::Mat&)>& MeasureMat,
                                     const std::function<cv::Mat()>& update_Q,
                                     const std::function<cv::Mat(const cv::Mat&)>& update_R,
                                     const cv::Mat& P)
  : trans_mat_(TransMat), measure_mat_(MeasureMat), update_Q_(update_Q), update_R_(update_R), P_post_(P)
{
  state_pre_ = cv::Mat::zeros(P.rows, 1, CV_64F);
  state_post_ = cv::Mat::zeros(P.rows, 1, CV_64F);
}

cv::Mat KalmanFilter::predict()
{
  // X_k_bar(先验估计) = A(状态转移矩阵) * X_k-1(后验估计)
  F_ = trans_mat_(state_post_);
  state_pre_ = F_ * state_post_;
  // state_pre_ = trans_mat_ * state_post_;
  Q_ = update_Q_();
  // p_k_bar(误差协方差先验矩阵) = A(状态转移矩阵) * p_k-1(误差协方差后验矩阵) * A_T(状态转移矩阵转置) +
  // Q(过程噪音协方差矩阵)
  P_pre_ = F_ * P_post_ * F_.t() + Q_;
  state_post_ = state_pre_;
  P_post_ = P_pre_;
  return state_pre_;
}

cv::Mat KalmanFilter::correct(const cv::Mat& measurement)
{
  //计算高斯增益
  //               P_k_bar(误差协方差先验矩阵) * H_T(测量转移矩阵的转置)
  // K_k(高斯增益) = ------------------------------------------------------------------------
  //               H(测量转移矩阵) * P_k_bar(误差协方差先验矩阵) * H_T(测量转移矩阵的转置) + R(测量噪音协方差矩阵)
  R_ = update_R_(measurement);
  H_ = measure_mat_(state_pre_);
  cv::Mat S = H_ * P_pre_ * H_.t() + R_;
  gain_ = P_pre_ * H_.t() * (H_ * P_pre_ * H_.t() + R_).inv();  // K
  // X_k(后验估计) = X_k_bar(先验估计) + K_k(高斯增益) * (Z_k(测量量) - H(测量转移矩阵) * X_k_bar(先验估计矩阵))
  state_post_ = state_pre_ + gain_ * (measurement - H_ * state_pre_);
  // P_k(误差协方差后验估计) = P_k_bar(误差协方差先验估计) - K_k(卡尔曼增益) * H(测量转移矩阵) *
  // P_k_bar(误差协方差先验估计)
  P_post_ = P_pre_ - gain_ * H_ * P_pre_;
  return state_post_;
}

void KalmanFilter::set_state(const cv::Mat& state)
{
  state_post_ = state;
}

