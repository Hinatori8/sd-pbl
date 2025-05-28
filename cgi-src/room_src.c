#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schedule.h"

// 指定された教室の情報を JSON 形式で出力する
static void print_room_json(const Room *r) {
    printf("{");
    // 教室ID
    printf("\"id\":\"%s\"", r->id);
    // 定員
    printf(",\"capacity\":%d", r->capacity);
    // 設備リスト（ビットマスク）
    printf(",\"features\":[");
    int first = 1;
    for (int f = 0; f < FEAT_MAX; ++f) {
        if (r->features & (1u << f)) {
            if (!first) printf(",");
            printf("\"%s\"", FeatureNames[f]);
            first = 0;
        }
    }
    printf("]");
    // 机・充電情報
    printf(",\"desk\":\"%s\"", DeskTypeNames[r->desk]);
    printf(",\"charge\":\"%s\"", ChargeTypeNames[r->charge]);

    // ── ここから機器情報 ──
    printf(",\"wired_mic\":%d",    r->wired_mic);
    printf(",\"wireless_mic\":%d", r->wireless_mic);
    printf(",\"tv\":%d",           r->tv);
    printf(",\"bd\":%d",           r->bd);
    printf(",\"dvd\":%d",          r->dvd);
    printf(",\"vhs\":%d",          r->vhs);
    printf(",\"od\":%d",           r->od);
    printf(",\"pc\":%d",           r->pc);
    printf(",\"mic\":%d",          r->mic);
    printf(",\"hdmi\":%d",         r->hdmi);
    printf(",\"webcam\":%d",       r->webcam);
    printf(",\"mic_input\":%d",    r->mic_input);
    // 画像ギャラリー（img_count と img_paths[] を使う）
    printf(",\"images\":[");
    for (int k = 0; k < r->img_count; ++k) {
      if (k) printf(",");
      printf("\"%s\"", r->img_paths[k]);
    }
    printf("]");
   // --- 既存の used 出力はそのまま ---
   // initial_used と動的予約 used を OR して出力
   printf(",\"used\":[");
   for (int p = 0; p < PERIOD_MAX; ++p) {
     if (p) printf(",");
     printf("[");
     for (int d = 0; d < DAY_MAX; ++d) {
       if (d) printf(",");
       bool busy = r->initial_used[p][d] || r->used[p][d];
       printf(busy ? "1" : "0");
     }
     printf("]");
   }
   printf("]");

  // 定員
  printf(",\"capacity\":%d", r->capacity);

  // 予約人数（ppl）
  printf(",\"ppl\":[");
  for (int p = 0; p < PERIOD_MAX; ++p) {
    if (p) printf(",");
    printf("[");
    for (int d = 0; d < DAY_MAX; ++d) {
      if (d) printf(",");
      printf("%d", r->reserved_ppl[p][d]);
    }
    printf("]");
  }
  printf("]");
   // 静的講義ブロック（初期使用）情報
   printf(",\"initial_used\":[");
   for (int p = 0; p < PERIOD_MAX; ++p) {
     if (p) printf(",");
     printf("[");
     for (int d = 0; d < DAY_MAX; ++d) {
       if (d) printf(",");
       printf(r->initial_used[p][d] ? "1" : "0");
     }
     printf("]");
   }
   printf("]");
 
  // 予約率（percent）
  printf(",\"percent\":[");
  for (int p = 0; p < PERIOD_MAX; ++p) {
    if (p) printf(",");
    printf("[");
    for (int d = 0; d < DAY_MAX; ++d) {
      if (d) printf(",");
        // フリー(used=false)なら0%、それ以外は人数÷定員
        int pct = (!r->initial_used[p][d] && !r->used[p][d]) || r->capacity == 0
                  ? 0
                  : (r->reserved_ppl[p][d] * 100) / r->capacity;
      printf("%d", pct);
    }
    printf("]");
  }
  printf("]");

    // 予約数
    printf(",\"reserved_count\":%d", r->reserved_count);
    // JSON終了
    printf("}");
}

int main(void) {
    // クエリから教室ID取得
    const char *qs = getenv("QUERY_STRING");
    char room_id[32];
    if (!qs || sscanf(qs, "room=%31[^&]", room_id) != 1) {
        printf("Content-Type: application/json; charset=UTF-8\r\n\r\n");
        printf("{\"error\":\"Invalid parameters\"}");
        return 0;
    }
    
    //データロード、自動解放、永続化
    load_schedule();
    auto_free_passed_periods();
    save_schedule();
    load_schedule();


    // 対象教室を検索
    for (int i = 0; i < room_count; ++i) {
        if (strcmp(rooms[i].id, room_id) == 0) {
            printf("Content-Type: application/json; charset=UTF-8\r\n\r\n");
            print_room_json(&rooms[i]);
            return 0;
        }
    }

    // 教室未発見
    printf("Content-Type: application/json; charset=UTF-8\r\n\r\n");
    printf("{\"error\":\"Room not found\"}");
    return 0;
}
