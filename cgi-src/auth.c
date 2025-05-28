/* auth.c  ── 初回登録＋ログイン兼用 */
#define _POSIX_C_SOURCE 200809L    /* fileno が宣言される */
#include <stdio.h>
#include <stdlib.h>                /* atoi, getenv, malloc, free */
#include <string.h>
#include <unistd.h>                /* fileno */
#include <openssl/sha.h>
#include <sys/file.h>
#include "schedule.h"

/* —―― users.dat の I/O ――― */
typedef struct { char uid[10]; char hash[65]; } User;
#define USERS_MAX 1000
#define USERFILE "../data/users.dat"

static User users[USERS_MAX];
static int  user_cnt = 0;

static void load_users(void){
    FILE *fp = fopen(USERFILE,"rb");
    if(!fp) return;
    flock(fileno(fp), LOCK_SH);
    user_cnt = fread(users,sizeof(User),USERS_MAX,fp);
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
}
static void save_users(void){
    FILE *fp = fopen(USERFILE,"wb");
    if(!fp) return;
    flock(fileno(fp), LOCK_EX);
    fwrite(users,sizeof(User),user_cnt,fp);
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
}

/* ――― 文字列→SHA-256(16進 64文字) ――― */
static void sha256hex(const char *s,char out[65]){
    unsigned char d[32];
    SHA256((const unsigned char*)s, strlen(s), d);
    for(int i=0;i<32;i++) sprintf(out+2*i,"%02x",d[i]);
    out[64]='\0';
}

/* ――― main ――― */
int main(void){
    /* 1) JSON 受信: {"uid":"g1234567","pwd":"1234","seed":"5678"} */
    char uid[10],pwd[8],seed[8]={0};
    const char *cl = getenv("CONTENT_LENGTH");
    int len = cl ? atoi(cl) : 0;
    char *body=malloc(len+1);
    if(!body || fread(body,1,len,stdin)!=(size_t)len){
        puts("Status:400\r\n\r\n{}"); return 0;
    }
    body[len]=0;
    sscanf(body,"{\"uid\":\"%9[^\"]\",\"pwd\":\"%7[^\"]\",\"seed\":\"%7[^\"]\"}",uid,pwd,seed);
    free(body);

    load_users();

    /* 2) 検索 */
    int idx=-1;
    for(int i=0;i<user_cnt;i++) if(strcmp(users[i].uid,uid)==0){ idx=i; break; }

    char hash[65]; sha256hex(pwd,hash);

    if(idx<0){
        /* --- 初回 --- */
        if(strcmp(pwd,seed)!=0){ puts("Status:400\r\n\r\n{\"success\":false,\"msg\":\"seed mismatch\"}"); return 0; }
        strcpy(users[user_cnt].uid,uid);
        strcpy(users[user_cnt].hash,hash);
        user_cnt++; save_users();
    }else{
        /* --- ２回目以降 --- */
        if(strcmp(users[idx].hash,hash)!=0){ puts("Status:401\r\n\r\n{\"success\":false}"); return 0;}
    }

    /* 3) OK → Cookie 発行 */
    printf("Content-Type: application/json\r\n");
    printf("Set-Cookie: UID=%s; Path=/; HttpOnly\r\n\r\n",uid);
    puts("{\"success\":true}");
    return 0;
}
