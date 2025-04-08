// Linux 版 Mini Redis TCP Server
// 功能：SET、GET、DEL、EXIT

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <mutex>
#include <cstring>         // for memset
#include <unistd.h>        // for close()
#include <netinet/in.h>    // for sockaddr_in
#include <sys/socket.h>    // for socket functions

#define PORT 8888
#define BUFFER_SIZE 1024

std::unordered_map<std::string, std::string> kv_store;
std::mutex store_mutex;

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    std::string welcome = "Welcome to MiniRedis on Linux!\n";
    send(client_fd, welcome.c_str(), welcome.size(), 0);

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int len = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (len <= 0) break;

        std::string input(buffer);
        std::istringstream iss(input);
        std::string cmd;
        iss >> cmd;

        if (cmd == "SET") {
            std::string key, value;
            iss >> key;
            std::getline(iss, value);
            while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) {
                value.erase(0, 1);
            }

            std::lock_guard<std::mutex> lock(store_mutex);
            kv_store[key] = value;
            send(client_fd, "OK\n", 3, 0);
        }
        else if (cmd == "GET") {
            std::string key;
            iss >> key;
            std::lock_guard<std::mutex> lock(store_mutex);
            if (kv_store.count(key)) {
                std::string reply = kv_store[key] + "\n";
                send(client_fd, reply.c_str(), reply.size(), 0);
            } else {
                send(client_fd, "(nil)\n", 7, 0);
            }
        }
        else if (cmd == "DEL") {
            std::string key;
            iss >> key;
            std::lock_guard<std::mutex> lock(store_mutex);
            if (kv_store.erase(key)) {
                send(client_fd, "OK\n", 3, 0);
            } else {
                send(client_fd, "(nil)\n", 7, 0);
            }
        }
        else if (cmd == "EXIT") {
            break;
        }
        else {
            send(client_fd, "Unknown command\n", 17, 0);
        }
    }

    close(client_fd);
}

int main() {
    int server_fd, client_fd;
    sockaddr_in server_addr{}, client_addr{};
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(server_fd, 3);
    std::cout << "MiniRedis Server started on port " << PORT << std::endl;

    while (true) {
        client_fd = accept(server_fd, (sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        handle_client(client_fd); // 單 client 版本
    }

    close(server_fd);
    return 0;
}
