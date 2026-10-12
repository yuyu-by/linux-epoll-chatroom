#include "net_utils.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

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

    // size_t len = strlen(reply);
    size_t sent = 0;
    while (sent < len)
    {
        ssize_t n_s = send(fd, sent + reply, len - sent, 0);
        if (n_s <= 0)
        {
            return -1;
        }
        sent = sent + (size_t)n_s;
    }
    return 1;
}

// 设置SOCKED的非阻塞模式
int set_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL); // 得到文件原本状态
    if (flags == -1)
    {
        perror("Getfile");
        return -1;
    }
    else if ((fcntl(fd, F_SETFL, flags | O_NONBLOCK)) == -1) // 保存原本状态的同时设置成非阻塞模式
    {
        perror("SET-NONBLOCK");
        return -1;
    }
    return 0;
}