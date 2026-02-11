#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "Instruction.hpp"
#include "Memory.hpp"
#include "CPU.hpp"

using namespace std;

int menu(string filename, int option);
vector<string> tokenize(const string& line);
bool isRegister(const string& s);
bool isConstant(const string& s);
bool isLineAdress(const string& s, int line_count);
bool isMemoryAddress(const string& s);
int integerize(const string& s);


int main() {
    string p_name, f_name;
    int option, valid_input = 0;

    while (true) {  /* Works until it takes valid inputs */
        cout << "Enter the command(executable name, filename and option): ";
        
        string line;
        if(!getline(cin, line)) {
            cout << "\nInput stream closed. Exiting." << endl;
            return 1;
        }

        stringstream ss(line);
        string extra_input;

        if(ss >> p_name >> f_name >> option) {
            if(ss >> extra_input) {
                cout << "Error: Too many arguments provided. Please provide exactly three." << endl;
            }
            else {
                if(option != 0 && option != 1 && option != 2) {
                    cout << "Error: The option must be 0, 1 or 2!" << endl;
                }
                else {
                    break;
                }
            }
        }
        else {
            cout << "Error: Invalid or insufficient input. Please follow the format: <executable> <filename> <option>" << endl;
        }
    }
    
    menu(f_name, option); /* Calls the menu function to organize the general progress */
        
    return 0;
}


