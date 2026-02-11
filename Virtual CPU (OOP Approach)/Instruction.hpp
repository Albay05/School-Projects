#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include<string>
class CPU;
class Memory;

class Instruction {
    protected:
        std::string original_command_line;
    public:
        virtual void execute(CPU& cpu, Memory & memory) = 0;

        virtual std::string to_string() const {
            return original_command_line;
        }

        virtual ~Instruction();
};

enum class MovType {
    REG_TO_REG,
    CONST_TO_REG,
    ADRESS_TO_REG,
    REG_TO_ADRESS,
    CONST_TO_ADRESS
};

class MoveInstruction : public Instruction {
    private:
        MovType type;
        int targetRegister;
        int sourceRegister;
        int address;
        int constant;
    public:
        MoveInstruction(MovType type, int parameter1, int parameter2);
        void execute(CPU& cpu, Memory & memory) override;
};

enum class AddSubType {
    REG_TO_REG,
    CONST_TO_REG,
    ADRESS_TO_REG
};

class AddInstruction : public Instruction {
    private:
        AddSubType type;
        int targetRegister;
        int sourceRegister;
        int address;
        int constant;
    public:
        AddInstruction(AddSubType type, int parameter1, int parameter2);
        void execute(CPU& cpu, Memory & memory) override;
};

class SubInstruction : public Instruction {
    private:
        AddSubType type;
        int targetRegister;
        int sourceRegister;
        int address;
        int constant;
    public:
        SubInstruction(AddSubType type, int parameter1, int parameter2);
        void execute(CPU& cpu, Memory & memory) override;
};

enum class JmpType {
    LINE,
    LINE_TO_REG
};

class JmpInstruction : public Instruction {
    private:
        JmpType type;
        int targetRegister;
        int line;
    public:
        JmpInstruction(JmpType type, int parameter1, int parameter2);
        void execute(CPU& cpu, Memory & memory) override;
};

enum class PrnType {
    REG,
    CONST,
    ADDRESS
};

class PrnInstruction : public Instruction {
    private:
        PrnType type;
        int reg;
        int constant;
        int address;
    public:
        PrnInstruction(PrnType type, int parameter);
        void execute(CPU& cpu, Memory & memory) override;
};

class InpInstruction : public Instruction {
    private:
        int reg;
    public:
        InpInstruction(int parameter);
        void execute(CPU& cpu, Memory& memory) override;
};

class HltInstruction : public Instruction {
    public:
        HltInstruction() {}
        void execute(CPU& cpu, Memory & memory) override;            
};

#endif