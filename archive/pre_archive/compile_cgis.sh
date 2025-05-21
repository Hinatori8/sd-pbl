#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# compile_cgis.sh
# CGI バイナリ一括コンパイルテンプレート
# -----------------------------------------------------------------------------

set -euo pipefail

cd "$(dirname "$0")"
# C 言語の標準と最適化オプション
STD="-std=c11"
OPT="-O2"

rm -f *.cgi
# --- コンパイル ---
echo "Compiling cancel_reservation.cgi..."
gcc $STD $OPT -o cancel_reservation.cgi cancel_reservation.c schedule.c

echo "Compiling reset.cgi..."
gcc $STD $OPT -o reset.cgi reset.c schedule.c

echo "Compiling my_reservations.cgi..."
gcc $STD $OPT -o my_reservations.cgi my_reservations.c schedule.c

echo "Compiling reserve.cgi..."
gcc $STD $OPT -o reserve.cgi reserve.c schedule.c

echo "Compiling schedule.cgi..."
gcc $STD $OPT -o schedule.cgi schedule_src.c schedule.c

echo "Compiling room.cgi..."
gcc $STD $OPT -o room.cgi room_src.c schedule.c

echo "Compiling server_time.cgi..."
gcc $STD $OPT -o server_time.cgi        server_time.c

echo "Compiling schedule_version.cgi..."
gcc $STD $OPT -o schedule_version.cgi schedule_version.c
# 実行権限を付与
chmod 755 *.cgi

echo "All CGI binaries have been compiled and permissions set."
ls -l *.cgi