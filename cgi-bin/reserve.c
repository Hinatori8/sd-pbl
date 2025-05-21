/* reserve.c ── 予約 CGI: 同一コマは人数“更新”で上書き */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "schedule.h"

/* reservation.c のシンボル */
extern Reservation reservations[];
extern int reservation_count;
void load_reservations(void);
void save_reservations(void);

static void respond(int ok, const char *msg)
{
    printf("Content-Type: application/json\r\n\r\n");
    if (ok) printf("{\"success\":true}");
    else    printf("{\"success\":false,\"message\":\"%s\"}", msg ? msg : "");
}

static int valid_uid(const char *s)
{
    if (!s || s[0] != 'g') return 0;
    for (int i = 1; i < 9; ++i)
        if (!isdigit((unsigned char)s[i])) return 0;
    return s[9] == '\0';
}

int main(void)
{
    /* POST 受信 */
    int len = atoi(getenv("CONTENT_LENGTH") ?: "0");
    char *body = malloc(len + 1);
    fread(body, 1, len, stdin); body[len] = 0;

    char user[32], room_id[32];
    int day, period, people;
    if (sscanf(body,
        "{\"user\":\"%31[^\"]\",\"room\":\"%31[^\"]\","
        "\"day\":%d,\"period\":%d,\"people\":%d}",
        user, room_id, &day, &period, &people) != 5 ||
        !valid_uid(user) || people <= 0) {
        free(body); respond(0, "bad request"); return 0;
    }
    free(body);

    load_schedule();
    load_reservations();

    /* 予約処理 */
    for (int i = 0; i < room_count; ++i) {
        if (strcmp(rooms[i].id, room_id)) continue;
        int p = period - 1;

        /* 上書き or 新規の判定 */
        for (int k = 0; k < reservation_count; ++k) {
            Reservation *r = &reservations[k];
            if (!strcmp(r->user, user) && !strcmp(r->room, room_id)
                && r->day == day && r->period == period) {

                /* ---- 人数変更ルート ---- */
                int new_total = rooms[i].reserved_ppl[p][day] - r->ppl + people;
                if (new_total > rooms[i].capacity) {
                    respond(0, "定員オーバー"); return 0;
                }
                rooms[i].reserved_ppl[p][day] = new_total;
                r->ppl = people;
                save_schedule();
                save_reservations();
                respond(1, NULL);
                return 0;
            }
        }

        /* ---- 新規予約ルート ---- */
        if (rooms[i].reserved_ppl[p][day] + people > rooms[i].capacity) {
            respond(0, "定員オーバー"); return 0;
        }

        rooms[i].reserved_ppl[p][day] += people;
        rooms[i].used[p][day] = true;
        Reservation nr = {0};
        strncpy(nr.user, user, sizeof(nr.user) - 1);
        strncpy(nr.room, room_id, sizeof(nr.room) - 1);
        nr.day = day; nr.period = period; nr.ppl = people;
        reservations[reservation_count++] = nr;

        save_schedule();
        save_reservations();
        respond(1, NULL);
        return 0;
    }
    respond(0, "room not found");
    return 0;
}
