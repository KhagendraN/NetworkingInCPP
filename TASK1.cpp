#include <cstdio>
#include <iostream>
#include <cstring>   // for memset and strncpy
#include <unistd.h>  // for close()
#include <sys/socket.h> // for socket()
#include <net/if.h>  // for struct ifreq
#include <sys/ioctl.h> // for ioctl()
#include <linux/if_packet.h> // for struct sockaddr_ll
#include <linux/if_ether.h> // for ETH_p_ALL 
#include <arpa/inet.h>  // for htons 

using namespace std;

// function to bind and create row socket 
int createAndBindRawSocket(const string& interfacename){
    int rawSock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if(rawSock < 0){
            perror("Failed to create socket :(");
            return -1;
        }
    

    // get index of all the interface (like eth0, wlan0)
    // check interface om linux using following command 
    // ip link show 

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interfacename.c_str(), IFNAMSIZ - 1);

    if(ioctl(rawSock, SIOCGIFINDEX, &ifr) < 0){
        perror("Couldn't get interface index : (");
        close(rawSock);
        return  -1;
    }

    // binding socket to the interface 
    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETH_P_ALL); // receive all protocols 
    sll.sll_ifindex = ifr.ifr_ifindex;   // interface index 

    if(bind(rawSock, (struct sockaddr*)&sll, sizeof(sll)) < 0){
        perror("Couldn't bind socket :(");
        close(rawSock);
        return -1;
    }
    
    cout<<"Socket created and bound to "<<interfacename<<endl;
    return rawSock;
}

// use example 
int main(){
    string interface = "eno1"; // replace it with actual one 
    // command to check interface is already mentioned above 

    int socketFd = createAndBindRawSocket(interface);

    if (socketFd < 0) {
        cerr<<"Failed to create raw socket"<<endl;
        return 1;
    }

    // code can be added here to receive packets

    close(socketFd);
    return 0;
}

