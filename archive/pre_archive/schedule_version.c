// schedule_version.c
#define SCHEDULE_FILE "/home/g2431050/vscode/work/SD_PBL/data/schedule.dat"
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include "schedule.h"

int main(void) {
    struct stat st;
    if (stat(SCHEDULE_FILE, &st) != 0) {
        // エラー時はゼロを返す
        printf("Content-Type: application/json; charset=UTF-8\r\n\r\n");
        printf("{\"version\": 0}\n");
        return 0;
    }
    // mtime を整数で返す
    printf("Content-Type: application/json; charset=UTF-8\r\n\r\n");
    printf("{\"version\": %ld}\n", (long)st.st_mtime);
    return 0;
}
