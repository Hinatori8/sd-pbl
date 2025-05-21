/* cancel_reservation.c ── 自分の予約だけをキャンセル */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "schedule.h"

/* reservation.c */
extern Reservation reservations[];
extern int  reservation_count;
void load_reservations(void);
void save_reservations(void);

static int valid_uid(const char *s)
{
    /* g + 8 桁数字のみ許可 */
    if (!s || s[0] != 'g') return 0;
    for (int i = 1; i < 8; ++i)
        if (!isdigit((unsigned char)s[i])) return 0;
    return s[8] == '\0';   /* ちょうど 8 文字で終端 */
}

static void respond(int ok, const char *msg)
{
    printf("Content-Type: application/json; charset=UTF-8\r\n\r\n");
    if (ok) printf("{\"success\":true}");
    else    printf("{\"success\":false,\"message\":\"%s\"}", msg ? msg : "");
}

int main(void)
{
    /* 1) POST 本文取得 */
    const char *cl = getenv("CONTENT_LENGTH");
    if (!cl) { respond(0, "No Content-Length"); return 0; }
    int len = atoi(cl);
    char *body = malloc(len + 1);
    if (!body) { respond(0, "malloc failed"); return 0; }
    if (fread(body, 1, len, stdin) != (size_t)len) {
        free(body); respond(0, "read error"); return 0;
    }
    body[len] = '\0';

    /* 2) JSON パース */
    char user[32], room[32]; int day, period;
    if (sscanf(body,
        "{\"user\":\"%31[^\"]\",\"room\":\"%31[^\"]\","
        "\"day\":%d,\"period\":%d}",
        user, room, &day, &period) != 4 ||
        !valid_uid(user)) {
        free(body); respond(0, "Invalid JSON"); return 0;
    }
    free(body);

    /* 3) データロード */
    load_schedule();
    load_reservations();
    auto_free_passed_periods();

    /* 4) Reservation 配列検索 */
    for (int k = 0; k < reservation_count; ++k) {
        Reservation *r = &reservations[k];
        if (!strcmp(r->user, user) && !strcmp(r->room, room)
            && r->day == day && r->period == period) {

            /* rooms[] から人数を減算 */
            for (int i = 0; i < room_count; ++i)
                if (!strcmp(rooms[i].id, room)) {
                    int p = period - 1;
                    rooms[i].reserved_ppl[p][day] -= r->ppl;
                    if (rooms[i].reserved_ppl[p][day] <= 0) {
                        rooms[i].reserved_ppl[p][day] = 0;
                        rooms[i].used[p][day] = false;
                    }
                    if (rooms[i].reserved_count > 0)
                        rooms[i].reserved_count--;
                }

            /* Reservation 配列から削除 (詰める) */
            memmove(r, r + 1,
                (reservation_count - k - 1) * sizeof(*r));
            reservation_count--;

            /* 保存 & 応答 */
            save_schedule();
            save_reservations();
            respond(1, NULL);
            return 0;
        }
    }
    respond(0, "Record not found");
    return 0;
}
