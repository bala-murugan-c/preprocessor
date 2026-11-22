#include <stdio.h>

#define PI 3.14
#define SIZE 10
#define SQUARE(x) ((x)*(x))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

// line comment
/* block
   comment */
int main()
{
    int arr[SIZE];              // simple macro
    float r = PI * SQUARE(2);   /* simple + argument macro */
    int m = MAX(3,4);
    int PIPE = 1;               // PI must not change PIPE
    printf("http://site\n");    // string with // must stay
    return 0;
}
