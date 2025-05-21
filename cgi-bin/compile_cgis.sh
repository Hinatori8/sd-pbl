#!/usr/bin/env bash
set -e

CFLAGS="-std=c11 -O2 -Wall"

echo "== Building CGI programs =="

echo "[1/6] reserve.cgi"
gcc $CFLAGS -o reserve.cgi \
    reserve.c reservation.c schedule.c -lm

echo "[2/6] cancel_reservation.cgi"
gcc $CFLAGS -o cancel_reservation.cgi \
    cancel_reservation.c reservation.c schedule.c -lm

echo "[3/6] my_reservations.cgi"
gcc $CFLAGS -o my_reservations.cgi \
    my_reservations.c reservation.c schedule.c -lm

echo "[4/6] schedule.cgi"
gcc $CFLAGS -o schedule.cgi \
    schedule_src.c reservation.c schedule.c -lm

echo "[5/6] room.cgi"
gcc $CFLAGS -o room.cgi \
    room_src.c schedule.c -lm

echo "[6/6] schedule_version.cgi"
gcc $CFLAGS -o schedule_version.cgi \
    schedule_version.c schedule.c -lm

echo "✅ All CGI built successfully!"
