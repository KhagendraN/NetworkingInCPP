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
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
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
    socklen_t client_length = sizeof client_address;
    int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_length);
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
        const char* client_address_decoded = inet_ntop(AF_INET,&client_address.sin_addr,client_ip_buffer , INET_ADDRSTRLEN);
        // checking connection 
        if (client_address_decoded != nullptr) {
            cout<<"[+] Client connected from IP : "<<client_address_decoded<<endl;
        }else {
            cout<<"[X] Failed to connect with IP : "<<client_address_decoded<<endl;
        }
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

/*
====================================================================
                🧠 HOW THIS SERVER WORKS — FLOW EXPLAINED
====================================================================

This is a simple TCP web server written in C++ using sockets. It listens
on a specified port (PORT 6060) and responds to every incoming HTTP
request with an HTTP 301 redirect to "https://khagendraneupane.com.np".

--------------------------------------------------------------------
💡 HIGH-LEVEL FLOW:
--------------------------------------------------------------------

1. ✅ Create a socket (TCP over IPv4)
2. ✅ Configure the server address (IP + port)
3. ✅ Bind the socket to that address
4. ✅ Mark the socket as passive (ready to listen for connections)
5. 🔁 Enter infinite loop:
     - Accept a new client connection
     - Log the client's IP address
     - Send HTTP 301 redirect response
     - Close the client socket
6. ❌ (optional) When loop breaks (server stops), close server socket

--------------------------------------------------------------------
🔧 FUNCTION-BY-FUNCTION EXPLANATION:
--------------------------------------------------------------------

🔹 create_socket()
    - Creates a socket using `socket()`
    - Domain: AF_INET (IPv4), Type: SOCK_STREAM (TCP), Protocol: 0 (auto-select)
    - If socket creation fails, the program exits

🔹 bind_socket()
    - Binds the socket to a server IP and port using `bind()`
    - Server IP is set to INADDR_ANY => binds to all network interfaces
    - Port is defined as PORT (6060)
    - Returns `true` if successful, otherwise prints an error and closes socket

🔹 listen_request()
    - Calls `listen()` to make the socket ready to accept connections
    - BACKLOGS = 10 → max pending connections in the queue
    - Returns `true` if successful, otherwise prints error and closes socket

🔹 accept_request()
    - Uses `accept()` to accept an incoming connection from a client
    - Returns a new socket file descriptor (`client_fd`) for that client
    - Also fills in the client's IP/port info in `client_address`

--------------------------------------------------------------------
🌐 INSIDE main()
--------------------------------------------------------------------

1. `sockaddr_in` structures are created for server and client addresses.
2. Server address is configured:
    - IPv4 (AF_INET)
    - Port is converted to network byte order using `htons()`
    - IP is set to INADDR_ANY (0.0.0.0) to accept connections from any interface

3. Socket is created and bound to the address.
4. The server begins listening for incoming connections.
5. Server enters an infinite loop:
    - Accepts a new client connection
    - Decodes and logs the client's IP address using `inet_ntop()`
    - Sends an HTTP 301 Moved Permanently response with `Location:` header
    - Closes the client socket after response is sent

6. (Optional) After breaking out of the loop, the server closes the main socket

--------------------------------------------------------------------
📤 HTTP REDIRECT RESPONSE SENT TO CLIENT:
--------------------------------------------------------------------

HTTP/1.1 301 Moved Permanently
Location: https://khagendraneupane.com.np
Content-Length: 0
Connection: close

--------------------------------------------------------------------
📌 NOTES:
--------------------------------------------------------------------

- The server is single-threaded and handles one client at a time.
- Each client connection is closed immediately after the response.
- This is a minimal and lightweight redirector — ideal for personal use or learning.

====================================================================
*/

