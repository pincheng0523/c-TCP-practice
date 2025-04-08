// Linux 版 Mini Redis TCP Client
// 功能：輸入 SET / GET / DEL / EXIT 與伺服器互動

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>        // for close()
#include <arpa/inet.h>     // for inet_pton, sockaddr_in
#include <sys/socket.h>    // for socket functions

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 建立 socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket creation failed");
        return 1;
    }

    // 設定 server 資訊
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // 連線
    if (connect(sock_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connection failed");
        return 1;
    }

    std::cout << "Connected to server on 127.0.0.1:" << PORT << std::endl;

    // 接收歡迎訊息
    int recv_len = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
    if (recv_len > 0) {
        buffer[recv_len] = '\0';
        std::cout << buffer;
    }

    // 開始互動
    while (true) {
        std::string input;
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input.empty()) continue;

        send(sock_fd, input.c_str(), input.length(), 0);

        if (input == "EXIT") break;

        memset(buffer, 0, BUFFER_SIZE);
        recv_len = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
        if (recv_len <= 0) {
            std::cout << "Server closed connection." << std::endl;
            break;
        }

        buffer[recv_len] = '\0';
        std::cout << buffer;
    }

    close(sock_fd);
    return 0;
}