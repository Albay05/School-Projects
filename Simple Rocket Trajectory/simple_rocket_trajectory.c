#include <stdio.h>
#include <math.h>


void enterParameters(){                                             /* For taking the the parameters from the user */

double velocity;                                                    /* Initial velocity */
double gravity = 9.8;                                               /* Gravity parameter (default value is 9.8) */
double height;                                                      /* Initial height */
double maxHeight;                                                   /* Maximum altitude in trajectory */
double flightDur;                                                   /* Duration of flight */
int a;                                                              /* To check the return value of scanf */
printf("Enter initial velocity (m/s): ");
a = scanf("%lf", &velocity);
if(a == 0){                                                         /* Warns the user if the input is not a number  */
  printf("Please enter a valid input!\n");
  return;
}
else if (velocity < 0){                                             /* Warns the user if the input is below zero  */
  printf("You cannot enter a negative value!\n");
  return;
}

printf("Enter gravity (m/s², default 9.8): ");
a = scanf("%lf", &gravity);
if(a == 0){
  printf("Please enter a valid input!\n");
  return;
}
else if (gravity <= 0){
  printf("You need to enter a positive value!\n");
  return;
}

printf("Enter launch height (m): ");
a = scanf("%lf", &height);
if(a == 0){
  printf("Please enter a valid input!\n");
  return;
}
else if (height < 0){
  printf("You cannot enter a negative value!\n");
  return;
}

maxHeight = ((velocity*velocity)/(2*gravity))+height;             /* Calculates the maximum height using the formula */
flightDur = (velocity + sqrt(velocity*velocity + (2*gravity*height)))/gravity;      /* Calculates the flight duration using the formula */

FILE *fptr;

fptr = fopen("rocket_data.txt","w");                              /* Creates the file for rocket data */

fprintf(fptr,"%lf %lf %lf %lf %lf", velocity, gravity, height, maxHeight, flightDur);     /* Enters the parameters to the file */

fclose(fptr);                                                     /* Closes the file */

printf("Rocket parameters saved to rocket_data.txt!\n");

}



void simulateTrajectory(){                                        /* For printing the trajectory to the console */

FILE *fptr;

fptr = fopen("rocket_data.txt","r");                              /* Opens the rocket data file in reading mode */

if(fptr==NULL){                                                   /* Warns the user if the file does not exist */
  printf("You should firstly enter the parameters from the first option!\n");
  return;
}

double velocity, gravity, height, maxHeight, flightDur;
int col, row;                                                     /* Variables for columns and rows of the graph */

fscanf(fptr,"%lf %lf %lf %lf %lf", &velocity, &gravity, &height, &maxHeight, &flightDur);     /* Reads the parameters from the file */

printf("Reading rocket parameters from file...\n");
printf("Equation: h(t) = %.1lf * t^2 + %.0lf * t + %.0lf\n", -gravity/2, velocity, height);   /* Prints the formula*/

for (row = (int)((maxHeight/10)*2+2); row >= 0; row--) {      /* Starts the row value from the maximum height and decreases , divison by 10 is because we represent
                                                                 height values by the multiples of 10 */
        if (row > 0 && row%2==1) {                            /* Prints the height value if the conditions are satisfied */
            printf("%3d |", ((row-1)/2) * 10);
        }
        else {                                                /* Prints a gap if the row is the one with the space */
            printf("     ");
        }

        for (col = 0; col <= (int)(flightDur+1); col++) {     /* Starts the column value from zero and increases until it is the flight duration + 1 */
            if (row == 0) {
              if(col%2==0 && col!=0)                          /* Prints the x values other than 0 with one gap */
                printf("%2d", col);
              else if(col == 0)                               /* Prints x=0 value 3 digits away from the y axis */
                printf("%4d", col);
              else
              printf("  ");                                   /* Prints gap if the column is odd numbered */
            }
              else if (row == 1){                             /* Prints the x axis */
                if(col%2 == 0 && col != 0)
                  printf("-|");
                else if(col == 0)
                  printf("---|");
                else
                  printf("--");
              }
            else {
                double t = col;
                double altitude = -(gravity / 2) * t * t + velocity * t + height;     /* Formula for calculating the altitude */
                int scaled_altitude = (int)(altitude / 10 + 0.5);                     /* Scales the altitude to be multiples of 10 and rounds it manually 
                                                                                         for the accuracy of the graph */

                if (scaled_altitude * 2 == row - 1) {         /* Prints "#" if the x and y values match */
                  if(col == 0)
                    printf("   #");                           /* For sketching the graph 3 digits away from the y axis */
                  else
                    printf(" #");
                } else {
                  if(col == 0)
                    printf("    ");
                  else
                    printf("  ");
                }
            }
        }
        printf("\n");                                         /* Gets to the next line */
    }
    
fclose(fptr);                                                 /* Closes the file */

}


