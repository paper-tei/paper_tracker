//
// Created by JellyfishKnight on 2025/2/26.
//
#include "inference.hpp"
#include <iostream>

int main() {
    cv::VideoCapture cap;
    cap.open("D:/Babble/test_video");
    Inference inference;
    inference.load_model("D:/Babble/model/model.onnx");
    cv::Mat frame;
    while (cap.read(frame)) {
        inference.inference(frame);
    }

    return 0;
}

