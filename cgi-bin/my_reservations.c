/* my_reservations.c ── 自分の予約だけ JSON で返す */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schedule.h"

/* reservation.c で定義 */
extern Reservation reservations[];
extern int  reservation_count;
void load_reservations(void);

static void print_json(const char *uid)
{
    printf("{\"reservations\":[");
    int first = 1;
    for (int k = 0; k < reservation_count; ++k) {
        Reservation *r = &reservations[k];
        if (strcmp(r->user, uid) != 0) continue;          /* ← フィルタ */
        if (!first) putchar(',');
        printf("{\"room\":\"%s\",\"day\":%d,\"period\":%d,\"people\":%d}",
               r->room, r->day, r->period, r->ppl);
        first = 0;
    }
    puts("]}");
}

int main(void)
{
    /* ---------- POST で渡された UID を取得 ---------- */
    char uid[32] = "";
    const char *cl = getenv("CONTENT_LENGTH");
    if (cl) {
        int len = atoi(cl);
        char *body = malloc(len + 1);
        if (body && fread(body, 1, len, stdin) == (size_t)len) {
            body[len] = '\0';
            sscanf(body, "{\"user\":\"%31[^\"]\"}", uid);
        }
        free(body);
    }

    /* バックアップ手段：Cookie から UID=... を拾う（任意） */
    if (uid[0] == '\0') {
        const char *ck = getenv("HTTP_COOKIE");
        if (ck) sscanf(ck, "UID=%31[^;]", uid);
    }

    printf("Content-Type: application/json; charset=UTF-8\r\n\r\n");

    if (uid[0] == '\0') {         /* 未ログインなら空配列を返す */
        puts("{\"reservations\":[]}");
        return 0;
    }

    load_reservations();
    print_json(uid);
    return 0;
}
