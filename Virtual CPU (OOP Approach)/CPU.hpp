#ifndef CPU_HPP
#define CPU_HPP
#include <vector>
#include <iostream>
#include <cstdlib>

class Memory;
class Instruction;

class CPU {
    private:
        static const int NUM_REGISTERS = 6;
        int registers[NUM_REGISTERS];

        int program_counter;    //Which line of the program is gonna be executed
        bool halted;

        int option; //Keeps the programs running option

        void check_register_index(int reg) const {
            if(reg < 1 || reg > NUM_REGISTERS) {
                std::cout << "Invalid register index! Terminating the program..." << std::endl;
                exit(1);
            }
        }
    public:
        CPU(int run_option);

        int get_register(int reg);
        void set_register(int reg, int value);
        void print_all_registers();

        int get_counter() const;
        void set_program_counter(int counter);

        bool is_halted() const;
        void set_halt(bool status);

        int get_option() const;
        int get_reg_num() const;

        bool execute_program(std::vector<Instruction*>& instructions, Memory& memory);
};

#endif