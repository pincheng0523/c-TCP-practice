//TCP 客戶端練習
//說明：可以輸入指令SET、GET、DEL、EXIT，並與伺服器互動

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib") //連結Winsock函式庫

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET sock1;
    sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    //初始化Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup 失敗\n";
        return 1;
    }

    //建立socket
    sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock1 == INVALID_SOCKET) {
        std::cout << "建立 socket 失敗\n";
        WSACleanup();
        return 1;
    }

    //設定要連線的伺服器IP和Port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); //本機

    //嘗試連線到伺服器
    if (connect(sock1, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "連線伺服器失敗\n";
        closesocket(sock1);
        WSACleanup();
        return 1;
    }

    std::cout << "已成功連線到伺服器 (127.0.0.1:" << PORT << ")\n";

    //接收伺服器的訊息
    int recvLen = recv(sock1, buffer, BUFFER_SIZE - 1, 0);
    if (recvLen > 0) {
        buffer[recvLen] = '\0'; //字串結尾
        std::cout << buffer;
    }

    //開始輸入
    while (true) {
        std::string userInput;
        std::cout << "> ";
        std::getline(std::cin, userInput);

        if (userInput.empty()) continue; //空指令就跳過

        //傳送使用者輸入的內容給伺服器
        send(sock1, userInput.c_str(), userInput.length(), 0);

        // 如果是EXIT就結束連線
        if (userInput == "EXIT") {
            break;
        }

        //等待伺服器回應
        recvLen = recv(sock1, buffer, BUFFER_SIZE - 1, 0);
        if (recvLen <= 0) {
            std::cout << "伺服器關閉了連線\n";
            break;
        }

        buffer[recvLen] = '\0';
        std::cout << buffer; //顯示回應
    }

    //結束連線與清除
    closesocket(sock1);
    WSACleanup();
    return 0;
}
