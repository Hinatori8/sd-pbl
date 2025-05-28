/* reserve.c  ―― 予約追加／人数更新 CGI */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include "schedule.h"

/* reservation.c */
extern Reservation reservations[];
extern int reservation_count;
void load_reservations(void);
void save_reservations(void);

/* Cookie → UID 取得 */
static int cookie_uid(char *out,size_t n){
    const char *ck=getenv("HTTP_COOKIE");
    return ck && sscanf(ck,"UID=%31[^;]",out)==1;
}

static void respond(int ok,const char *msg){
    printf("Content-Type: application/json\r\n\r\n");
    if(ok) printf("{\"success\":true}");
    else  printf("{\"success\":false,\"message\":\"%s\"}",msg?msg:"");
}

static int valid_uid(const char *s){
    if(!s||s[0]!='g') return 0;
    for(int i=1;i<8;i++) if(!isdigit((unsigned char)s[i])) return 0;
    return s[8]=='\0';
}

int main(void){
    /* --- POST 受信 --- */
    int len=atoi(getenv("CONTENT_LENGTH")?: "0");
    char *body=malloc(len+1);
    if(!body||fread(body,1,len,stdin)!=(size_t)len){
        respond(0,"read error"); return 0;
    }
    body[len]='\0';

    char user[32]="",room[32];int day,period,ppl;
    int hit=sscanf(body,
        "{\"user\":\"%31[^\"]\",\"room\":\"%31[^\"]\","
        "\"day\":%d,\"period\":%d,\"people\":%d}",
        user,room,&day,&period,&ppl);
    free(body);

    if(hit!=5||!valid_uid(user)) cookie_uid(user,sizeof user);
    if(!valid_uid(user)||ppl<=0){respond(0,"bad request");return 0;}

    load_schedule();
    load_reservations();

    /* --- 予約ロジック --- */
    for(int i=0;i<room_count;i++){
        if(strcmp(rooms[i].id,room)) continue;
        int p=period-1;

        /* 既存 → 人数更新 */
        for(int k=0;k<reservation_count;k++){
            Reservation *r=&reservations[k];
            if(!strcmp(r->user,user)&&!strcmp(r->room,room)
               &&r->day==day&&r->period==period){
                int new_tot=rooms[i].reserved_ppl[p][day]-r->ppl+ppl;
                if(new_tot>rooms[i].capacity){respond(0,"定員オーバー");return 0;}
                rooms[i].reserved_ppl[p][day]=new_tot;
                r->ppl=ppl;
                save_schedule();save_reservations();
                respond(1,NULL);return 0;
            }
        }
        /* 新規 */
        if(rooms[i].reserved_ppl[p][day]+ppl>rooms[i].capacity){
            respond(0,"定員オーバー");return 0;
        }
        rooms[i].reserved_ppl[p][day]+=ppl;
        rooms[i].used[p][day]=true;

        Reservation nr={0};
        snprintf(nr.user, sizeof nr.user, "%s", user);
        snprintf(nr.room, sizeof nr.room, "%.15s", room);
        nr.day=day;nr.period=period;nr.ppl=ppl;
        reservations[reservation_count++]=nr;

        save_schedule();save_reservations();
        respond(1,NULL);return 0;
    }
    respond(0,"room not found");
    return 0;
}