int menu(string filename, int option) {
    ifstream fptr;

    fptr.open(filename, ios::in);

    /* Opens the file and reads it line by line */

    if(!fptr.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return 1;
    }

    vector<string> lines;
    string line;

    while (getline(fptr, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    int line_count = lines.size();

    vector<Instruction*> instructions;
    vector<vector<string>> program_tokens;

    for(int i = 0; i < line_count; i++) {
        program_tokens.push_back(tokenize(lines[i]));
    }

    for(int i = 0; i < line_count; i++) {
        string command_name = program_tokens[i][0];
        if(command_name.compare("MOV") == 0) {
            MovType type;

            if(isRegister(program_tokens[i][2]) && isRegister(program_tokens[i][1]))
                type = MovType::REG_TO_REG;

            else if(isConstant(program_tokens[i][2]) && isRegister(program_tokens[i][1]))
                type = MovType::CONST_TO_REG;

            else if(isMemoryAddress(program_tokens[i][2]) && isRegister(program_tokens[i][1]))
                type = MovType::ADRESS_TO_REG;

            else if(isRegister(program_tokens[i][2]) && isMemoryAddress(program_tokens[i][1]))
                type = MovType::REG_TO_ADRESS;

            else if(isConstant(program_tokens[i][2]) && isMemoryAddress(program_tokens[i][1]))
                type = MovType::CONST_TO_ADRESS;
            
            Instruction* current_instruction = new MoveInstruction(type, integerize(program_tokens[i][1]), integerize(program_tokens[i][2]));
            instructions.push_back(current_instruction);
        }
        else if(command_name.compare("ADD") == 0) {
            AddSubType type;

            if(isRegister(program_tokens[i][2]) && isRegister(program_tokens[i][1]))
                type = AddSubType::REG_TO_REG;

            else if(isConstant(program_tokens[i][2]) && isRegister(program_tokens[i][1]))
                type = AddSubType::CONST_TO_REG;

            else if(isMemoryAddress(program_tokens[i][2]) && isRegister(program_tokens[i][1]))
                type = AddSubType::ADRESS_TO_REG;
            
            Instruction* current_instruction = new AddInstruction(type, integerize(program_tokens[i][1]), integerize(program_tokens[i][2]));
            instructions.push_back(current_instruction);
        }
        else if(command_name.compare("SUB") == 0) {
            AddSubType type;

            if(isRegister(program_tokens[i][2]) && isRegister(program_tokens[i][1]))
                type = AddSubType::REG_TO_REG;

            else if(isConstant(program_tokens[i][2]) && isRegister(program_tokens[i][1]))
                type = AddSubType::CONST_TO_REG;

            else if(isMemoryAddress(program_tokens[i][2]) && isRegister(program_tokens[i][1]))
                type = AddSubType::ADRESS_TO_REG;
            
            Instruction* current_instruction = new SubInstruction(type, integerize(program_tokens[i][1]), integerize(program_tokens[i][2]));
            instructions.push_back(current_instruction);
        }
        else if(command_name.compare("JMP") == 0) {
            JmpType type;

            if(isLineAdress(program_tokens[i][1], line_count))
                type = JmpType::LINE;

            else if(isLineAdress(program_tokens[i][2], line_count) && isRegister(program_tokens[i][1]))
                type = JmpType::LINE_TO_REG;
            
            if(program_tokens[i].size() == 3) {
                Instruction* current_instruction = new JmpInstruction(type, integerize(program_tokens[i][1]), integerize(program_tokens[i][2]));
                instructions.push_back(current_instruction);
            }

            else if(program_tokens[i].size() == 2) {
                Instruction* current_instruction = new JmpInstruction(type, integerize(program_tokens[i][1]), -1);
                instructions.push_back(current_instruction);
            }
        }
        else if(command_name.compare("PRN") == 0) {
            PrnType type;
            if(isRegister(program_tokens[i][1]))
                type = PrnType::REG;

            else if(isConstant(program_tokens[i][1]))
                type = PrnType::CONST;

            else if(isMemoryAddress(program_tokens[i][1]))
                type = PrnType::ADDRESS;

            Instruction* current_instruction = new PrnInstruction(type, integerize(program_tokens[i][1]));
            instructions.push_back(current_instruction);
        }
        else if(command_name.compare("INP") == 0) {
            Instruction* current_instruction = new InpInstruction(integerize(program_tokens[i][1]));
            instructions.push_back(current_instruction);
        }
        else if(command_name.compare("HLT") == 0) {
            Instruction* current_instruction = new HltInstruction();
            instructions.push_back(current_instruction);
        }
        else {
            cout << "Error: Invalid command name. Terminating program..." << endl;
            exit(1);
        }
    }

    Memory new_memory;
    CPU new_cpu(option);

    new_cpu.execute_program(instructions, new_memory);

    for (Instruction* inst : instructions) {
        delete inst;
    }
    instructions.clear();

    fptr.close();
    return 1;
}


vector<string> tokenize(const string& line) {
    /* Tokenizes the given line to command strings and removes the punctuation characters */
    vector<string> tokens;
    stringstream ss(line);
    string word;

    while(ss >> word) {
        if(!word.empty() && word.back() == ',' || word.back() == ';')
            word.pop_back();
        
        tokens.push_back(word);
    }
    return tokens;
}


bool isRegister(const string& s) {
    /* Checks if the parameter is a register */
    return (s.size() == 2 && s[0] == 'R' && s[1] >= '1' && s[1] <= '6');
}


bool isConstant(const string& s) {
    /* Checks if the parameter is a constant number */
    if(s.empty()) return false;
    for(int i = 0; i < s.size(); i++) {
        if(!isdigit(s[i]) && !(i == 0 && s[i] == '-'))
            return false;
    }
    return true;
}


bool isLineAdress(const string& s, int line_count) {
    /* Checks if the parameter is a valid line address */
    try {
        int value = stoi(s);
        return (value > 0 && value <= line_count);
    } catch (...) {
        return false;
    }
}


bool isMemoryAddress(const string& s) {
    /* Checks if the parameter is a valid memory address*/
    if(s[0] != '#') return false;
    try {
        string number;
        number = s.substr(1, s.size()-1);
        int value = stoi(number);
        return (value >= 0 && value < 100);
    } catch (...) {
        return false;
    }
}

int integerize(const string& s) {
    if (s.empty() || (s.size() == 1 && (s[0] == 'R' || s[0] == '#'))) {
        return -999;
    }

    if(s[0] == 'R' || s[0] == '#') {
        return(stoi(s.substr(1)));
    }
    else return(stoi(s));
}