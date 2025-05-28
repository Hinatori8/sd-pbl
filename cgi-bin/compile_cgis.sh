#!/usr/bin/env bash
# compile_cgis.sh  ---- build all CGI programs into ./cgi-bin
set -e

CC=gcc
CFLAGS="-std=c11 -O2 -Wall"
COMMON_SRCS="reservation.c schedule.c"

echo "== Building CGI programs =="

mkdir -p cgi-bin            # 1 段だけ

# 1) 認証 CGI
echo "[1/8] auth.cgi"
$CC $CFLAGS -o cgi-bin/auth.cgi       auth.c -lcrypto

# 2) 初回パスワード用 seed (時刻 or 乱数) CGI
echo "[2/8] seed.cgi"
$CC $CFLAGS -o cgi-bin/seed.cgi       server_time.c

# 3) 予約追加
echo "[3/8] reserve.cgi"
$CC $CFLAGS -o cgi-bin/reserve.cgi    reserve.c $COMMON_SRCS -lm

# 4) 予約取消
echo "[4/8] cancel_reservation.cgi"
$CC $CFLAGS -o cgi-bin/cancel_reservation.cgi \
            cancel_reservation.c      $COMMON_SRCS -lm

# 5) 自分の予約一覧
echo "[5/8] my_reservations.cgi"
$CC $CFLAGS -o cgi-bin/my_reservations.cgi \
            my_reservations.c         $COMMON_SRCS -lm

# 6) 時間割表示
echo "[6/8] schedule.cgi"
$CC $CFLAGS -o cgi-bin/schedule.cgi   schedule_src.c $COMMON_SRCS -lm

# 7) 教室一覧
echo "[7/8] room.cgi"
$CC $CFLAGS -o cgi-bin/room.cgi       room_src.c schedule.c -lm

# 8) バージョン表示／ダンプ
echo "[8/8] schedule_version.cgi"
$CC $CFLAGS -o cgi-bin/schedule_version.cgi \
            schedule_version.c schedule.c -lm

# （任意）データ初期化 CGI
if [[ -f reset.c ]]; then
  echo "[+] reset.cgi"
  $CC $CFLAGS -o cgi-bin/reset.cgi    reset.c $COMMON_SRCS -lm
fi

chmod 755 cgi-bin/*.cgi
echo "✅ All CGI built successfully into ./cgi-bin/"
