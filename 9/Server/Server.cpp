#include <iostream>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#include <thread>
#include <fstream>
using namespace std;
class TCPServer {
private:
    SOCKET server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    const int port = 8080;
    void handle_client(SOCKET client_socket) {
        char filename[1024] = { 0 };
        int bytes_received = recv(client_socket, filename, sizeof(filename), 0);
        if (bytes_received == SOCKET_ERROR) {
            cerr << "recv failed with error" << WSAGetLastError() << endl;
            closesocket(client_socket);
        }
        else {
            cout << "Filename received " << filename << endl;
            ifstream file(filename, ios::binary | ios::ate);
            if (!file.is_open()) {
                cerr << "Failed to open file :" << filename << endl;
            }
            else {
                streamsize size = file.tellg();
                file.seekg(0, ios::beg);
                char* buffer = new char[size];
                if (file.read(buffer, size)) {
                    send(client_socket, buffer, size, 0);
                }
                delete[] buffer;
                file.close();
            }
        }
        closesocket(client_socket);
    }
public:
    TCPServer() {
        WSADATA wsaData;
        int opt = 1;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
            exit(EXIT_FAILURE);
        }

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
            std::cerr << "setsockopt failed with error: " << WSAGetLastError() << std::endl;
            closesocket(server_fd);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
            std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
            closesocket(server_fd);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 3) == SOCKET_ERROR) {
            std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
            closesocket(server_fd);
            WSACleanup();
            exit(EXIT_FAILURE);
        }
    }
    void run() {
        std::cout << "Waiting for connections..." << std::endl;

        while (true) {
            SOCKET client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
            if (client_socket == INVALID_SOCKET) {
                std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
                continue;
            }
            thread clientThread(&TCPServer::handle_client, this, client_socket);
            clientThread.detach();
        }
    }
    ~TCPServer() {
        closesocket(server_fd);
        WSACleanup();
    }
};
int main() {
    TCPServer server;
    server.run();
    return 0;
}
