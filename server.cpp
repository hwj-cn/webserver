#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <fstream>

#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;


constexpr int PORT = 8080;



void read_config() {
    ifstream json_f("config.json");

    if (!json_f.is_open()) {
        cerr << "config.json didn't open!" << endl;
        exit(EXIT_FAILURE);
    }

    json j;
    json_f >> j;


    auto io_types = j["io_types"].template get<vector<string>>();
    

    cout << "io_types: ";
    for_each(io_types.begin(), io_types.end(), [](const auto& x) { cout << x << " "; });
    cout << endl;

    auto io_type_select = j["io_type_select"].template get<int>();

    assert(io_type_select >= 0 && io_type_select < io_types.size());

    cout << "io_type_select: " << io_types[io_type_select] << endl;
}

int main() {
    read_config();

    int server_fd, new_socket;

    struct sockaddr_in address;

    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // 创建 socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        cerr << "socket 创建失败" << endl;
        exit(EXIT_FAILURE);
    }


    // 设置 socket 选项，允许地址复用
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        cerr << "setsockopt 失败" << endl;
        exit(EXIT_FAILURE);
    }

    // 配置地址信息
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 绑定 socket 到指定 IP 和端口
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        cerr << "bind 失败" << endl;
        exit(EXIT_FAILURE);
    }


    // 开始监听
    if (listen(server_fd, 10) < 0) {
        cerr << "listen 失败" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "服务器启动，监听端口" << PORT << endl;
    cout << "sizeof(buffer): " << sizeof(buffer) << endl;


    while (true) {
        // 阻塞等待客户端连接
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            cerr << "accept 失败" << endl;
            exit(EXIT_FAILURE);
        }

        ssize_t bytes_read = read(new_socket, buffer, sizeof(buffer) - 1);

        if (bytes_read < 0) {
            buffer[bytes_read] = '\0';
            cout << "接收到请求: " << buffer << endl;
        }

        // 构造 HTTP 相应
        string response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: test/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Hello, World!";

        // 发送响应
        write(new_socket, response.c_str(), response.size());

        // 关闭连接
        close(new_socket);
    }

    // 关闭服务器 socket（一般不会执行到这里）
    close(server_fd);
    return 0;
}