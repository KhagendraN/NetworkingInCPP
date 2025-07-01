#include <iostream>
#include <string>

int main() {
    std::string base = "192.168.1."; 

    // Loop from 1 to 254: skips .0 (network) and .255 (broadcast)
    for (int i = 1; i < 255; ++i) {
        std::cout << base << i << std::endl;
    }

    return 0;
}
