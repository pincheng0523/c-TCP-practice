//Key-Value TCP 伺服器練習
//功能：SET、GET、DEL、EXIT

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <mutex>

#pragma comment(lib, "ws2_32.lib") // Winsock 函式庫

#define PORT 8888
#define BUFFER_SIZE 1024

std::unordered_map<std::string, std::string> map1; // 儲存 key-value
std::mutex map_lock; // 保護map的鎖，避免多client造成衝突

// 處理單一client
void talkToClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    std::string welcome = "Welcome to MiniRedis on Windows!\n";
    send(clientSocket, welcome.c_str(), welcome.length(), 0);

    while (true) {
        memset(buffer, 0, BUFFER_SIZE); // 清空buffer

        int recvLen = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (recvLen <= 0) {
            break; // 沒有資料或連線關閉
        }

        std::string line(buffer); // 把接收到的資料轉成string
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "SET") {
            std::string key, value;
            iss >> key;
            std::getline(iss, value);
            // 去掉前面的空白
            while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) {
                value.erase(0, 1);
            }

            map_lock.lock();
            map1[key] = value;
            map_lock.unlock();

            std::string reply = "OK\n";
            send(clientSocket, reply.c_str(), reply.size(), 0);
        }
        else if (cmd == "GET") {
            std::string key;
            iss >> key;

            map_lock.lock();
            if (map1.find(key) != map1.end()) {
                std::string reply = map1[key] + "\n";
                send(clientSocket, reply.c_str(), reply.size(), 0);
            }
            else {
                std::string reply = "(nil)\n";
                send(clientSocket, reply.c_str(), reply.size(), 0);
            }
            map_lock.unlock();
        }
        else if (cmd == "DEL") {
            std::string key;
            iss >> key;

            map_lock.lock();
            if (map1.erase(key)) {
                std::string reply = "OK\n";
                send(clientSocket, reply.c_str(), reply.size(), 0);
            }
            else {
                std::string reply = "(nil)\n";
                send(clientSocket, reply.c_str(), reply.size(), 0);
            }
            map_lock.unlock();
        }
        else if (cmd == "EXIT") {
            break; //離開連線
        }
        else {
            std::string reply = "Unknown command\n";
            send(clientSocket, reply.c_str(), reply.size(), 0);
        }
    }

    closesocket(clientSocket); //關閉這個client的socket
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr;
    int addrSize = sizeof(serverAddr);

    //初始化Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSA 啟動失敗\n";
        return 1;
    }

    //建立socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Socket 建立失敗\n";
        WSACleanup();
        return 1;
    }

    //設定伺服器位址
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    //綁定socket
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "綁定失敗\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    //開始
    listen(serverSocket, 3);
    std::cout << "MiniRedis Server 啟動在 port " << PORT << "\n";

    while (true) {
        //等待client連線
        clientSocket = accept(serverSocket, (sockaddr*)&serverAddr, &addrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "接受 client 失敗\n";
            continue;
        }

        talkToClient(clientSocket); //呼叫函式處理這個client
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
