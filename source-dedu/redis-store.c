/*
 * key-value storage using redis
 * <fp, ref count> 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hiredis/hiredis.h"

static redisContext *c = NULL;

void redisInit(){
    const char *hostname = "127.0.0.1";
    int port = 6379;

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    c = redisConnectWithTimeout(hostname, port, timeout);
    if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }

    printf("**redis init ok**\n");
}

int isExistsFP(char *fp){
    redisReply *reply = redisCommand(c,"GET %s", fp);
    if(reply->type == REDIS_REPLY_NIL){
        printf("**key(%s) not exists**\n", fp);
        return 0;
    }
    freeReplyObject(reply);

    return 1;
}

// file name -> fp
void addFileFP(char *fp, char *filename){
    redisReply *reply = redisCommand(c,"SET %s %s", filename, fp);
    freeReplyObject(reply);
}

// fp -> {file name 1, file name 2.....}
void addFilename(char *fp, char *filename){
    redisReply *reply = redisCommand(c,"SADD %s %s", fp, filename);
    freeReplyObject(reply);
}

int isExistsFilename(char *fp, char *filename){
    int res = 0;
    redisReply *reply = redisCommand(c,"SISMEMBER %s %s", fp, filename);
    if(reply->integer)
        res = 1;
    freeReplyObject(reply);

    return res;
}


