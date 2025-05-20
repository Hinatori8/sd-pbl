// reservation.c  ―― Reservation 配列の永続化
#include "schedule.h"
#include <stdio.h>
#include <string.h>
#include <sys/file.h>

Reservation reservations[RESV_MAX];
int reservation_count = 0;

#define RESV_FILE "../data/reservations.dat"

static FILE *open_locked(const char *path, const char *mode, int lock)
{
    FILE *fp = fopen(path, mode);
    if (!fp) return NULL;
    flock(fileno(fp), lock);
    return fp;
}

void load_reservations(void)
{
    FILE *fp = open_locked(RESV_FILE, "rb", LOCK_SH);
    if (!fp) { reservation_count = 0; return; }
    reservation_count = fread(reservations,
                              sizeof(Reservation),
                              RESV_MAX,
                              fp);
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
}

void save_reservations(void)
{
    FILE *fp = open_locked(RESV_FILE, "wb", LOCK_EX);
    if (!fp) return;
    fwrite(reservations,
           sizeof(Reservation),
           reservation_count,
           fp);
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
}
