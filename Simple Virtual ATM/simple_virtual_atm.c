#include <stdio.h>
#include <stdlib.h>

double get_balance();                           /* Reads balance from file */
void update_balance(double new_balance);        /* Updates balance in the file */
void check_balance();                           /* Displays current balance */
void deposit_money();                           /* Allows user to deposit money */
void withdraw_money();                          /* Allows user to withdraw money */
void menu();                                    /* Displays the ATM menu */

int main(){

menu();                                         /* Starts the process by calling menu function */

return 0;
}

void menu(){

int opt;                                        /* To store the option */

printf("-----Virtual ATM-----\n");              /* Takes the option input from the user */
printf("1. Check Balance\n");
printf("2. Deposit Money\n");
printf("3. Withdraw Money\n");
printf("4. Exit\n");
printf("Choose an option: ");
scanf("%d", &opt);

switch(opt){                                    /* Calls the required function according to the choice */
case 1:
  check_balance();
  break;
case 2:
  deposit_money();
  break;
case 3:
  withdraw_money();
  break;
case 4:
  printf("Terminating the virtual machine...\n");
  return ;
  break;
default:
  printf("Please choose a valid option!\n");    /* Tells the user to input something valid in case it is not */
  menu();
}

}

double get_balance(){
FILE *fptr;

fptr = fopen("account.txt","r");

double balance;                                 /* To store the balance */
double initial = 100;                           /* For the initial balance */

if(fptr == NULL){                               /* Creates the file and places the initial value in it if the file does not exist */
  fptr = fopen("account.txt","w");
  fprintf(fptr,"%lf", initial);
  fclose(fptr);
  
  fptr = fopen("account.txt","r");
  fscanf(fptr, "%lf", &balance);
  fclose(fptr);
}

else{
  fscanf(fptr, "%lf", &balance);                /* Gets the balance from the file if it already exists */
  fclose(fptr);
}
return balance;                                 /* Returns the balance value */
}

void update_balance(double new_balance){

FILE *fptr;

fptr = fopen("account.txt", "r+");

fprintf(fptr, "%lf", new_balance);              /* Replaces the new balance value with the old one */

fclose(fptr);

}

void check_balance(){                           /* Checks the balance value and prints it */

double balance = get_balance();

printf("Current balance: %.2lf TRY\n", balance);

}

void deposit_money(){

double depo;                                    /* To store the deposit amount */
double current = get_balance();                 /* To get the current balance */

printf("Enter the amount to deposit: ");        /* Gets the amount to deposit */
scanf("%lf", &depo);

if(depo < 0){                                   /* Warns the user if they try to deposit a negative value */
  printf("Deposit unsuccessful, you cannot deposit a negative value!\n");
}

else{
  current += depo;                              /* Increments the current value */
  update_balance(current);                      /* Updates the value in the file */
  printf("Deposit successful! New balance: %.2lf TRY\n", current);    /* Informs the user */
}

}

void withdraw_money(){

double wdr;                                     /* To store the withdraw amount */
double current = get_balance();                 /* To get the current balance */

printf("Enter the amount to withdraw: ");       /* Gets the amount to withdraw */
scanf("%lf", &wdr);

if(wdr < 0){                                    /* Warns the user if they try to withdraw a negative value */
  printf("Withdraw unsuccessful, you cannot withdraw a negative value!\n");
}

else if(wdr > current){                         /* Warns the user if they try to withdraw an amount more than the balance */
  printf("Withdraw unsuccessful! Your balance is only %.2lf TRY\n", current);
}

else{
  current -= wdr;                               /* Decrements the current balance */
  update_balance(current);                      /* Updates the value in the file */
  printf("Withdraw successful! New balance: %.2lf TRY\n", current);   /* Informs the user */
}

}
