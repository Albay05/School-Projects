#ifndef ABSTRACT_HPP
#define ABSTRACT_HPP

#include <iostream>
#include "Cell.hpp"

namespace cfgame {
    class ConnectFourAbstract {
        protected:
            Cell** gameboard; // The board for the gameplay
            GameMode mode;  // PvP or PvC
            
            int width;
            int height;
            bool is_game_over;

            void printBoard();
            void terminate();
            
            virtual void play(); // Computer's move
            virtual void play(int col, CellState player_state); // Player's move
            virtual bool check_win(int row, int col, CellState player_state) = 0; // Checks if the user wins
        public:
            virtual void playGame(); // Virtual function to manipulate the playing process
            virtual void set_gamemode(GameMode g_mode) {
                this->mode = g_mode;
            }

            void printToFile(std::ostream & file); // Prints to given file
            void readBoard(std::fstream & file); // Reads the board from given file
            bool is_move_possible(int col);// Checks if there is any empty cell in the column
            int possible_row(int col);// Finds the empty cell in the column


            int getWidth();
            int getHeight();

            virtual ~ConnectFourAbstract(); // Destructor to delete dynamically allocated data 
    }; // class ConnectFourAbstract
} // namespace cfgame

#endif