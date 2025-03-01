//
// Created by JellyfishKnight on 2025/2/26.
//
#include "inference.hpp"
#include <iostream>
#include <fstream>
#include <onnxruntime_cxx_api.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

bool file_exists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

std::wstring utf8_to_wstring(const std::string& str) {
    std::wstring result;
    result.assign(str.begin(), str.end());
    return result;
}

void Inference::load_model(const std::string &model_path) {
    try {
        std::string actual_model_path = model_path.empty() ? "D:/Babble/model/model.onnx" : model_path;

        if (!file_exists(actual_model_path)) {
            std::cerr << "错误：模型文件不存在: " << actual_model_path << std::endl;
            return;
        }

        // 创建环境 - 只需要创建一次
        static Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ONNXRuntimeDemo");

        // 配置会话选项 - 关键性能参数
        session_options.SetIntraOpNumThreads(2);  // 减少线程数量，避免CPU过载
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
        session_options.SetExecutionMode(ExecutionMode::ORT_SEQUENTIAL);  // 串行执行可能更适合单个推理
        session_options.AddConfigEntry ("session.intra_op.allow_spinning", "0");
        // 启用内存优化
        // session_options.EnableMemoryPattern();
        session_options.EnableCpuMemArena();
        session_options.DisableMemPattern();
        // self.opts.add_session_config_entry("session.intra_op.allow_spinning", "0")

        // // enable cuda
        // OrtCUDAProviderOptions cuda_options;
        // cuda_options.device_id = 0;                        // 使用 GPU 0
        // cuda_options.arena_extend_strategy = 0;            // 内存分配策略
        // cuda_options.gpu_mem_limit = SIZE_MAX;             // GPU 内存限制
        // cuda_options.cudnn_conv_algo_search = OrtCudnnConvAlgoSearchExhaustive;  // cuDNN 算法搜索策略
        // cuda_options.do_copy_in_default_stream = 1;
        // session_options.AppendExecutionProvider_CUDA(cuda_options);
        // 转换为宽字符路径
        std::wstring wmodel_path = utf8_to_wstring(actual_model_path);

        // 创建会话
        session_ = std::make_shared<Ort::Session>(env, wmodel_path.c_str(), session_options);

        // 初始化输入和输出名称
        init_io_names();

        // 提前分配内存
        allocate_buffers();

        std::cout << "模型加载完成" << std::endl;

    } catch (const Ort::Exception& e) {
        std::cerr << "ONNX Runtime 错误: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "标准异常: " << e.what() << std::endl;
    }
}

void Inference::init_io_names() {
    input_names_.clear();
    output_names_.clear();
    input_name_ptrs_.clear();
    output_name_ptrs_.clear();

    Ort::AllocatorWithDefaultOptions allocator;

    // 获取输入信息
    size_t num_input_nodes = session_->GetInputCount();

    for (size_t i = 0; i < num_input_nodes; i++) {
        auto name = session_->GetInputNameAllocated(i, allocator);
        input_names_.push_back(name.get());

        // 获取输入形状
        auto type_info = session_->GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        auto shape = tensor_info.GetShape();

        // 动态维度处理
        for (size_t j = 0; j < shape.size(); j++) {
            if (shape[j] < 0) {
                if (j == 0) shape[j] = 1;        // 批次大小
                else if (j == 1) shape[j] = 1;   // 通道数(灰度)
                else if (j == 2) shape[j] = 256; // 高度
                else if (j == 3) shape[j] = 256; // 宽度
            }
        }

        input_shapes_.push_back(shape);
    }

    // 更新指针数组 - 只需要执行一次
    for (const auto& name : input_names_) {
        input_name_ptrs_.push_back(name.c_str());
    }

    // 获取输出信息
    size_t num_output_nodes = session_->GetOutputCount();

    for (size_t i = 0; i < num_output_nodes; i++) {
        auto name = session_->GetOutputNameAllocated(i, allocator);
        output_names_.push_back(name.get());
    }

    // 更新指针数组 - 只需要执行一次
    for (const auto& name : output_names_) {
        output_name_ptrs_.push_back(name.c_str());
    }

    // 设置输入维度信息
    if (!input_shapes_.empty()) {
        auto& shape = input_shapes_[0];
        if (shape.size() >= 4) {
            input_c_ = shape[1];
            input_h_ = shape[2];
            input_w_ = shape[3];
        }
    }
}

void Inference::allocate_buffers() {
    // 预分配处理图像的内存
    processed_image_ = cv::Mat(input_h_, input_w_, CV_32FC1);

    // 预分配输入数据内存
    if (!input_shapes_.empty()) {
        size_t input_size = 1;
        for (auto dim : input_shapes_[0]) {
            input_size *= dim;
        }
        input_data_.resize(input_size);
    }

    // 创建内存信息 - 只需创建一次
    memory_info_ = Ort::MemoryInfo::CreateCpu(
        OrtAllocatorType::OrtArenaAllocator,
        OrtMemType::OrtMemTypeDefault
    );
}

