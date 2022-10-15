#include <stdio.h>

int main(int argc, char **argv, char **env) {
    printf("Content-Type: text/plain\n\n");

    while (env) {
        printf("%s<br>", *env);
        env++;
    }
    return 0;
}