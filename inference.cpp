//
// Created by JellyfishKnight on 2025/2/26.
//
#include "inference.hpp"
#include <iostream>

void Inference::load_model(const std::string &model_path) {
    net = cv::dnn::readNetFromONNX(model_path);
}

void Inference::inference(cv::Mat image) {
    preprocess(image);
    run_model(image);
}

void Inference::run_model(cv::Mat image) {
    cv::Mat blob = cv::dnn::blobFromImage(image, 1.0, cv::Size(256, 256), cv::Scalar(0, 0, 0), false, false);

    net.setInput(blob);

    cv::Mat out = net.forward();

    cv::Mat output_clipped;
    cv::max(out, 0, out);
    cv::min(out, 1, output_clipped);
    this->result = output_clipped.clone();

    std::cout << this->result << std::endl;
}


void Inference::show_result() {

}

void Inference::preprocess(cv::Mat image) {
    cv::cvtColor(image, image, cv::COLOR_RGB2GRAY);
    image.resize(256, 256);
}
