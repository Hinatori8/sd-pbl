#include "schedule.h"
#include <stdio.h>
#include <stdlib.h>

/* DeskTypeNames, ChargeTypeNames, FeatureNames は
   room_src.c／schedule.h 側で定義済みのものを使います */

static void print_available_json(Day d, int period) {
    puts("Content-Type: application/json; charset=UTF-8\r\n\r\n");
    fputs("{\"rooms\":[", stdout);
    for (int i = 0, first = 1; i < room_count; ++i) {
        // used[時限][曜日] の順で参照す
        bool occupied = rooms[i].initial_used[period-1][d] || rooms[i].used[period-1][d];
        if (!occupied) {
            if (!first) putchar(',');
            /* オブジェクト開始 */
            printf("{\"id\":\"%s\"", rooms[i].id);

            /* features */
            printf(",\"features\":[");
            for (int f = 0, ffirst = 1; f < FEAT_MAX; ++f) {
                if (rooms[i].features & (1u<<f)) {
                    if (!ffirst) putchar(',');
                    printf("\"%s\"", FeatureNames[f]);
                    ffirst = 0;
                }
            }
            printf("]");

            /* desk / charge */
            printf(",\"desk\":\"%s\"",   DeskTypeNames[rooms[i].desk]);
            printf(",\"charge\":\"%s\"",  ChargeTypeNames[rooms[i].charge]);
            printf(",\"wired_mic\":%d",   rooms[i].wired_mic);
            printf(",\"wireless_mic\":%d", rooms[i].wireless_mic);
            /* オブジェクト終端 */
            putchar('}');
            first = 0;
        }
    }
    puts("]}");
}

int main(void) {
    load_schedule();
    auto_free_passed_periods();   // ← 追加: 古い予約を解放
    save_schedule();             // ← 追加: 解放結果をファイルに永続化
    const char *qs = getenv("QUERY_STRING");
    int day = 0, period = 0;
    if (qs) sscanf(qs, "day=%d&period=%d", &day, &period);
    if (day<0||day>=DAY_MAX||period<1||period>PERIOD_MAX) {
        puts("Status: 400 Bad Request\n");
        return 1;
    }
    print_available_json((Day)day, period);
    return 0;
}
