#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;


int menu(string filename, int option);
int implement_func(string line, int line_count, int* memory, int& index, int option);
bool isRegister(const string& s);
bool isConstant(const string& s);
bool is_LineAdress(const string& s, int line_count);
void move_func(vector<string> tokens, int* memory, int option);
void add_func(vector<string> tokens, int* memory, int option);
void sub_func(vector<string> tokens, int* memory, int option);
void jump_func(vector<string> tokens, int* memory, int option, int& index);
void print_func(vector<string> tokens, int* memory, int option);
void input_func(vector<string> tokens, int* memory, int option);
void halt_func(int* memory);
vector<string> tokenize(const string& line);


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
                if(option != 0 && option != 1) {
                    cout << "Error: The option must be 0 or 1!" << endl;
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


void move_func(vector<string> tokens, int* memory, int option) {
    /* Checks the parameter types and moves the values accordingly */
    if(isConstant(tokens[2]) == 1) {
        memory[(tokens[1][1] - '0') - 1] = stoi(tokens[2]);
        if(option == 1) {
            cout << tokens[0] << " " << tokens[1] << ", " << tokens[2] << " - ";
            for(int i = 0; i < 6; i++) {
                cout << "R" << i+1 << "=" << memory[i];
                if(i != 5) cout << ", ";
            }
            cout << endl;
        }
    }
    else {
        memory[(tokens[1][1] - '0') - 1] = memory[(tokens[2][1] - '0') - 1];
        if(option == 1) {
            cout << tokens[0] << " " << tokens[1] << ", " << tokens[2] << " - ";
            for(int i = 0; i < 6; i++) {
                cout << "R" << i+1 << "=" << memory[i];
                if(i != 5) cout << ", ";
            }
            cout << endl;
        }
    }
}


void add_func(vector<string> tokens, int* memory, int option) {
    /* Checks the parameter types and adds the values accordingly */
    if(isConstant(tokens[2]) == 1) {
        memory[(tokens[1][1] - '0') - 1] += stoi(tokens[2]);
        if(option == 1) {   /* Prints the commands details and register if asked to */
            cout << tokens[0] << " " << tokens[1] << ", " << tokens[2] << " - ";
            for(int i = 0; i < 6; i++) {
                cout << "R" << i+1 << "=" << memory[i];
                if(i != 5) cout << ", ";
            }
            cout << endl;
        }
    }
    else {
        memory[(tokens[1][1] - '0') - 1] += memory[(tokens[2][1] - '0') - 1];
        if(option == 1) {
            cout << tokens[0] << " " << tokens[1] << ", " << tokens[2] << " - ";
            for(int i = 0; i < 6; i++) {
                cout << "R" << i+1 << "=" << memory[i];
                if(i != 5) cout << ", ";
            }
            cout << endl;
        }
    }
}


void sub_func(vector<string> tokens, int* memory, int option) {
    /* Checks the parameter types and substracts the values accordingly */
    if(isConstant(tokens[2]) == 1) {
        memory[(tokens[1][1] - '0') - 1] -= stoi(tokens[2]);
        if(option == 1) {   /* Prints the commands details and register if asked to */
            cout << tokens[0] << " " << tokens[1] << ", " << tokens[2] << " - ";
            for(int i = 0; i < 6; i++) {
                cout << "R" << i+1 << "=" << memory[i];
                if(i != 5) cout << ", ";
            }
            cout << endl;
        }
    }
    else {
        memory[(tokens[1][1] - '0') - 1] -= memory[(tokens[2][1] - '0') - 1];
        if(option == 1) {
            cout << tokens[0] << " " << tokens[1] << ", " << tokens[2] << " - ";
            for(int i = 0; i < 6; i++) {
                cout << "R" << i+1 << "=" << memory[i];
                if(i != 5) cout << ", ";
            }
            cout << endl;
        }
    }
}


void jump_func(vector<string> tokens, int* memory, int option, int& index) {
    /* Checks the parameter types and jumps to the required line */
    if(tokens.size() == 3) {
        if(memory[(tokens[1][1] - '0') - 1] == 0) {
            index = stoi(tokens[2]) - 1;
            if(option == 1) {   /* Prints the commands details and register if asked to */
                cout << tokens[0] << " " << tokens[1] << ", " << tokens[2] << " - ";
                for(int i = 0; i < 6; i++) {
                    cout << "R" << i+1 << "=" << memory[i];
                    if(i != 5) cout << ", ";
                }
                cout << endl;
            }
        }
    }
    else {
        index = stoi(tokens[1]) - 1;
        if(option == 1) {
                cout << tokens[0] << " " << tokens[1] << " - ";
                for(int i = 0; i < 6; i++) {
                    cout << "R" << i+1 << "=" << memory[i];
                    if(i != 5) cout << ", ";
                }
                cout << endl;
        }
    }
}


void print_func(vector<string> tokens, int* memory, int option) {
    /* Checks the parameter types and prints the required memry data */
    if(isRegister(tokens[1])) {
        cout << memory[(tokens[1][1] - '0') - 1] << " ";
        if(option == 1) {   /* Prints the commands details and register if asked to */
            cout << endl;
            cout << tokens[0] << " " << tokens[1] << " - ";
            for(int i = 0; i < 6; i++) {
                cout << "R" << i+1 << "=" << memory[i];
                if(i != 5) cout << ", ";
            }
            cout << endl;
        }
        else cout << endl;
    }
    else {
        cout << tokens[1] << " ";
        if(option == 1) {
            cout << endl;
            cout << tokens[0] << " " << tokens[1] << " - ";
            for(int i = 0; i < 6; i++) {
                cout << "R" << i+1 << "=" << memory[i];
                if(i != 5) cout << ", ";
            }
            cout << endl;
        }
        else cout << endl;
    }
}


void input_func(vector<string> tokens, int* memory, int option) {
    /* Takes input from user and saves is to the given register */
    char ch;
    cout << "Enter a character: ";
    cin >> ch;
    memory[(tokens[1][1] - '0') - 1] = (int)ch;
    if(option == 1) {   /* Prints the commands details and register if asked to */
        cout << tokens[0] << " " << tokens[1] << " - ";
        for(int i = 0; i < 6; i++) {
            cout << "R" << i+1 << "=" << memory[i];
            if(i != 5) cout << ", ";
        }
        cout << endl;
    }
}


void halt_func(int* memory) {
    /* Prints all the regşsters and terminates the program */
    cout << "Terminating the program..." << endl;

    for(int i = 0; i < 6; i++) {
        cout << "R" << i+1 << "=" << memory[i];
        if(i != 5) cout << ", ";
    }
    cout << endl;
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


bool is_LineAdress(const string& s, int line_count) {
    /* Checks if the parameter is a valid line address */
    try {
        int value = stoi(s);
        return (value > 0 && value <= line_count);
    } catch (...) {
        return false;
    }
}


int implement_func(string line, int line_count, int* memory, int& index, int option) {
    vector<string> commands = {"MOV", "ADD", "SUB", "JMP", "PRN", "INP", "HLT"};
    
    vector<string> tokens = tokenize(line);

    /* Sends the line to the tokenizer function and checks if the firsr command is valid */
    
    string error_message = "Invalid command in the file. Exiting the program...\n";
    int match = 0, index_of_match, all_done = 0;

    for(int i = 0; i < commands.size(); i++) {  /* Finds which command is in the line */
        if(tokens[0].compare(commands[i]) == 0) {
            match = 1;
            index_of_match = i;
            break;
        }
    }

    if(!match) {
        cout << error_message;
        exit(1);
    }

    else {  /* Checks the commands parameters and send them to the relevant function if valid */
        switch(index_of_match) {
            case 0:
                if(tokens.size() != 3 || !isRegister(tokens[1]) || (!isRegister(tokens[2]) && !isConstant(tokens[2]))) {
                    cout << error_message;
                    exit(1);
                }
                move_func(tokens, memory, option);
                break;

            case 1:
                if(tokens.size() != 3 || !isRegister(tokens[1]) || (!isRegister(tokens[2]) && !isConstant(tokens[2]))) {
                    cout << error_message;
                    exit(1);
                }
                add_func(tokens, memory, option);
                break;

            case 2:
                if(tokens.size() != 3 || !isRegister(tokens[1]) || (!isRegister(tokens[2]) && !isConstant(tokens[2]))) {
                    cout << error_message;
                    exit(1);
                }
                sub_func(tokens, memory, option);
                break;

            case 3:
                if(tokens.size() == 2) {
                    if(!is_LineAdress(tokens[1], line_count)) {
                        cout << error_message;
                        exit(1); 
                    }
                }
                else if(tokens.size() == 3) {
                    if(!isRegister(tokens[1]) || !is_LineAdress(tokens[2], line_count)) {
                        cout << error_message;
                        exit(1); 
                    }
                }
                else {
                    cout << error_message;
                    exit(1);
                }
                jump_func(tokens, memory, option, index);
                break;

            case 4:
                if(tokens.size() != 2 || (!isRegister(tokens[1]) && !isConstant(tokens[1]))) {
                    cout << error_message;
                    exit(1);
                }
                print_func(tokens, memory, option);
                break;

            case 5:
                if(tokens.size() != 2 || !isRegister(tokens[1])) {
                    cout << error_message;
                    exit(1);
                }
                input_func(tokens, memory, option);
                break;

            case 6:
                halt_func(memory);
                all_done = 1;
                break;
        }
    }
    return all_done;    /* Returns if the program is over */
}


int menu(string filename, int option) {
    ifstream fptr;

    fptr.open(filename, ios::in);

    /* Opens the file and reads it line by line */

    if(!fptr.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return 1;
    }

    int memory[6] = {0};
    vector<string> lines;
    string line;

    while (getline(fptr, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    int line_count = lines.size();
    int index = 0;
    
    while(index < line_count) { /* Sends each line to the implementer function */
        int prev_index = index;
        if(implement_func(lines[index], line_count, memory, index, option))
            break;
        
        if(index == prev_index) index++;
    }

    fptr.close();
    return 1;
}