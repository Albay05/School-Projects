#ifndef MEMORY_HPP
#define MEMORY_HPP

class Memory {
    private:
        static const int MEMORY_SIZE = 100;
        unsigned char data[MEMORY_SIZE];

        void check_address(int address) const;
    public:
        Memory();

        unsigned char get_value(int address);

        void set_value(int address, unsigned char value);
        
        void print_all_memory() const;
};

#endif