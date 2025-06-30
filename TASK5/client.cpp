#include <cstdio>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define SERVER_PORT "6060"
#define SERVER_ADDR "127.0.0.1" // localhost

using namespace std;

int main(){
    struct addrinfo address, *result;
    int sock_fd;
    char buffer[1024];

    memset(&address, 0, sizeof address);
    address.ai_family = AF_UNSPEC;
    address.ai_socktype = SOCK_STREAM;

    // get address information of server 
    if (getaddrinfo(SERVER_ADDR, SERVER_PORT, &address, &result) !=0) {
        cerr<<"Unable to get address info :("<<endl;
        return 1;
    }

    sock_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock_fd == -1) {
        perror("Unable to create socket :(");
        return 1;
    }

    // connecting to server
    if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
        perror("Unable to connect :(");
        close(sock_fd);
        return 1;
    }

    cout<<"[*] Connected to server :)"<<endl;

    // receiving message from server
    int bytes_received = recv(sock_fd, buffer, sizeof buffer -1, 0);
    if (bytes_received == -1) {
        perror("recv");
        close(sock_fd);
        return  1;
    }

    buffer[bytes_received] = '\0';
    cout<<"[*] Message from server: \n"<< buffer<<endl;

    freeaddrinfo(result);
    close(sock_fd);

    return 0;
}