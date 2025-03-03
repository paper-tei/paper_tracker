//
// Created by JellyfishKnight on 25-3-3.
//

#ifndef KALMAN_FILTER_HPP
#define KALMAN_FILTER_HPP

#include <functional>

#include <opencv2/core.hpp>

class KalmanFilter
{
public:
    KalmanFilter() = default;

    KalmanFilter(const std::function<cv::Mat(const cv::Mat&)>& TransMat,
                      const std::function<cv::Mat(const cv::Mat&)>& MeasureMat,
                      const std::function<cv::Mat()>& update_Q,
                      const std::function<cv::Mat(const cv::Mat&)>& update_R, const cv::Mat& P);

    void set_state(const cv::Mat& state);

    cv::Mat predict();
    cv::Mat correct(const cv::Mat& measurement);

    std::function<cv::Mat()> update_Q_;
    std::function<cv::Mat(const cv::Mat&)> update_R_;
    std::function<cv::Mat(const cv::Mat&)> trans_mat_;
    std::function<cv::Mat(const cv::Mat&)> measure_mat_;

    cv::Mat F_;  //状态转移矩阵
    cv::Mat H_;  //测量转移矩阵
    cv::Mat Q_;  //过程激励噪声协方差矩阵
    cv::Mat R_;  //测量噪声协方差矩阵
    cv::Mat P_pre_;
    cv::Mat P_post_;
    cv::Mat gain_;        //卡尔曼增益系数
    cv::Mat state_pre_;   // k时刻先验估计
    cv::Mat state_post_;  // k时刻后验估计
};



#endif //KALMAN_FILTER_HPP
