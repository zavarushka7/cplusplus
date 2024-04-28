#include <iostream>
#include <string>
#include <fstream>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
class TCPClient {
private:
    SOCKET sock;
    struct sockaddr_in serv_addr;
    string filename;

public:
    TCPClient(const char* address, int port) {
        WSADATA wsaData;
        cout << "Enter the file name: ";
        cin >> filename;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed." << std::endl;
            exit(EXIT_FAILURE);
        }

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            std::cerr << "Socket creation failed." << std::endl;
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
            std::cerr << "Invalid address: Address not supported." << std::endl;
            closesocket(sock);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "Connection Failed." << std::endl;
            closesocket(sock);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        if (send(sock, filename.c_str(), filename.size(), 0) == SOCKET_ERROR) {
            cerr << "Send failed" << endl;
        }
        else {
            cout << "Filename sent" << endl;
            char buffer[4096];
            int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
            if (bytesReceived > 0) {
                cout << "Received file content: " << endl;
                cout.write(buffer, bytesReceived);
                cout << endl;
            }
        }

        closesocket(sock);
        WSACleanup();
        cout << "Press enter to exit...";
        cin.get();
    }
};
int main() {
    TCPClient client("127.0.0.1", 8080);
    cin.get();
    return 0;
}