void Inference::inference(cv::Mat& image) {
    if (image.empty()) {
        return;
    }

    // 预处理图像 - 直接修改预分配的内存
    preprocess(image);

    // 运行模型
    run_model();
}

void Inference::preprocess(const cv::Mat& input) {
    // 转换为灰度图（如果需要）
    if (input.channels() == 3 && input_c_ == 1) {
        cv::cvtColor(input, gray_image_, cv::COLOR_BGR2GRAY);
    } else if (input.channels() == 1 && input_c_ == 3) {
        cv::cvtColor(input, gray_image_, cv::COLOR_GRAY2BGR);
    } else {
        gray_image_ = input;
    }

    // 调整大小
    cv::resize(gray_image_, processed_image_, cv::Size(input_w_, input_h_));

    // 归一化处理 - 避免不必要的数据复制
    processed_image_.convertTo(processed_image_, CV_32F, 1.0/255.0);

    // 直接拷贝到输入数据缓冲区
    if (processed_image_.isContinuous()) {
        std::memcpy(input_data_.data(), processed_image_.ptr<float>(),
                   processed_image_.total() * sizeof(float));
    } else {
        // 如果数据不连续，则逐行拷贝
        size_t row_bytes = processed_image_.cols * processed_image_.elemSize();
        for (int i = 0; i < processed_image_.rows; ++i) {
            std::memcpy(input_data_.data() + i * processed_image_.cols,
                       processed_image_.ptr(i), row_bytes);
        }
    }
}

void Inference::run_model() {
    if (!session_ || input_name_ptrs_.empty() || output_name_ptrs_.empty()) {
        return;
    }

    try {
        // 创建输入张量 - 使用预分配的内存
        input_tensor_ = Ort::Value::CreateTensor<float>(
            memory_info_,
            input_data_.data(),
            input_data_.size(),
            input_shapes_[0].data(),
            input_shapes_[0].size()
        );

        static Ort::IoBinding io_binding{*session_};
        // 绑定输入
        io_binding.BindInput(input_name_ptrs_[0], input_tensor_);
        // 绑定输出
        for (size_t i = 0; i < output_name_ptrs_.size(); i++) {
            io_binding.BindOutput(output_name_ptrs_[i], memory_info_);
        }
        // 运行推理
        session_->Run(Ort::RunOptions{nullptr}, io_binding);
        // 获取输出
        output_tensors_ = io_binding.GetOutputValues();
        // // 运行模型
        // output_tensors_ = session_->Run(
        //     Ort::RunOptions{nullptr},
        //     input_name_ptrs_.data(),
        //     &input_tensor_,
        //     input_name_ptrs_.size(),
        //     output_name_ptrs_.data(),
        //     output_name_ptrs_.size()
        // );



        // 处理结果
        process_results();

    } catch (const std::exception& e) {
        std::cerr << "推理错误: " << e.what() << std::endl;
    }
}

void Inference::process_results() {
    if (output_tensors_.empty()) {
        return;
    }

    // 处理第一个输出
    try {
        Ort::Value& output_tensor = output_tensors_.front();
        float* output_data = output_tensor.GetTensorMutableData<float>();

        // 获取输出形状
        auto shape_info = output_tensor.GetTensorTypeAndShapeInfo();
        auto output_shape = shape_info.GetShape();

        // 计算输出大小
        size_t output_size = 1;
        for (auto dim : output_shape) {
            output_size *= dim;
        }

        // 这里可以实现你的后处理逻辑
        // 例如: 解析输出数据，更新结果等

    } catch (const std::exception& e) {
        std::cerr << "处理结果出错: " << e.what() << std::endl;
    }
}

void Inference::show_result() {
    // 结果显示逻辑
}

std::vector<float> Inference::get_output() const
{
    if (output_tensors_.empty()) {
        return std::vector<float>();
    }

    try {
        // 获取第一个输出张量
        const Ort::Value& output_tensor = output_tensors_.front();
        const float* output_data = output_tensor.GetTensorData<float>();

        // 获取输出形状
        auto shape_info = output_tensor.GetTensorTypeAndShapeInfo();
        auto output_shape = shape_info.GetShape();

        // 计算输出大小
        size_t output_size = 1;
        for (auto dim : output_shape) {
            output_size *= dim;
        }

        // 复制数据到结果向量
        std::vector<float> result(output_data, output_data + output_size);
        return result;
    }
    catch (const std::exception& e) {
        std::cerr << "获取输出数据错误: " << e.what() << std::endl;
        return std::vector<float>();
    }
}