void saveTrajectory(){                                        /* Saves the trajectory and some data to the file */

FILE *fptr;

fptr = fopen("rocket_data.txt","r");                          /* Opens the file in readible mode */

if(fptr==NULL){                                               /* Warns the user if the file does not exist */
  printf("You should firstly enter the parameters from the first option!\n");
  return;
}

double velocity, gravity, height, maxHeight, flightDur;
int col, row;

fscanf(fptr,"%lf %lf %lf %lf %lf", &velocity, &gravity, &height, &maxHeight, &flightDur);   /* Reads rocket data from the file */

printf("Saving trajectory data...\n");

fclose(fptr);                                                 /* Closes data file */

FILE *file;

file = fopen("trajectory.txt", "w");                              /* Opens the trajectory file to print the graph */

for (row = (int)((maxHeight/10)*2+2); row >= 0; row--) {          /* Does the same trajectory process on the file */
        if (row > 0 && row%2==1) {
            fprintf(file, "%3d |", ((row-1)/2) * 10);
        }
        else {
            fprintf(file, "     ");
        }

        for (col = 0; col <= (int)(flightDur+1); col++) {
            if (row == 0) {
              if(col%2==0 && col!=0)
                fprintf(file, "%2d", col);
              else if(col == 0) 
                fprintf(file, "%4d", col);
              else
              fprintf(file, "  ");
            }
              else if (row == 1){
                if(col%2 == 0 && col != 0)
                  fprintf(file, "-|");
                else if(col == 0)
                  fprintf(file, "---|");
                else
                  fprintf(file, "--");
              }
            else {
                double t = col;
                double altitude = -(gravity / 2) * t * t + velocity * t + height;
                int scaled_altitude = (int)(altitude / 10 + 0.5);

                if (scaled_altitude * 2 == row - 1) {
                  if(col == 0)
                    fprintf(file,"   #");
                  else
                    fprintf(file," #");
                } else {
                  if(col == 0)
                    fprintf(file, "    ");
                  else
                    fprintf(file, "  ");
                }
            }
        }
        fprintf(file, "\n");
    }
printf("Maximum altitude: %.2lf\n", maxHeight);               /* Prints the values both to the screen and to trajectory.txt */
printf("Total flight duration: %.2lf\n", flightDur);
fprintf(file, "Maximum altitude: %.2lf\n", maxHeight);
fprintf(file ,"Total flight duration: %.2lf\n", flightDur);

printf("Graph saved to trajectory.txt!\n");

fclose(file);

}

void menu(){                                                  /* Menu function to navigate the user */

char cho;                                                     /* Variable for the option */

printf("\nWelcome to the Rocket Launch Simulator!\n");        /* Infroms the user and gets the option */
printf("---------------------------------------\n");
printf("1. Enter launch parameters\n");
printf("2. Simulate rocket trajectory\n");
printf("3. Save trajectory data\n");
printf("4. Exit\n");
printf("Choice: ");
scanf(" %c", &cho);

switch(cho){                                                  /* Navigates the user to the required function based on the input */
case '1':
  enterParameters();
  break;
case '2':
  simulateTrajectory();
  break;
case '3':
  saveTrajectory();
  break;
case '4':
  printf("Terminating the simulator...\n");
  return;
  break;
default:
  printf("Please enter a valid value!\n");                   /* Warns the user if the input is not valid */
}
}




int main(){

menu();                                                     /* Starts the process by calling the menu function */

}
