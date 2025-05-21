#include <stdio.h>
#include "schedule.h"

int main(void) {
    printf("Content-Type: application/json; charset=UTF-8\r\n\r\n");
    load_schedule();
    auto_free_passed_periods();   
    save_schedule();     

    printf("{\"reservations\":[");
    int first = 1;
    for (int i = 0; i < room_count; ++i) {
        for (int p = 0; p < PERIOD_MAX; ++p) {
            for (int d = 0; d < DAY_MAX; ++d) {
                // initial_used で講義ブロック分を除外
                if (rooms[i].used[p][d] && !rooms[i].initial_used[p][d]) {
                    if (!first) printf(",");
                    printf(
                      "{\"id\":\"%s\",\"day\":%d,\"period\":%d}",
                      rooms[i].id, d, p+1
                    );
                    first = 0;
                }
            }
        }
    }
    printf("]}");
    return 0;
}
