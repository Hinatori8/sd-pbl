// server_time.c

// XSI 拡張のタイムゾーン機能を有効化
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>  // setenv
#include <time.h>    // tzset, time, localtime

int main(void) {
    // タイムゾーン設定
    setenv("TZ", "Asia/Tokyo", 1);
    tzset();

    time_t now = time(NULL);
    struct tm *lt = localtime(&now);

    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"time\":\"%04d-%02d-%02d %02d:%02d:%02d\"}",
           lt->tm_year + 1900,
           lt->tm_mon  + 1,
           lt->tm_mday,
           lt->tm_hour,
           lt->tm_min,
           lt->tm_sec);
    return 0;
}
