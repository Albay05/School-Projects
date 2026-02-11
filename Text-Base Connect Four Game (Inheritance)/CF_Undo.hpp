#ifndef CFP_UNDO_HPP
#define CFP_UNDO_HPP

#include <iostream>
#include "CF_Plus.hpp"

namespace cfgame {
    class ConnectFourPlusUndo : public ConnectFourPlus {
        private:
            void play(std::fstream & move_file); // Play function are redefined as the moves are saved to a file
            void play(int col, CellState player_state, std::fstream & move_file);

            void pushMove(int row, int col, CellState c_state, std::fstream & move_file); // Pushes the move to a file
            void undoLastMove(const std::string & file_name); // Pops the last move from the stack in the file
        public:
            ConnectFourPlusUndo();// Default Constructor
            ConnectFourPlusUndo(int row, int col);// Constructor with row and col values
            ConnectFourPlusUndo(std::fstream& file);// Constructor to set the board from file

            void playGame() override;// Main game mechanism
            
            ~ConnectFourPlusUndo();
    };
} // namespace cfgame

#endif