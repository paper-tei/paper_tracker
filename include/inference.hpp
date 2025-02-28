//
// Created by JellyfishKnight on 2025/2/26.
//

#ifndef BABBLE_INFERENCE_HPP
#define BABBLE_INFERENCE_HPP

#include <string>
#include <memory>
#include <vector>
#include <opencv2/core.hpp>
#include <onnxruntime_cxx_api.h>

class Inference {
public:
    Inference() : input_h_(256), input_w_(256), input_c_(1) {}
    ~Inference() = default;

    // 加载模型
    void load_model(const std::string &model_path = "");

    // 运行推理
    void inference(cv::Mat& image);

    // 显示结果
    void show_result();

private:
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
};

#endif //BABBLE_INFERENCE_HPP
