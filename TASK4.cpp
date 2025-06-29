/*
This is a C++ example demonstrating how to receive Ethernet frames using raw sockets
and extract the source MAC address and ARP reply information on a Linux system.
 */

/*
Requirements:

    - Must run as root (raw sockets require elevated privileges).

    - Only works on Linux.

    - Must have permissions to capture packets on the interface.
*/
/*Features:

    - Opens a raw socket on the Ethernet interface.

    - Listens for Ethernet frames.

    - Filters for ARP replies.

    - Extracts and prints:

        - Source MAC address

        - Sender IP address (from ARP payload)
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <unistd.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>

using namespace std;

// function to return mac address as string
string format_mac(const uint8_t *mac) {
    ostringstream oss;
    for (int i = 0; i < 6; ++i) {
        if (i > 0) oss << ":";
        oss << hex << setw(2) << setfill('0') << static_cast<int>(mac[i]);
    }
    return oss.str();
}

/*function to return ip address as string 
    - uses inet_ntop 
    - ntop means network to presentation 
    - converts to binary to text*/
string format_ip(uint32_t ip) {
    struct in_addr ip_addr;
    ip_addr.s_addr = ip;

    char str[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &ip_addr, str, INET_ADDRSTRLEN)) {
        return string(str);
    } else {
        perror("inet_ntop");
        return string("Invalid IP");
    }
}

int main() {
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    uint8_t buffer[65536];

    while (true) {
        ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer), 0, nullptr, nullptr);
        if (len < 0) {
            perror("Failed to receive packets");
            close(sockfd);
            return 1;
        }

        struct ethhdr *eth = (struct ethhdr *)buffer;


        if (ntohs(eth->h_proto) == ETH_P_ARP) {
            struct ether_arp *arp = (struct ether_arp *)(buffer + sizeof(struct ethhdr));

            if (ntohs(arp->ea_hdr.ar_op) == ARPOP_REPLY) {
                string mac = format_mac(arp->arp_sha);
                string ip = format_ip(*(uint32_t *)arp->arp_spa);

                cout << "ARP Reply Received:" << endl;
                cout << "Source MAC: " << mac << endl;
                cout << "Sender IP : " << ip << endl;
                cout << "-----------------------------------" << endl;
            }
        }
    }

    close(sockfd);
    return 0;
}
