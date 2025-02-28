//
// Created by JellyfishKnight on 2025/2/26.
//
#include "inference.hpp"
#include <iostream>
#include <onnxruntime_cxx_api.h>
#include <opencv2/highgui.hpp>

void Inference::load_model(const std::string &model_path) {
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ONNXRuntimeDemo");

    // 配置会话选项（可选）
    session_options.SetIntraOpNumThreads(12);  // 设置线程数
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
    const wchar_t* rmodel_path = L"D:/Babble/model/model.onnx";
    session_ = std::make_shared<Ort::Session>(env, rmodel_path, session_options);
}

void Inference::inference(cv::Mat& image) {
    if (image.empty())
    {
        return ;
    }
    preprocess(image);
    run_model(image);
}

void Inference::run_model(cv::Mat& image) {

    if (!image.isContinuous())
    {
        image = image.clone();
    }

    Ort::AllocatorWithDefaultOptions allocator;
    size_t num_input_nodes = session_->GetInputCount();
    size_t num_output_nodes = session_->GetOutputCount();
    std::vector<const char*> input_names(num_input_nodes);
    for (size_t i = 0; i < num_input_nodes; i++) {
        input_names[i] = session_->GetInputNameAllocated(i, allocator).get();
    }

    std::vector<const char*> output_names(num_output_nodes);
    for (size_t i = 0; i < num_output_nodes; i++) {
        output_names[i] = session_->GetOutputNameAllocated(i, allocator).get();
    }

    float* data_ptr = image.ptr<float>();
    std::size_t data_size = image.total() * image.channels();

    std::vector<float> input_data(data_ptr, data_ptr + data_size);
    std::vector<int64_t> input_shape = {1, 1, 256, 256};

    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(
        OrtAllocatorType::OrtArenaAllocator,
        OrtMemType::OrtMemTypeDefault
    );

    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        memory_info,
        input_data.data(),
        input_data.size(),
        input_shape.data(),
        input_shape.size()
    );

    std::vector<Ort::Value> output_tensors;

    try
    {
        output_tensors = session_->Run(
            Ort::RunOptions{nullptr},  // 运行选项（默认可空）
            input_names.data(),        // 输入名称数组
            &input_tensor,             // 输入张量指针数组
            num_input_nodes,           // 输入数量
            output_names.data(),       // 输出名称数组
            num_output_nodes           // 输出数量
        );
    } catch (std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return ;
    }

    if (output_tensors.empty())
    {
        std::cout << "No Target Found" << std::endl;
        return ;
    }
    Ort::Value& output_tensor = output_tensors.front();
    float* output_data = output_tensor.GetTensorMutableData<float>();

    // 获取输出维度
    auto output_shape = output_tensor.GetTensorTypeAndShapeInfo().GetShape();
    size_t output_size = 1;
    for (auto dim : output_shape) {
        output_size *= dim;
    }

    // 打印结果示例
    std::cout << "Output shape: ";
    for (auto dim : output_shape) std::cout << dim << " ";
    std::cout << "\nModelValues: ";
    for (size_t i = 0; i < output_size; i++) {
        std::cout << output_data[i] << " ";
    }
    std::cout << std::endl;
}


void Inference::show_result() {

}

void Inference::preprocess(cv::Mat& image) {
    image.convertTo(image, CV_32F);
    image.resize(256, 256);
}
