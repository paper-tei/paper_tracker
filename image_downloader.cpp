//
// Created by JellyfishKnight on 25-3-2.
//
#include "image_downloader.hpp"

#include <iostream>
#include <opencv2/imgcodecs.hpp>


ImageDownloader::ImageDownloader(const std::string& ip, const std::string& port) :
    ip(ip), port(port), thread_pool_(4)
{
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return;
    }

    // Create a SOCKET for the server to listen for client connections
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    // The sockaddr structure specifies the address family,
    // IP address, and port for the socket that is being bound
    sockaddr_in service{};
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ip.c_str());
    service.sin_port = htons(std::stoi(port));

    // Bind the socket
    if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    // Listen for incoming connection requests
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return;
    }
}

ImageDownloader::~ImageDownloader()
{
    stop_video_stream_server();
    thread_pool_.stop_now();
}

void ImageDownloader::stop_video_stream_server()
{
    server_running = false;
}

void ImageDownloader::client_handler(SOCKET client_socket)
{
    char buffer[1024];
    int bytesReceived = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived <= 0) {
        closesocket(client_socket);
        return;
    }

    std::string responseHeader =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: multipart/x-mixed-replace;boundary=" + BOUNDARY + "\r\n"
        "Cache-Control: no-cache\r\n"
        "Connection: close\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "\r\n";

    send(client_socket, responseHeader.c_str(), static_cast<int>(responseHeader.length()), 0);
    while (server_running) {
        cv::Mat frame = this->get_video_frame();
        if (frame.empty()) {
            continue;
        }

        std::vector<uchar> frame_buffer;
        cv::imencode(".mjpeg", frame, frame_buffer);

        std::string frameHeader =
            "--" + BOUNDARY + "\r\n"
            "Content-Type: image/jpeg\r\n"
            "Content-Length: " + std::to_string(frame_buffer.size()) + "\r\n"
            "\r\n";

        int result = send(client_socket, frameHeader.c_str(), static_cast<int>(frameHeader.length()), 0);
        if (result == SOCKET_ERROR) break;

        result = send(client_socket, reinterpret_cast<const char*>(frame_buffer.data()), static_cast<int>(frame_buffer.size()), 0);
        if (result == SOCKET_ERROR) break;

        std::string frameFooter = "\r\n";
        result = send(client_socket, frameFooter.c_str(), static_cast<int>(frameFooter.length()), 0);
        if (result == SOCKET_ERROR) break;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    closesocket(client_socket);
}

bool ImageDownloader::start_video_stream_server()
{
    server_running = true;
    thread_pool_.submit([this] ()
    {
        while (server_running)
        {
            sockaddr_in clientAddr{};
            int addrLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);

            if (clientSocket != INVALID_SOCKET) {
                thread_pool_.submit([clientSocket, this] ()
                {
                    client_handler(clientSocket);
                });
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    return true;
}

cv::Mat ImageDownloader::get_video_frame()
{
    if (video_frame_queue_.empty())
    {
        return {};
    }
    auto frame = std::move(video_frame_queue_.front());
    video_frame_queue_.pop();
    return frame;
}

void ImageDownloader::start_video_from_esp32(const std::string& url, std::vector<char>& buffer)
{
    int maxRetries = 30;
    int retryCount = 0;
    CURL* curl = nullptr;
    CURLcode res;

    if (retryCount < maxRetries) {
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &ImageDownloader::write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 1L);
            curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 3000L);

            res = curl_easy_perform(curl);

            if (res == CURLE_OK) {
                std::cout << "CURL request successful" << std::endl;
                curl_easy_cleanup(curl);
                return;
            }
            else {
                std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
                retryCount++;
                std::cout << "Retrying (" << retryCount << "/" << maxRetries << ")..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));  // ??? 1 ???????
            }

            curl_easy_cleanup(curl);
        }
        else {
            std::cerr << "Failed to initialize CURL." << std::endl;
        }
    }

    if (retryCount == maxRetries) {
        std::cerr << "Failed to download image after " << maxRetries << " retries." << std::endl;
    }
}

void ImageDownloader::update_video_frame(cv::Mat frame)
{
    video_frame_queue_.push(std::move(frame));
}

size_t ImageDownloader::write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    static std::vector<char> residualBuffer; // ????????????
    const size_t totalSize = size * nmemb;
    std::vector<char> buffer = *static_cast<std::vector<char>*>(userp);

    // ???????????
    if (!residualBuffer.empty()) {
        buffer.insert(buffer.begin(), residualBuffer.begin(), residualBuffer.end());
        residualBuffer.clear();
    }

    // ?????????????????
    {
        std::lock_guard<std::mutex> lock(bufferMutex);
        buffer.insert(buffer.end(),
            static_cast<char*>(contents),
            static_cast<char*>(contents) + totalSize);
    }

    while (true) {
        auto boundaryStart = std::ranges::search(buffer, BOUNDARY_HEADER).begin();

        if (boundaryStart == buffer.end()) break;

        auto boundaryEnd = std::search(
            boundaryStart + BOUNDARY_HEADER.size(), buffer.end(),
            BOUNDARY_END.begin(), BOUNDARY_END.end()
        );

        if (boundaryEnd == buffer.end()) {
            residualBuffer.assign(boundaryStart, buffer.end());
            buffer.clear();
            break;
        }

        auto payloadStart = std::search(
            boundaryStart, boundaryEnd,
            "\r\n\r\n", "\r\n\r\n" + 4
        );

        if (payloadStart != boundaryEnd) {
            payloadStart += 4;
            std::vector<char> frameData(payloadStart, boundaryEnd);
            {
                std::lock_guard<std::mutex> lock(bufferMutex);
                imageBuffer.swap(frameData);
            }
            if (imageBuffer.size() > 5 * 1024 * 1024) {
                std::lock_guard<std::mutex> lock(bufferMutex);
                imageBuffer.clear();
            }
            // convert frame data(gray) to cv::Mat
            cv::Mat frame;
            frame = cv::imdecode(imageBuffer, cv::IMREAD_COLOR);
            update_video_frame(frame);
        }

        // ?????????
        buffer.erase(buffer.begin(), boundaryEnd + BOUNDARY_END.size());
    }

    return totalSize;

}






