 //
 // Created by JellyfishKnight on 2025/2/26.
 //

 #ifndef INFERENCE_HPP
 #define INFERENCE_HPP

#include <string>
#include <opencv2/core.hpp>
#include <onnxruntime_cxx_api.h>

class Inference {
public:
     Inference() = default;

     ~Inference() = default;
     //D:\Babble\model\model.onnx
     void load_model(const std::string &model_path);

     void inference(cv::Mat& image);

     void show_result();
private:
     void preprocess(cv::Mat& image);

     void run_model(cv::Mat& image);

     cv::Mat result;
     Ort::SessionOptions session_options;
    std::shared_ptr<Ort::Session> session_;
};



 #endif //INFERENCE_HPP
