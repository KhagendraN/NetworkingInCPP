#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define PORT 6060
#define BACKLOGS 10

using namespace std;

// function to create socket 
int create_socket(){
    int socket_fd = socket(AF_INET, SOCK_STREAM, AF_UNSPEC);
    if (socket_fd == -1) {
        perror("[X] Socket creation failed!");
        exit(1);
        return  0;
    
    }else {
        perror("[✔] Socket Created successfully!");
        return socket_fd;
    }
}

// function to bind socket with port 
bool bind_socket(int socket_fd, struct sockaddr_in &address){
    if (bind(socket_fd, (struct sockaddr *)&address, sizeof address) !=-1) {
        return true;
    }else {
        perror("[X] Bind failed!");
        close(socket_fd);
        return false;
    }
}

// function to listen for request 
bool listen_request(int socket_fd){
    if (listen(socket_fd, BACKLOGS) != -1) {
        return true;
    }else {
        perror("[X] Listening failed!");
        close(socket_fd);
        return false;
    }
}

// function to accept request 
int accept_request(int server_fd, struct sockaddr_in &client_address){
    int client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &client_address);
    if (client_fd != -1) {
        return client_fd;
    }else {
        perror("[X] Accept failed!!");
        return 0; //false
    }
}

int main(){
    struct sockaddr_in address, client_address;
    int address_length = sizeof address;
    int server_fd;
    // creating socket 
    server_fd = create_socket();

    // configuring address structure 
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    // binding socket 
    if (bind_socket(server_fd, address)) {
        cout<<"[✔] Binding successful :-)"<<endl;
    }

    // listening to socket 
    if (listen_request(server_fd)) {
        cout<<"[*] Listening on port "<<PORT<<" ........"<<endl;
    }

   // main loop 
   while (true) {
        // accept request
        int client_fd = accept_request(server_fd, client_address);
        char client_ip_buffer[INET_ADDRSTRLEN];
        string client_address_decoded = inet_ntop(AF_INET,&client_address.sin_addr,client_ip_buffer , INET_ADDRSTRLEN);
        cout<<"[+] Client connected from IP : "<<client_address_decoded<<endl;

        // response from server 
        string http_response =
            "HTTP/1.1 301 Moved Permanently\r\n"
            "Location: https://khagendraneupane.com.np\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n";

        // sending response
        int send_status = send(client_fd, http_response.c_str(), http_response.size(), 0);
        if (send_status != -1) {
            cout<<"[✔] HTTP response completed successfully!"<<endl;
        }
        close(client_fd);
   }

   close(server_fd);
   return 0;
}
