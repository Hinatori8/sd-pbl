#include "schedule.h"
#define _POSIX_C_SOURCE 200809L
#include<stdio.h>
#include<string.h>
#include<time.h>
#define SCHEDULE_FILE "/home/g2431050/vscode/work/SD_PBL/data/schedule.dat"

// 各時限の終了時刻（分）。0:00 からの経過分で表す。
const int PERIOD_END_MINUTES[PERIOD_MAX] = {
    /* 1限終了 */ 16 * h + 45,   
    /* 2限終了 */ 12 * h + 50,
    /* 3限終了 */ 15 * h + 20,    
    /* 4限終了 */ 17 * h + 10,    
    /* 5限終了 */ 19 * h + 0,    
};

const char *FeatureNames[FEAT_MAX] = {
  // ── 黒板系 ──
  "黒板",              // FEAT_BLACKBOARD
  "黒板(2枚上下)",     // FEAT_BLACKBOARD_DBL
  
  // ── ホワイトボード系 ──
  "ホワイトボード",    // FEAT_WHITEBOARD
  "ホワイトボード(壁面)",// FEAT_WHITEBOARD_WALL
  "グラスボード",      // FEAT_GLASSBOARD
  
  // ── ネットワーク ──
  "有線LAN"           // FEAT_LAN
};

const char *DeskTypeNames[]   = { "固定", "可動" };
const char *ChargeTypeNames[] = { "なし", "机固定", "延長コード" };
const char *SizeTypeNames[] = {"大", "中", "小"}; 
/* rooms[] に必要な数だけ記載 */
Room rooms[ROOM_MAX] = {
    {
      .id       = "1BA",
      .capacity = 42,
      .features = (1u << FEAT_BLACKBOARD) | (1u<<FEAT_LAN), //黒板とLANがあるという意味
      .desk     = MOVABLE_DESK,
      .charge   = NO_CHARGE,
      .size     = SMALL,
      .initial_used = {
        /*    1限 2限 3限 4限 5限 */
        { 0, 0, 0, 0, 0 ,0},  /* 月曜日 */
        { 0, 1, 0, 0, 0 ,0},  /* 火曜日 */
        { 0, 1, 0, 0, 0 ,0},  /* 火曜日 */
        { 0, 1, 0, 0, 0 ,0},  /* 火曜日 */
        { 0, 1, 0, 0, 0 ,0},  /* 火曜日 */
      },
      .used = {{0}},
      .reserved_count = 0,
      .wired_mic = 1,
      .wireless_mic = 1,
      .tv            = 1,
      .bd            = 1,
      .dvd           = 0,
      .vhs           = 1,
      .od            = 0,
      .pc            = 1,
      .mic           = 1,
      .hdmi          = 0,
      .webcam        = 0,
      .mic_input     = 0,
      .img_count = 2,
      .img_paths = {
        "/images/rooms/3B14/class.jpg",
        "/images/rooms/3B14/class2.jpeg"
      },
    },
    {
      .id       = "3B15",
      .capacity = 40,
      .features = (1u << FEAT_BLACKBOARD) | (1u<<FEAT_LAN), //黒板とLANがあるという意味
      .desk     = FIXED_DESK,
      .charge   = NO_CHARGE,
      .size     = MID,
      .initial_used = {
        /*    1限 2限 3限 4限 5限 */
        { 0, 0, 0, 0, 0 ,0},  /* 月曜日 */
        { 0, 1, 0, 0, 0 ,0},  /* 火曜日 */
        { 0, 1, 0, 0, 0 ,0},  /* 火曜日 */
        { 0, 1, 0, 0, 0 ,0},  /* 火曜日 */
        { 0, 1, 0, 0, 0 ,0},  /* 火曜日 */
      },
      .used = {{0}},
      .reserved_count = 0,
      .wired_mic = 1,
      .wireless_mic = 1,
      .tv            = 1,
      .bd            = 1,
      .dvd           = 1,
      .vhs           = 0,
      .od            = 1,
      .pc            = 1,
      .mic           = 1,
      .hdmi          = 0,
      .webcam        = 1,
      .mic_input     = 1,
      .img_count = 0,
      .img_paths = {},
    },
  };
  
const int room_count = 2;  /* rooms[] に登録した数 */

