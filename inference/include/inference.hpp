//
// Created by JellyfishKnight on 2025/2/26.
//

#ifndef BABBLE_INFERENCE_HPP
#define BABBLE_INFERENCE_HPP

#include "kalman_filter.hpp"
#include <string>
#include <memory>
#include <vector>
#include <opencv2/core.hpp>
#include <onnxruntime_cxx_api.h>
#include "logger.hpp"
#include <chrono>

class Inference {
public:
    Inference() : input_h_(256), input_w_(256), input_c_(1)
    {
        init_kalman_filter();
    }
    ~Inference() = default;

    // 加载模型
    void load_model(const std::string &model_path = "");

    // 运行推理
    void inference(cv::Mat& image);

    std::vector<float> get_output();

    void set_use_filter(bool use);
private:
    void init_kalman_filter();

    // 初始化输入输出名称
    void init_io_names();

    // 预分配所有缓冲区
    void allocate_buffers();

    // 预处理图像
    void preprocess(const cv::Mat& input);

    // 运行模型
    void run_model();

    // 处理结果
    void process_results();

    void plot_curve(float raw, float filtered);

    // 会话和会话选项
    std::shared_ptr<Ort::Session> session_;
    Ort::SessionOptions session_options;

    // 输入输出名称
    std::vector<std::string> input_names_;
    std::vector<std::string> output_names_;
    std::vector<const char*> input_name_ptrs_;
    std::vector<const char*> output_name_ptrs_;

    // 输入形状
    std::vector<std::vector<int64_t>> input_shapes_;

    // 输入尺寸
    int input_h_;
    int input_w_;
    int input_c_;

    // 预分配的缓冲区
    cv::Mat gray_image_;           // 灰度转换缓冲区
    cv::Mat processed_image_;      // 预处理图像缓冲区
    std::vector<float> input_data_; // 输入数据缓冲区

    // ONNX Runtime资源
    Ort::MemoryInfo memory_info_{nullptr};
    Ort::Value input_tensor_{nullptr};
    std::vector<Ort::Value> output_tensors_;

    bool use_filter = false;
    bool last_use_filter = use_filter;
    KalmanFilter kalman_filter_;
    std::vector<float> raw_data;       // 存储原始数据
    std::vector<float> filtered_data;  // 存储滤波后数据
    int max_points = 200;        // 只保留最近 200 个点，防止图像过长
};

#endif //BABBLE_INFERENCE_HPP
