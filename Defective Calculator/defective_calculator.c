#include <stdio.h>

int main(){

float a, b, bek, dny, err, totbek = 0, totdny = 0, totper = 0;
int cnt = 0;
char sym;

printf("Enter an operation (Example: 3+2): ");		/* Takes the input from the user */
scanf("%f%c%f", &a, &sym, &b);

if (sym == '+')
{
    bek = a + b;					/* Calculates the expected result */
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;					/* Adds the expected result to the total expected results */

    dny = a - b;					/* Calculates the experimental result */
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)					/* Takes the absolute value of the error */
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    
    if (bek == 0){			/* If the expected equals 0, program gives error to prevent division by zero */
        totper += 0;
        cnt += 1;			/* Increments the counter for how many times expected equals error */
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;		/* Adds the value of the error to the total error percentage */
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
    
}

else if (sym == '-')
{
    bek = a - b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a * b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
}

else if (sym == '*')
{
    bek = a * b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a + b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
}

else if (sym == '/')
{
    bek = a / b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a - b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
}

printf("\nEnter an operation (Example: 3+2): ");
scanf("%f%c%f", &a, &sym, &b);

if (sym == '+')
{
    bek = a + b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a - b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
    
}

else if (sym == '-')
{
    bek = a - b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a * b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
}

else if (sym == '*')
{
    bek = a * b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a + b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
}

else if (sym == '/')
{
    bek = a / b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a - b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
}

printf("\nEnter an operation (Example: 3+2): ");
scanf("%f%c%f", &a, &sym, &b);

if (sym == '+')
{
    bek = a + b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a - b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
    
}

else if (sym == '-')
{
    bek = a - b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a * b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
}

else if (sym == '*')
{
    bek = a * b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a + b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
}

else if (sym == '/')
{
    bek = a / b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a - b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
}

printf("\nEnter an operation (Example: 3+2): ");
scanf("%f%c%f", &a, &sym, &b);

if (sym == '+')
{
    bek = a + b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a - b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
    
}

else if (sym == '-')
{
    bek = a - b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a * b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
}

else if (sym == '*')
{
    bek = a * b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a + b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
}

else if (sym == '/')
{
    bek = a / b;
    printf("\nExpected Result: %.2f\n", bek);
    totbek += bek;

    dny = a - b;
    printf("\nExperimental Result: %.2f\n", dny);
    totdny += dny;

    err = bek - dny;
    if (err < 0)
    {
        err *= -1;
    }
    printf("\nError: %.2f\n", err);
    if (bek == 0){
        totper += 0;
        cnt += 1;
        printf("\nCouldn't calculate the error percentage due to division by zero mistake!\n");
    }
    else {
        totper += err/bek*100;
        printf("\nError Percentage: %.2f\n", (err/bek*100));
    }
}

if (cnt == 4)				/* If all the expected value are 0 then the program gives this message */
    printf("\nCouldn't calculate the avearge error percentage due to division by zero mistake!\n");
else					/* The program calculates the average error by dividing total error percentages by the amount of operation that doesn't have the expected error value equal to 0 */
    printf("\nAverage Error Value: %.2f%%\n", totper/(4-cnt));

return 0;
}
