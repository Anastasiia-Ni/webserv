#include <stdio.h>
#include <time.h>

int main() 
{
    time_t now;
    printf("Content-Type: text/plain\n\n");
    
    now = time(NULL);
    printf("Hello world!\n");
    printf("%d\n", time);
    printf("%s\n", ctime(&time));
}
