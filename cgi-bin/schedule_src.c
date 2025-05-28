/* schedule_src.c  ── 検索 CGI: 自分の予約は除外し、定員未満なら表示 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schedule.h"

extern Reservation reservations[];
extern int reservation_count;
void load_reservations(void);
void load_schedule(void);

static void print_available_json(int day, int period, const char *uid)
{
    printf("{\"rooms\":[");
    int first = 1;
    int p = period - 1;

    for (int i = 0; i < room_count; ++i) {
        /* 講義で使用中なら除外 */
        if (rooms[i].initial_used[p][day]) continue;

        /* 自分の予約がある教室は除外 */
        bool mine = false;
        if (*uid) {
            for (int k = 0; k < reservation_count; ++k) {
                Reservation *r = &reservations[k];
                if (!strcmp(r->user, uid) && !strcmp(r->room, rooms[i].id)
                    && r->day == day && r->period == period) {
                    mine = true; break;
                }
            }
        }
        if (mine) continue;

        /* 定員いっぱいなら除外 */
        if (rooms[i].reserved_ppl[p][day] >= rooms[i].capacity) continue;

        /* ------ 出力 ------ */
        if (!first) putchar(',');
        printf("{\"id\":\"%s\",\"remain\":%d}",
               rooms[i].id,
               rooms[i].capacity - rooms[i].reserved_ppl[p][day]);
        first = 0;
    }
    puts("]}");
}

int main(void)
{
    /* --- クエリ取得 (?day=0&period=1&uid=g12345678) --- */
    int day = -1, period = -1;
    char uid[32] = "";
    const char *qs = getenv("QUERY_STRING");
    if (qs) {
        sscanf(qs, "day=%d&period=%d&uid=%31[^&]", &day, &period, uid);
    }
    if (day < 0 || period < 1) {
        printf("Status: 400 Bad Request\r\n\r\n");
        return 0;
    }

    load_schedule();
    load_reservations();

    printf("Content-Type: application/json; charset=UTF-8\r\n\r\n");
    print_available_json(day, period, uid);
    return 0;
}
