/* reserve.c ── 人数加算型の予約 CGI */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "schedule.h"

/* reservation.c で定義した変数／関数 */
extern Reservation reservations[];
extern int  reservation_count;
void load_reservations(void);
void save_reservations(void);

static int valid_uid(const char *s)
{
    /* g + 8 桁数字のみ許可 */
    if (!s || s[0] != 'g') return 0;
    for (int i = 1; i < 9; ++i)
        if (!isdigit((unsigned char)s[i])) return 0;
    return s[9] == '\0';   /* ちょうど 9 文字で終端 */
}

/* JSON レスポンス */
static void respond(int ok, const char *msg)
{
    printf("Content-Type: application/json; charset=UTF-8\r\n\r\n");
    if (ok) printf("{\"success\":true}");
    else    printf("{\"success\":false,\"message\":\"%s\"}", msg ? msg : "");
}

int main(void)
{
    /*── 1) リクエスト読み込み ──────────────────*/
    const char *cl = getenv("CONTENT_LENGTH");
    if (!cl) { respond(0, "No Content-Length"); return 0; }

    int len = atoi(cl);
    char *body = malloc(len + 1);
    if (!body) { respond(0, "malloc failed"); return 0; }
    if (fread(body, 1, len, stdin) != (size_t)len) {
        free(body); respond(0, "read error"); return 0;
    }
    body[len] = '\0';

    /*── 2) JSON パース (超簡易 sscanf) ───────────*/
    char user[32], room_id[32];
    int day, period, people;
    if (sscanf(body,
        "{\"user\":\"%31[^\"]\",\"room\":\"%31[^\"]\","
        "\"day\":%d,\"period\":%d,\"people\":%d}",
        user, room_id, &day, &period, &people) != 5) {
        free(body); respond(0, "Invalid JSON"); return 0;
    }
    free(body);
    /* 学籍番号フォーマットチェック */
    if (!valid_uid(user)){
        respond(0,"uid format error"); return 0;
    }
    if (people <= 0) { respond(0, "people must be >0"); return 0; }
    /*── 3) データロード & 自動解放 ───────────────*/
    load_schedule();
    load_reservations();
    auto_free_passed_periods();   /* 終了済みコマをリセット */

    /*── 4) 予約ロジック ───────────────────────*/
    for (int i = 0; i < room_count; ++i) {
        if (strcmp(rooms[i].id, room_id) != 0) continue;

        /* 範囲 & 講義ブロック判定 */
        if (day < 0 || day >= DAY_MAX || period < 1 || period > PERIOD_MAX) {
            respond(0, "Out of range"); return 0;
        }
        int p = period - 1;
        if (rooms[i].initial_used[p][day]) {
            respond(0, "講義で使用中です"); return 0;
        }

        /* 同一ユーザーの重複チェック */
        for (int k = 0; k < reservation_count; ++k) {
            Reservation *r = &reservations[k];
            if (!strcmp(r->user, user) && !strcmp(r->room, room_id)
                && r->day == day && r->period == period) {
                respond(0, "同一ユーザーが既に予約しています"); return 0;
            }
        }

        /* 定員チェック */
        int after = rooms[i].reserved_ppl[p][day] + people;
        if (after > rooms[i].capacity) {
            respond(0, "定員オーバー"); return 0;
        }

        /* 追加予約を反映 */
        rooms[i].used[p][day] = true;
        rooms[i].reserved_ppl[p][day] = after;
        rooms[i].reserved_count++;

        Reservation nr = {0};
        strncpy(nr.user,  user,  sizeof(nr.user)  - 1);
        strncpy(nr.room,  room_id, sizeof(nr.room) - 1);
        nr.day    = day;
        nr.period = period;
        nr.ppl    = people;
        reservations[reservation_count++] = nr;

        /* 永続化 */
        save_schedule();
        save_reservations();

        respond(1, NULL);
        return 0;
    }
    respond(0, "Room not found");
    return 0;
}
