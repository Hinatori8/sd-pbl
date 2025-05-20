// reset.c
#include <stdio.h>
#include "schedule.h"   // ← ここで reset_schedule_data を宣言済みにする

int main(void) {
    // レスポンスヘッダー
    printf("Content-Type: text/plain\r\n\r\n");

    // 実際のリセット処理
    reset_schedule_data();

    printf("Schedule reset complete.\n");
    return 0;
}
