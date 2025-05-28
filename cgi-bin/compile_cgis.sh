#!/usr/bin/env bash
set -e

CC="gcc"
CFLAGS="-std=c11 -O2 -Wall"
COMMON="reservation.c schedule.c"

echo "== Building CGI programs =="

$CC $CFLAGS -o reserve.cgi            reserve.c            $COMMON -lm
$CC $CFLAGS -o cancel_reservation.cgi cancel_reservation.c $COMMON -lm
$CC $CFLAGS -o my_reservations.cgi    my_reservations.c    $COMMON -lm
$CC $CFLAGS -o schedule.cgi           schedule_src.c       $COMMON -lm
$CC $CFLAGS -o room.cgi               room_src.c           schedule.c -lm
$CC $CFLAGS -o schedule_version.cgi   schedule_version.c   schedule.c -lm
$CC $CFLAGS -o reset.cgi              reset.c              $COMMON -lm
$CC $CFLAGS -o auth.cgi               auth.c               -lcrypto

# ---------- インストール ----------
install_dir="${CGI_INSTALL_DIR:-./cgi-bin}"
mkdir -p "$install_dir"
for f in *.cgi; do
  chmod 755 "$f"
  mv -f "$f" "$install_dir/"
  echo "  → installed $f"
done
echo "✅ All CGI built successfully!"
