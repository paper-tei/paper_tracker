//
// Created by JellyfishKnight on 2025/2/25.
//

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <string>
#include <vector>


class Camera {
public:
    void find_cameras();

    void open_camera(const std::string& camera_address, const std::string& camera_port);

    void close_camera();



private:

};





#endif //CAMERA_HPP
