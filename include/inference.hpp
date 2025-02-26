//
// Created by JellyfishKnight on 2025/2/26.
//

#ifndef INFERENCE_HPP
#define INFERENCE_HPP

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <opencv2/dnn.hpp>
#include <Eigen/Dense>
#include <Eigen/Core>

class Inference {
public:
    Inference() = default;

    ~Inference() = default;
    //D:\Babble\model\model.onnx
    void load_model(const std::string &model_path);

    void inference(cv::Mat image);

    void show_result();
private:
    void preprocess(cv::Mat image);

    void run_model(cv::Mat image);

    cv::Mat result;
    cv::dnn::Net net;

};



#endif //INFERENCE_HPP
