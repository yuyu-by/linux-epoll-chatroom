#ifndef NET_UTILS_H
#define NET_UTILS_H
#include<stddef.h>//size_t类型
int recv_line(int fd,char *line,size_t capacity);
int send_all(int fd,const char *reply,size_t len);
int set_nonblock(int fd);
typedef struct Client
{
    int fd;
    char buffer[4096];
    size_t used;
    struct Client *prev;
    struct Client *next;
} Client;
typedef struct Client *Link;
//不可以修改对应的值，但可以修改指针的指向//char *const line,可以修改值，但不可以修改指针的指向

#endif