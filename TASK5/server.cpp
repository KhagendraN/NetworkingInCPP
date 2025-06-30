#include <cstdio>
#include <iostream>
#include <cstring>
#include <ostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#define PORT "6060"
#define BACKLOG 10

using namespace std;

int main(){
    struct sockaddr_storage their_addr;
    socklen_t their_size;
    struct addrinfo address, *result;
    int sock_fd, new_fd;

    memset(&address, 0, sizeof address);
    address.ai_family = AF_UNSPEC;
    address.ai_socktype = SOCK_STREAM;
    address.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &address, &result)) {
        cerr<<"getaddrinfo ERROR :("<<endl;
        return 1;
    }

    sock_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock_fd == -1) {
        perror("Unable to create socket :(");
        return 1;
    }

    if (bind(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
        perror("Unable to bind :(");
        close(sock_fd);
        return 1;
    }

    if (listen(sock_fd, BACKLOG) == -1) {
        perror("Unable to listen :(");
        close(sock_fd);
        return 1;
    }

    cout<<"[*] Server is listening on port "<<PORT<<endl;

    freeaddrinfo(result);

    while (true) {
        their_size = sizeof their_addr;
        new_fd = accept(sock_fd, (struct sockaddr *) &their_addr, &their_size);
        if (new_fd == -1) {
            perror("Couldn't accept the request :(");
            continue;
        }
        
        cout<<"[+] New client connected :)"<<endl;

        // message from server 
        const char *msg = "Hello, this is message from server! ";
        send(new_fd, msg, strlen(msg), 0);
        close(new_fd);
    }

    close(sock_fd);
    return 0;
}