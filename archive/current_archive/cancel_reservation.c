#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schedule.h"

static void respond(int ok, const char *msg) {
    printf("Content-Type: application/json; charset=UTF-8\r\n\r\n");
    if (ok) printf("{\"success\":true}");
    else    printf("{\"success\":false,\"message\":\"%s\"}", msg);
}

int main(void) {
    // POST 受け取り
    const char *cl = getenv("CONTENT_LENGTH");
    if (!cl) { respond(0,"No Content-Length"); return 0; }
    int len = atoi(cl);
    char *body = malloc(len+1);
    size_t r = fread(body,1,len,stdin);
    if (r != (size_t)len) {
        respond(0, "読み取りエラー");
        return 0;
    }
    body[len]=0;

    // simple parse: {"room":"XXX","day":D,"period":P}
    char room[32]; int day, period;
    if (sscanf(body, "{\"room\":\"%31[^\"]\",\"day\":%d,\"period\":%d}",
               room,&day,&period) != 3) {
        free(body); respond(0,"Invalid JSON"); return 0;
    }
    free(body);

    load_schedule();
    auto_free_passed_periods();
    save_schedule();       

    // キャンセル実行
    if (cancel_reservation(room, day, period)) {
        respond(1,NULL);
    } else {
        respond(0,"Cancel failed");
    }
    return 0;
}
