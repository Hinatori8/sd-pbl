#ifndef SCHEDULE_H
#define SCHEDULE_H
#include <stdbool.h>
#include <stdint.h>
/* 定数 */
#define SCHEDULE_FILE "../data/schedule.dat"
#define DAY_MAX     6 /* 曜日数 */
#define PERIOD_MAX  5 /* 時限数 */
#define ROOM_MAX   60 /* 登録教室上限数 */
#define MAX_IMAGES 5 /* 登録画像上限数 */
#define h 60

/* 曜日 */
typedef enum { MON, TUE, WED, THU, FRI , SAT} Day;

/* 設備列挙 */
typedef enum {
    // ── 黒板系 ──
    FEAT_BLACKBOARD,         // 黒板
    FEAT_BLACKBOARD_DBL,     // 黒板(2枚上下)
    // ── ホワイトボード系 ──
    FEAT_WHITEBOARD,         // ホワイトボード
    FEAT_WHITEBOARD_WALL,    // ホワイトボード(壁面)
    FEAT_GLASSBOARD,         // グラスボード
    // ── ネットワーク ──
    FEAT_LAN,                // 有線LAN

    FEAT_MAX
} FeatureType;

/* 設備マスク */
typedef uint32_t FeatureMask;

/* FeatureNames 配列は room_src.c で実体を持つ */
extern const char *FeatureNames[FEAT_MAX];

/* 机タイプ */
typedef enum {
    FIXED_DESK, // == 0を意味させる(マジックナンバー対策)
    MOVABLE_DESK
} DeskType;

/* 充電タイプ */
typedef enum {
    NO_CHARGE,
    CHARGE_FIXED,
    CHARGE_EXTENSION
} ChargeType;

/* 教室サイズ */
typedef enum {
    BIG,MID,SMALL
} SizeType;

/* 教室構造体にフィールド追加 */
typedef struct {
    const char   *id;
    int           capacity;
    FeatureMask   features;
    DeskType      desk;
    ChargeType    charge;
    SizeType      size;
    bool initial_used[PERIOD_MAX][DAY_MAX];
    bool          used[PERIOD_MAX][DAY_MAX];
    int wired_mic;
    int wireless_mic;
    int tv;         // TV ○/×
    int bd;         // BD ○/×
    int dvd;        // DVD ○/×
    int vhs;        // VHS ○/×
    int od;         // OD○/×
    int pc;
    int mic;
    int hdmi;
    int webcam;     // Webカメラ ○/×
    int mic_input;  // 持込PC接続用音声マイク入力 ○/×
    /*画像情報*/
    int           img_count;
    const char    *img_paths[MAX_IMAGES];
    int reserved_count;
    int  reserved_ppl[PERIOD_MAX][DAY_MAX];
} Room;

/* ここで初めて extern 宣言 */
extern Room  rooms[ROOM_MAX];
extern const int room_count;

 // =============== 永続化用データ構造 ===============
typedef struct {
    bool used[PERIOD_MAX][DAY_MAX];
    int  reserved_count;
    int  reserved_ppl[PERIOD_MAX][DAY_MAX];
} RoomState;
extern const char *FeatureNames[FEAT_MAX];
extern const char *DeskTypeNames[];
extern const char *ChargeTypeNames[];
extern const char *SizeTypeNames[];
extern const int PERIOD_END_MINUTES[PERIOD_MAX];
void load_schedule(void);
void save_schedule(void);
void reset_schedule_data(void);
int cancel_reservation(const char *room_id, int day, int period);
void auto_free_passed_periods(void);
#endif /* SCHEDULE_H */
