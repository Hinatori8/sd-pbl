#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schedule.h"

// schedule.c 側で実装済み
void load_schedule(void);
void save_schedule(void);
void auto_free_passed_periods(void);
extern Room rooms[];
extern const int room_count;

// JSON レスポンスを生成
static void respond(int ok, const char *msg) {
    printf("Content-Type: application/json; charset=UTF-8\r\n\r\n");
    if (ok) {
        printf("{\"success\":true}");
    } else {
        printf("{\"success\":false,\"message\":\"%s\"}", msg);
    }
}

int main(void) {
    // 1) Content-Length の取得
    const char *cl_env = getenv("CONTENT_LENGTH");
    if (!cl_env) {
        respond(0, "Content-Length がありません");
        return 0;
    }
    int content_length = atoi(cl_env);
    if (content_length <= 0) {
        respond(0, "Content-Length が不正です");
        return 0;
    }

    // 2) リクエストボディの読み込み
    char *body = malloc(content_length + 1);
    if (!body) {
        respond(0, "メモリ確保エラー");
        return 0;
    }
    size_t nread = fread(body, 1, content_length, stdin);
    if (nread != (size_t)content_length) {
        free(body);
        respond(0, "リクエスト読み込みエラー");
        return 0;
    }
    body[content_length] = '\0';

    // 3) JSON の簡易パース
    char room_id[32];
    int day, period, people;
    if (sscanf(body,
        "{\"room\":\"%31[^\"]\",\"day\":%d,\"period\":%d,\"people\":%d}",
        room_id, &day, &period, &people) != 4) {
        free(body);
        respond(0, "Invalid JSON");
        return 0;
    }
    free(body);

    // 4) 既存予約データの読み込み
    load_schedule();
    auto_free_passed_periods();

    // 5) 予約処理
    for (int i = 0; i < room_count; ++i) {
        if (strcmp(rooms[i].id, room_id) == 0) {
            // 範囲チェック
            if (day < 0 || day >= DAY_MAX || period < 1 || period > PERIOD_MAX) {
                respond(0, "Out of range");
                return 0;
            }

            // まず「静的ブロックされている時間」かを判定
            if (rooms[i].initial_used[period-1][day]) {
                respond(0, "この時間は予約できません");
                return 0;
            }

            // 続いて既に予約済みかどうか
            if (rooms[i].used[period-1][day]) {
                respond(0, "既に予約済みです");
                return 0;
            }

            // 予約をマーク & 人数を記録
            rooms[i].used[period-1][day] = true;
            rooms[i].reserved_ppl[period-1][day] = people;
            rooms[i].reserved_count += 1;
            save_schedule();
            respond(1, NULL);
            return 0;
        }
    }

    // 教室が見つからない場合
    respond(0, "Room not found");
    return 0;
}
