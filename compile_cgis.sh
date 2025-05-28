#!/usr/bin/env bash
# compile_cgis.sh  ---- build CGI into ./cgi-bin  (sources in ./cgi-src)
set -e
cd "$(dirname "$0")"        # プロジェクトルートへ

SRCDIR="cgi-src"            # ← .c が入っているフォルダ
OUTDIR="cgi-bin"
mkdir -p "$OUTDIR"

CC=gcc
CFLAGS="-std=c11 -O2 -Wall"
COMMON="$SRCDIR/reservation.c $SRCDIR/schedule.c"

build () {           # build <out> <srcs...> [libs...]
  echo " -> $1"
  $CC $CFLAGS -o "$OUTDIR/$1" "${@:2}" || { echo "✗ $1"; exit 1; }
}

echo "== Building CGI =="
build auth.cgi               $SRCDIR/auth.c                        -lcrypto
build seed.cgi               $SRCDIR/server_time.c
build reserve.cgi            $SRCDIR/reserve.c            $COMMON  -lm
build cancel_reservation.cgi $SRCDIR/cancel_reservation.c $COMMON  -lm
build my_reservations.cgi    $SRCDIR/my_reservations.c    $COMMON  -lm
build schedule.cgi           $SRCDIR/schedule_src.c       $COMMON  -lm
build room.cgi               $SRCDIR/room_src.c           $SRCDIR/schedule.c -lm
build schedule_version.cgi   $SRCDIR/schedule_version.c   $SRCDIR/schedule.c -lm
[[ -f $SRCDIR/reset.c ]] && build reset.cgi $SRCDIR/reset.c $COMMON -lm

chmod 755 "$OUTDIR"/*.cgi
echo "✅  CGI built into ./$OUTDIR/"