void save_schedule(void) {
  RoomState states[ROOM_MAX];
  for (int i = 0; i < room_count; ++i) {
      // 動的予約状態をコピー
      memcpy(states[i].used,            rooms[i].used,            sizeof(rooms[i].used));
      states[i].reserved_count         = rooms[i].reserved_count;
      memcpy(states[i].reserved_ppl,    rooms[i].reserved_ppl,    sizeof(rooms[i].reserved_ppl));
  }
  FILE *fp = fopen(SCHEDULE_FILE, "wb");
  if (!fp) {
      perror("save_schedule: fopen failed");
      return;
  }
  fwrite(states, sizeof(RoomState), room_count, fp);
  fclose(fp);
}

void load_schedule(void) {
  FILE *fp = fopen(SCHEDULE_FILE, "rb");
  if (fp) {
      // 既存ファイルから読み込み
      RoomState states[ROOM_MAX];
      size_t n = fread(states, sizeof(RoomState), room_count, fp);
      fclose(fp);
      if (n == (size_t)room_count) {
          // 正常に読み込めた場合のみ反映
          for (int i = 0; i < room_count; ++i) {
              memcpy(rooms[i].used,         states[i].used,         sizeof(rooms[i].used));
              rooms[i].reserved_count      = states[i].reserved_count;
              memcpy(rooms[i].reserved_ppl,states[i].reserved_ppl, sizeof(rooms[i].reserved_ppl));
          }
      } else {
          // 不完全読み込み時は再生成
          save_schedule();
      }
  } else {
      // ファイルがない場合は新規作成
      save_schedule();
  }
  // 静的講義ブロックを必ず OR する
  for (int i = 0; i < room_count; ++i) {
      for (int p = 0; p < PERIOD_MAX; ++p) {
          for (int d = 0; d < DAY_MAX; ++d) {
              rooms[i].used[p][d] |= rooms[i].initial_used[p][d];
          }
      }
  }
}

void reset_schedule_data(void) {
  // 1) data ファイルを削除
  if (remove(SCHEDULE_FILE) != 0) {
      perror("reset_schedule_data: remove failed");
  }
  // 2) メモリ上の状態もゼロクリア
  for (int i = 0; i < room_count; ++i) {
      memset(rooms[i].used, 0, sizeof(rooms[i].used));
      rooms[i].reserved_count = 0;
  }
  // 3) 新規ファイルを作成
  save_schedule();
}
int cancel_reservation(const char *room_id, int day, int period) {
  load_schedule();  // ファイルから最新状態を取得

  for (int i = 0; i < room_count; ++i) {
      if (strcmp(rooms[i].id, room_id) == 0) {
          int p = period - 1;
          if (day < 0 || day >= DAY_MAX || p < 0 || p >= PERIOD_MAX)
              return 0;
          // 講義用 initial_used は残しつつ、ユーザー予約分だけ引く
          if (rooms[i].used[p][day] && !rooms[i].initial_used[p][day]) {
              rooms[i].used[p][day] = false;
              rooms[i].reserved_ppl[p][day] = 0;
              if(rooms[i].reserved_count > 0) rooms[i].reserved_count--;
              save_schedule();
              return 1;
          }
          return 0;  // そもそも予約されてない
      }
  }
  return 0;  // room_id not found
}

void auto_free_passed_periods(void) {
    time_t now = time(NULL);
    struct tm lt;
    struct tm *tmp = localtime(&now);  // 複製用
    lt = *tmp;

    int wday = lt.tm_wday;            // Sunday=0, Monday=1, …, Saturday=6
    if (wday < 1 || wday > DAY_MAX)   // 日曜(0)と定休日(7)は何もしない
        return;

    int today_idx = wday - 1;         // 予約配列は 0=月曜, …,5=土曜
    int cur_min   = lt.tm_hour * 60 + lt.tm_min;

    // 各時限について、終了時刻を過ぎていたら解放
    for (int p = 0; p < PERIOD_MAX; ++p) {
        if (cur_min <= PERIOD_END_MINUTES[p])
            continue;  // まだ終了時刻前

        for (int i = 0; i < room_count; ++i) {
            if (rooms[i].used[p][today_idx]) {
                rooms[i].used[p][today_idx] = false;
                rooms[i].reserved_ppl[p][today_idx] = 0;
            }
        }
    }
}
// main など、スケジュール確認ルーチンの先頭で呼び出す
void load_schedule_and_cleanup(void) {
    load_schedule();                // 既存：ファイルから rooms[] を読み込む
    auto_free_passed_periods();     // 追加：自動解放
}