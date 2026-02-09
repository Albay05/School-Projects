#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){

int width, height;                              /* Variable for width and the height of the board */
int doorX, doorY;                               /* Variable for the locations of the door */
int playerX, playerY;                           /* Variable for the locations of the player */
char move;                                      /* Variable for keeping the move input */                                      
srand(time(0));                                 /* For generating random numbers that are different from each other */

FILE *fptr;

fptr = fopen("game_state.txt", "r+");           /* Opens the file in r+ mode to be able to edit it */

if(fptr == NULL){
fptr = fopen("game_state.txt", "w");            /* Opens the file in writable mode to generate it if doesn't exist */

printf("Generating a new board...\n");

printf("Enter the width and the height of the board:\n");     /* Gets the size of the board from the user */
scanf("%d %d", &width, &height);

playerX = (rand() % (width - 0 + 1));                           /* Generates the coordinates of the player randomly */

playerY = (rand() % (height - 0 + 1));

doorX = (rand() % (width - 0 + 1));                             /* Generates the coordinates of the door randomly */

doorY = (rand() % (height - 0 + 1));

printf("Width: %d, Height: %d, Player: (%d,%d), Door: (%d,%d)\n", width, height, playerX, playerY, doorX, doorY);   /* Notifies the user about the parameters */

fprintf(fptr, "%d %d %d %d %d %d", width, height, playerX, playerY, doorX, doorY);        /* Prints the parameters to the file */

if(playerX == doorX && playerY == doorY){                       /* Checks if the door is reached */
printf("Congratulations! You escaped!\n");
printf("Generating a new board...\n");
remove("game_state.txt");                                       /* Deletes the file if the game is over */
return 0;
}

getchar();                                                      /* For taking the backspace left from the scanf */
}
else                                                            /* In case the file exists */
{
if(playerX == doorX && playerY == doorY){                       /* Checks if the door is reached */
printf("Congratulations! You escaped!\n");
printf("Generating a new board...\n");
remove("game_state.txt");                                       /* Deletes the file if the game is over */
return 0;
}

printf("Enter the move(WASD): ");                               /* Gets the move from the user */
scanf("%c", &move);

fscanf(fptr, "%d %d %d %d %d %d", &width, &height, &playerX, &playerY, &doorX, &doorY);  /* Reads the parameters from the file */


if(move == 'W' || move == 'w'){                                 /* Checks the move */
if(playerY < height){
printf("Player moves up to (%d,%d)\n", playerX, playerY + 1);   /* Updates the coordinates of the player if the move is not out of borders */
playerY += 1;
}
else                                                            /* Warns the user to not get out of borders */
printf("You can't get out of borders!\n");
if(playerX != doorX || playerY != doorY){                       /* Gives this message if the door is not reached */
printf("Game continues...\n");
}
}
else if(move == 'A' || move == 'a'){
if(playerX > 0){
printf("Player moves left to (%d,%d)\n", playerX - 1, playerY);
playerX -= 1;
}
else
printf("You can't get out of borders!\n");
if(playerX != doorX || playerY != doorY){
printf("Game continues...\n");
}
}
else if(move == 'S' || move == 's'){
if(playerY > 0){
printf("Player moves down to (%d,%d)\n", playerX, playerY - 1);
playerY -= 1;
}
else
printf("You can't get out of borders!\n");
if(playerX != doorX || playerY != doorY){
printf("Game continues...\n");
}
}
else if(move == 'D' || move == 'd'){
if(playerX < width){
printf("Player moves right to (%d,%d)\n", playerX + 1, playerY);
playerX += 1;
}
else
printf("You can't get out of borders!\n");
if(playerX != doorX || playerY != doorY){
printf("Game continues...\n");
}
}
else
printf("Please enter a valid input!\n");                        /* Warns the user if the input is not valid */

rewind(fptr);                                                   /* Gets the file pointer to the beginning */
fprintf(fptr, "%d %d %d %d %d %d", width, height, playerX, playerY, doorX, doorY);      /* Updates the coordinates */

if(playerX == doorX && playerY == doorY){                       /* Checks if the door is reached */
printf("Congratulations! You escaped!\n");
printf("Generating a new board...\n");
remove("game_state.txt");
return 0;
}
}
fclose(fptr);                                                   /* Closes the file */
return 0;
}
