//  ARP scanner
/* Warning and Requirements

    Requires root/admin privileges to send raw packets.

    Works on Linux with libpcap.*/

/*Features

    Sends ARP requests to all hosts in a given subnet.

    Listens for ARP replies using libpcap.*/

#include <iostream>
#include <cstring>
#include <pcap.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netpacket/packet.h>

using namespace std;

struct arp_header {
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;
    uint8_t sha[6];
    uint8_t spa[4];
    uint8_t tha[6];
    uint8_t tpa[4];
};

// function to get mac address
void get_mac_address(const char* iface, uint8_t* mac) {
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(ifr.ifr_name, iface, IFNAMSIZ);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    close(fd);
    memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
}

// function to get ip address
void get_ip_address(const char* iface, uint8_t* ip) {
    struct ifreq ifr;
    // datagram socket are used aka connectionless socket - no connection required!
    int fd = socket(AF_INET, SOCK_DGRAM, 0); 
    strncpy(ifr.ifr_name, iface, IFNAMSIZ);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
    struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
    memcpy(ip, &ipaddr->sin_addr, 4);
}

void send_arp_request(int sockfd, const char* iface, uint8_t* src_mac, uint8_t* src_ip, uint8_t* dst_ip) {
    uint8_t buffer[42];
    struct ether_header* eth = (struct ether_header*)buffer;
    struct arp_header* arp = (struct arp_header*)(buffer + sizeof(struct ether_header));

    // Ethernet header
    memset(eth->ether_dhost, 0xff, 6);  // Broadcast
    memcpy(eth->ether_shost, src_mac, 6);
    eth->ether_type = htons(ETHERTYPE_ARP);

    // ARP header
    arp->htype = htons(1);       // Ethernet
    arp->ptype = htons(0x0800);  // IPv4
    arp->hlen = 6;
    arp->plen = 4;
    arp->oper = htons(1);        // ARP Request
    memcpy(arp->sha, src_mac, 6);
    memcpy(arp->spa, src_ip, 4);
    memset(arp->tha, 0x00, 6);
    memcpy(arp->tpa, dst_ip, 4);

    struct sockaddr_ll addr = {};
    addr.sll_ifindex = if_nametoindex(iface);
    addr.sll_halen = ETH_ALEN;
    memset(addr.sll_addr, 0xff, 6);

    sendto(sockfd, buffer, 42, 0, (struct sockaddr*)&addr, sizeof(addr));
}

void packet_handler(u_char* arg, const struct pcap_pkthdr* header, const u_char* packet) {
    struct ether_header* eth = (struct ether_header*)packet;
    if (ntohs(eth->ether_type) == ETHERTYPE_ARP) {
        struct arp_header* arp = (struct arp_header*)(packet + sizeof(struct ether_header));
        if (ntohs(arp->oper) == 2) {  // ARP Reply
            cout << "Found: IP " 
                 << inet_ntoa(*(in_addr*)&arp->spa) 
                 << " - MAC " 
                 << ether_ntoa((ether_addr*)arp->sha) << endl;
        }
    }
}

int main() {
    const char* iface = "wlan0";  
    /*change wlan0 to your interface name 
        - you can get interface list by running following command on linux
        - command : ip link show 
        - for reference check optput image for TASK3 on assets directory*/
    uint8_t src_mac[6], src_ip[4], target_ip[4]; 
    char errbuf[PCAP_ERRBUF_SIZE];

    get_mac_address(iface, src_mac);
    get_ip_address(iface, src_ip);

    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    // Set up libpcap for sniffing
    pcap_t* handle = pcap_open_live(iface, BUFSIZ, 0, 1000, errbuf);
    if (!handle) {
        cerr << "pcap_open_live failed: " << errbuf << endl;
        return 1;
    }

    cout << R"(
 _______  _______  _______  _        _       _________ _        _______    _______           ______   _        _______ _________      
(  ____ \(  ____ \(  ___  )( (    /|( (    /|\__   __/( (    /|(  ____ \  (  ____ \|\     /|(  ___ \ ( (    /|(  ____ \\__   __/      
| (    \/| (    \/| (   ) ||  \  ( ||  \  ( |   ) (   |  \  ( || (    \/  | (    \/| )   ( || (   ) )|  \  ( || (    \/   ) (         
| (_____ | |      | (___) ||   \ | ||   \ | |   | |   |   \ | || |        | (_____ | |   | || (__/ / |   \ | || (__       | |         
(_____  )| |      |  ___  || (\ \) || (\ \) |   | |   | (\ \) || | ____   (_____  )| |   | ||  __ (  | (\ \) ||  __)      | |         
      ) || |      | (   ) || | \   || | \   |   | |   | | \   || | \_  )        ) || |   | || (  \ \ | | \   || (         | |         
/\____) || (____/\| )   ( || )  \  || )  \  |___) (___| )  \  || (___) |  /\____) || (___) || )___) )| )  \  || (____/\   | | _  _  _ 
\_______)(_______/|/     \||/    )_)|/    )_)\_______/|/    )_)(_______)  \_______)(_______)|/ \___/ |/    )_)(_______/   )_((_)(_)(_)
                                                                                                                                      

)" << endl;
    for (int i = 1; i < 255; ++i) {
        memcpy(target_ip, src_ip, 3);
        target_ip[3] = i;
        send_arp_request(sockfd, iface, src_mac, src_ip, target_ip);
        /*small delay to avoid packet loss
            - since we are using Datagram protocol which works on 
            - User datagram protocol (UDP) with risk of data loss*/
        usleep(10000);  
    }

    // Capture replies for a few seconds
    pcap_loop(handle, 50, packet_handler, nullptr);

    pcap_close(handle);
    close(sockfd);
    return 0;
}
