#!/usr/bin/env bash
#  🔧 すべての .cgi を ./cgi-bin にフラット生成
set -e
cd "$(dirname "$0")"

SRCDIR="cgi-src"   # ここに *.c
OUTDIR="cgi-bin"   # ここへ *.cgi
mkdir -p "$OUTDIR"

CC=gcc
CFLAGS="-std=c11 -O2 -Wall"
COMMON="$SRCDIR/reservation.c $SRCDIR/schedule.c"

build(){ echo " -> $1"; $CC $CFLAGS -o "$OUTDIR/$1" "${@:2}"; }

echo "== Building CGI =="

build auth.cgi               $SRCDIR/auth.c                        -lcrypto
build seed.cgi               $SRCDIR/seed.c
build reserve.cgi            $SRCDIR/reserve.c            $COMMON  -lm
build cancel_reservation.cgi $SRCDIR/cancel_reservation.c $COMMON  -lm
build my_reservations.cgi    $SRCDIR/my_reservations.c    $COMMON  -lm
build schedule.cgi           $SRCDIR/schedule_src.c       $COMMON  -lm
build room.cgi               $SRCDIR/room_src.c           $SRCDIR/schedule.c -lm
build schedule_version.cgi   $SRCDIR/schedule_version.c   $SRCDIR/schedule.c -lm
[[ -f $SRCDIR/reset.c ]] && build reset.cgi $SRCDIR/reset.c $COMMON -lm

chmod 755 "$OUTDIR"/*.cgi
echo "✅  CGI built into ./$OUTDIR/"
