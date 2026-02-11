#include "CPU.hpp"
#include "Memory.hpp"
#include "Instruction.hpp"
#include <iostream>
#include <vector>


CPU::CPU(int run_option) {
    set_program_counter(0);
    set_halt(false);
    this->option = run_option;
    for(int i = 1; i <= NUM_REGISTERS; i++)
        set_register(i, 0);
}

int CPU::get_register(int reg) {
    check_register_index(reg);

    return registers[reg-1];
}

void CPU::set_register(int reg, int value) {
    check_register_index(reg);

    registers[reg-1] = value;
}

void CPU::print_all_registers() {
    std::cout << "Printing all registers: ";
    for(int i = 0; i < NUM_REGISTERS; i++) 
        std::cout << "R" << i + 1 << ": " << registers[i] << " ";
    std::cout << std::endl;
}

int CPU::get_counter() const {
     return program_counter;
}

void CPU::set_program_counter(int counter) {
    program_counter = counter;
}

bool CPU::is_halted() const {
    return halted;
}

void CPU::set_halt(bool status) {
    halted = status;
}

int CPU::get_option() const {
    return this->option;
}

int CPU::get_reg_num() const {
    return this->NUM_REGISTERS;
}

bool CPU::execute_program(std::vector<Instruction*>& instructions, Memory& memory) {
    set_program_counter(0);

    if(instructions.empty()) {
        std::cout << "Error: No instruction to execute." << std::endl;
        return false;
    }

    while(!is_halted() && get_counter() < instructions.size()) {
        int current_line = get_counter();
        Instruction* current_instruction = instructions[current_line];
        int current_option = get_option();

        if(current_option == 1 || current_option == 2) {
            std::cout << "Executing Line" << current_line + 1 << ": " << current_instruction->to_string() << std::endl;
            print_all_registers();
        }
        if(current_option == 2) {
            memory.print_all_memory();
        }

        current_instruction->execute(*this, memory);

        if(current_line == get_counter())
            set_program_counter(current_line + 1);
    }
    return true;
}