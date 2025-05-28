/* cancel_reservation.c  ―― 自分の予約をキャンセル */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include "schedule.h"

extern Reservation reservations[];
extern int reservation_count;
void load_reservations(void); void save_reservations(void);

static int valid_uid(const char *s){
    if(!s||s[0]!='g') return 0;
    for(int i=1;i<8;i++) if(!isdigit((unsigned char)s[i])) return 0;
    return s[8]=='\0';
}
static int cookie_uid(char *out,size_t n){
    const char *ck=getenv("HTTP_COOKIE");
    return ck && sscanf(ck,"UID=%31[^;]",out)==1;
}
static void respond(int ok,const char *msg){
    printf("Content-Type: application/json\r\n\r\n");
    if(ok) printf("{\"success\":true}");
    else  printf("{\"success\":false,\"message\":\"%s\"}",msg?msg:"");
}

int main(void){
    int len=atoi(getenv("CONTENT_LENGTH")?: "0");
    char *body=malloc(len+1);
    if(!body||fread(body,1,len,stdin)!=(size_t)len){
        respond(0,"read error");return 0;
    } body[len]='\0';

    char user[32]="",room[32];int day,period;
    int hit=sscanf(body,
        "{\"user\":\"%31[^\"]\",\"room\":\"%31[^\"]\","
        "\"day\":%d,\"period\":%d}",user,room,&day,&period);
    free(body);

    if(hit!=4||!valid_uid(user)) cookie_uid(user,sizeof user);
    if(!valid_uid(user)){respond(0,"Invalid UID");return 0;}

    load_schedule(); load_reservations(); auto_free_passed_periods();

    for(int k=0;k<reservation_count;k++){
        Reservation *r=&reservations[k];
        if(!strcmp(r->user,user)&&!strcmp(r->room,room)
           &&r->day==day&&r->period==period){

            for(int i=0;i<room_count;i++) if(!strcmp(rooms[i].id,room)){
                int p=period-1;
                rooms[i].reserved_ppl[p][day]-=r->ppl;
                if(rooms[i].reserved_ppl[p][day]<=0){
                    rooms[i].reserved_ppl[p][day]=0;
                    rooms[i].used[p][day]=false;
                }
                if(rooms[i].reserved_count>0) rooms[i].reserved_count--;
            }
            memmove(r,r+1,(reservation_count-k-1)*sizeof(*r));
            reservation_count--;
            save_schedule();save_reservations();
            respond(1,NULL);return 0;
        }
    }
    respond(0,"Record not found");return 0;
}
