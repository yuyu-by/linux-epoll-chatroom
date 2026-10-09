#include "net_utils.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

// 实现逐行接收字符串
int recv_line(int fd, char *line, size_t capacity)
{
    size_t used = 0;
    while (1)
    {

        if (capacity == 0) // 检查容量是否可以接受字符
        {
            return -1;
        }
        ssize_t c_r;
        char ch;
        c_r = recv(fd, &ch, 1, 0); // 把接收到的字符保存到了ch当中
        if (c_r == -1)
        {

            return -1;
        }
        else if (c_r == 0)
        {

            return 0;
        }
        if (ch == '\n')
        {
            line[used] = '\0';
            return 1;
        }
        if (used >= capacity - 1)
        {
            return -1;
        }

        line[used] = ch;
        used++;
    }
}
// 发送信息
int send_all(int fd, const char *reply, size_t len)
{
    
    //size_t len = strlen(reply);
    size_t sent = 0;
    while (sent < len)
    {
        ssize_t n_s = send(fd, sent+reply, len-sent, 0);
         if (n_s <= 0)
            {
               return -1;
            }
            sent = sent + (size_t)n_s;
        
    }
    return 1;
}