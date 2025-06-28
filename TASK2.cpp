// Implement a function that converts a MAC address from std::string format ("aa:bb:cc:dd:ee:ff") to a uint8_t[6] array.

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdint>

using namespace std;

bool macStringToBytes(const string & macStr, uint8_t macBytes[6]){
    istringstream iss(macStr);
    string byteStr;

    int i = 0;

     while (std::getline(iss, byteStr, ':')) {
        if (i >= 6 || byteStr.length() != 2)
            return false;

        try {
            macBytes[i] = static_cast<uint8_t>(stoul(byteStr, nullptr, 16));
        } catch (const exception&) {
            return false;
        }

        ++i;
    }

    return (i == 6);
}

int main() {
    string macStr;
    cout<<"Enter a mac address: "<<endl;
    cin>> macStr;
    uint8_t mac[6];

    if (macStringToBytes(macStr, mac)) {
        cout << "MAC bytes: ";
        for (int i = 0; i < 6; ++i)
            cout << hex << setw(2) << setfill('0') << static_cast<int>(mac[i]) << " ";
        cout << endl;
    } else {
        cerr << "Invalid MAC address format." << endl;
    }

    return 0;
}
