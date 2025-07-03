#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>

#define PORT 6060
#define BACKLOGS 10

using namespace std;

class Client{
public:
    static void handle_client(int client_fd, sockaddr_in client_address) {
        char client_ip_buffer[INET_ADDRSTRLEN];
        const char* client_ip = inet_ntop(AF_INET, &client_address.sin_addr, client_ip_buffer, INET_ADDRSTRLEN);

        if (client_ip != nullptr) {
            cout << "[+] Client connected from IP: " << client_ip << endl;
        } else {
            cout << "[X] Failed to decode client IP" << endl;
        }

        string http_response =
            "HTTP/1.1 301 Moved Permanently\r\n"
            "Location: https://khagendraneupane.com.np\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n";

        int send_status = send(client_fd, http_response.c_str(), http_response.size(), 0);
        if (send_status != -1) {
            cout << "[✔] HTTP response sent successfully!" << endl;
        }

        close(client_fd);
    }
};

class Server{
private:
    int server_fd;
    sockaddr_in address;

public:
    Server() {
        // Initialize server socket
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            perror("[X] Socket creation failed!");
            exit(EXIT_FAILURE);
        } else {
            cout << "[✔] Socket created successfully!" << endl;
        }

        address.sin_family = AF_INET;
        address.sin_port = htons(PORT);
        address.sin_addr.s_addr = INADDR_ANY;

        // Bind
        if (bind(server_fd, (sockaddr*)&address, sizeof(address)) == -1) {
            perror("[X] Bind failed!");
            close(server_fd);
            exit(EXIT_FAILURE);
        } else {
            cout << "[✔] Bind successful!" << endl;
        }

        // Listen
        if (listen(server_fd, BACKLOGS) == -1) {
            perror("[X] Listen failed!");
            close(server_fd);
            exit(EXIT_FAILURE);
        } else {
            cout << "[*] Listening on port " << PORT << "..." << endl;
        }
    }

    void run() {
        while (true) {
            sockaddr_in client_address;
            socklen_t client_len = sizeof(client_address);

            int client_fd = accept(server_fd, (sockaddr*)&client_address, &client_len);
            if (client_fd == -1) {
                perror("[X] Accept failed!");
                continue;
            }

            // Spawn a new thread to handle this client
            thread client_thread(Client::handle_client, client_fd, client_address);
            client_thread.detach(); 
        }
    }

    ~Server() {
        close(server_fd);
    }
};

int main(){
    Server server;
    server.run();
    return 0;
}