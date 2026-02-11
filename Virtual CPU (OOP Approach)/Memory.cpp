#include "Memory.hpp"
#include <iostream>


Memory::Memory() {
    for(int i = 0; i < MEMORY_SIZE; i++) {
        set_value(i, 0);
    }
}

unsigned char Memory::get_value(int address) {
    check_address(address);

    return data[address];
}

void Memory::set_value(int address, unsigned char value) {
    check_address(address);

    data[address] = value;
}

void Memory::print_all_memory() const {
    using namespace std;
    cout << "Printing all the memory: ";
    for(int i = 0; i < MEMORY_SIZE; i++) {
        cout << "M" << i+1 << ":" << static_cast<int>(data[i]) << " ";
    }
    cout << endl;
}

void Memory::check_address(int address) const {
    if(address < 0 || address > 99) {
        std::cout << "Invalid address value, exiting the program..." << std::endl;
        exit(1);
    }
}