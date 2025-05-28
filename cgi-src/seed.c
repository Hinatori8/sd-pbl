/* seed.c ── 初回パスワード用 4 桁乱数を返す */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>    /* getpid */

int main(void){
    srand((unsigned)time(NULL) ^ getpid());
    printf("Content-Type: text/plain\r\n\r\n%04d", rand() % 10000);
    return 0;
}
