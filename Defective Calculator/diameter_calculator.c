#include <stdio.h>

int main(){

float cap, rad;

printf("Enter the diameter of a circle: ");		/* Takes the diameter as an input */
scanf("%f", &cap);
rad = cap / 2;						/* Calculates the radius */

printf("r = %.1f", rad);
printf("\nArea of sphere: %.2f\n", (3.14*4*rad*rad));	/* Calculates the area using the formula */

return 0;

}
