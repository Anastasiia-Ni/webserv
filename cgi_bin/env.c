#include <stdio.h>

int main(int argc, char **argv, char **env) {
    printf("Content-Type: text/plain\n\n");

    for (int i = 0; env[i]; i++) {
        printf("%d: %s\n", i, env[i]); // change \n on the <br>
    }
    return 0;
}