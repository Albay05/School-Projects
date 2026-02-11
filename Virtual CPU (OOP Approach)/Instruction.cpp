#include "Instruction.hpp"
#include "CPU.hpp"
#include "Memory.hpp"
#include <iostream>

Instruction::~Instruction() {

}

MoveInstruction::MoveInstruction(MovType type, int parameter1, int parameter2) {
    this->type = type;
    switch(type) {
        case(MovType::REG_TO_REG): {
            targetRegister = parameter1;
            sourceRegister = parameter2;
            break;}
        case(MovType::CONST_TO_REG):{
            constant = parameter2;
            targetRegister = parameter1;
            break;}
        case(MovType::ADRESS_TO_REG):{
            address = parameter2;
            targetRegister = parameter1;
            break;}
        case(MovType::REG_TO_ADRESS):{
            sourceRegister = parameter2;
            address = parameter1;
            break;}
        case(MovType::CONST_TO_ADRESS):{
            address = parameter1;
            constant = parameter2;
            break;}
        }
}

void MoveInstruction::execute(CPU& cpu, Memory & memory) {
    switch(this->type) {
        case(MovType::REG_TO_REG):{
            int value = cpu.get_register(this->sourceRegister);
            cpu.set_register(this->targetRegister, value);
            break;}
        case(MovType::CONST_TO_REG):{
            cpu.set_register(this->targetRegister, this->constant);
            break;}
        case(MovType::ADRESS_TO_REG):{
            int value = (int)(memory.get_value(address));
            cpu.set_register(this->targetRegister, value);
            break;}
        case(MovType::REG_TO_ADRESS):{
            unsigned char value = (unsigned char)(cpu.get_register(this->sourceRegister));
            memory.set_value(this->address, value);
            break;}
        case(MovType::CONST_TO_ADRESS):{
            memory.set_value(this->address, (unsigned char)(this->constant));
            break;}
        }
}

AddInstruction::AddInstruction(AddSubType type, int parameter1, int parameter2) {
    this->type = type;
    switch (type) {
        case(AddSubType::REG_TO_REG):{
            targetRegister = parameter1;
            sourceRegister = parameter2;
            break;}
        case(AddSubType::CONST_TO_REG):{
            constant = parameter2;
            targetRegister = parameter1;
            break;}
        case(AddSubType::ADRESS_TO_REG):{
            address = parameter2;
            targetRegister = parameter1;
            break;}
    }
}

void AddInstruction::execute(CPU& cpu, Memory & memory) {
    switch(this->type) {
        case(AddSubType::REG_TO_REG):{
            int value = cpu.get_register(sourceRegister) + cpu.get_register(targetRegister);
            cpu.set_register(targetRegister, value);
            break;}
        case(AddSubType::CONST_TO_REG):{
            int value = this->constant + cpu.get_register(targetRegister);
            cpu.set_register(targetRegister, value);
            break;}
        case(AddSubType::ADRESS_TO_REG):{
            int value = memory.get_value(this->address) + cpu.get_register(targetRegister);
            cpu.set_register(targetRegister, value);
            break;}
    }
}

SubInstruction::SubInstruction(AddSubType type, int parameter1, int parameter2) {
    this->type = type;
    switch (type) {
        case(AddSubType::REG_TO_REG):{
            targetRegister = parameter1;
            sourceRegister = parameter2;
            break;}
        case(AddSubType::CONST_TO_REG):{
            constant = parameter2;
            targetRegister = parameter1;
            break;}
        case(AddSubType::ADRESS_TO_REG):{
            address = parameter2;
            targetRegister = parameter1;
            break;}
    }
}

void SubInstruction::execute(CPU& cpu, Memory & memory) {
    switch(this->type) {
        case(AddSubType::REG_TO_REG):{
            int value = cpu.get_register(targetRegister) - cpu.get_register(sourceRegister);
            cpu.set_register(targetRegister, value);
            break;}
        case(AddSubType::CONST_TO_REG):{
            int value = cpu.get_register(targetRegister) - this->constant;
            cpu.set_register(targetRegister, value);
            break;}
        case(AddSubType::ADRESS_TO_REG):{
            int value = cpu.get_register(targetRegister) - memory.get_value(this->address);
            cpu.set_register(targetRegister, value);
            break;}
    }
}

JmpInstruction::JmpInstruction(JmpType type, int parameter1, int parameter2) {
    this->type = type;
    switch(type) {
        case(JmpType::LINE_TO_REG):{
            targetRegister = parameter1;
            this->line = parameter2;
            break;}
        case(JmpType::LINE):{
        this->line = parameter1;
            break;}
    }
}

void JmpInstruction::execute(CPU& cpu, Memory & memory) {
    switch(this->type) {
        case(JmpType::LINE_TO_REG):{
            if(cpu.get_register(this->targetRegister) == 0)
                cpu.set_program_counter(this->line - 1);
            break;}
        case(JmpType::LINE):{
            cpu.set_program_counter(this->line - 1);
            break;}
    }
}

PrnInstruction::PrnInstruction(PrnType type, int parameter) {
    this->type = type;
    switch(type) {
        case(PrnType::REG):{
            this->reg = parameter;
            break;}
        case(PrnType::CONST):{
            this->constant = parameter;
            break;}
        case(PrnType::ADDRESS):{
            this->address = parameter;
            break;}
    }
}

void PrnInstruction::execute(CPU& cpu, Memory& memory) {
    switch(type) {
        case(PrnType::REG):{
            std::cout << "R" << this->reg << ": " << cpu.get_register(this->reg) << std::endl;
            break;}
        case(PrnType::CONST):{
            std::cout << this->constant << std::endl;
            break;}
        case(PrnType::ADDRESS):{
            std::cout << "M" << this->address << ": " << static_cast<int>(memory.get_value(this->address)) << std::endl;
            break;}
        }
}

InpInstruction::InpInstruction(int parameter) {
    this->reg = parameter;
}

void InpInstruction::execute(CPU& cpu, Memory& memory) {
    int value;
    std::cout << "Enter a number to insert into register " << this->reg << ":";
    std::cin >> value;
    cpu.set_register(this->reg, value);
}

void HltInstruction::execute(CPU& cpu, Memory & memory) {
    cpu.print_all_registers();

    memory.print_all_memory();

    std::cout << "Terminating the program..." << std::endl;
    cpu.set_halt(true);
}