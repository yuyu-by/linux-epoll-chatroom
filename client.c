#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "net_utils.h"
#include <string.h>
#include <poll.h>
#include <errno.h>
#define EVENTS 2
int main(void)
{
    // 创建TCP（网络接口）
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }

    // 设置服务器地址
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 客户端连接服务器的ip地址和端口号
    if ((connect(sockfd, (struct sockaddr *)&addr, sizeof(addr))) == -1)

    {
        perror("connect");
        close(sockfd);
        exit(1);
    }
    printf("Connect to server\n");

    struct pollfd fds[EVENTS];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN;

    char message[1024];
    while (1)
    {
        printf("请输入信息...\n");
        fflush(stdout);
        int ready = poll(fds, EVENTS, -1);
        //-1
        if (ready == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("poll");
            break;
        }
    
        for (int i = 0; i < EVENTS; i++)
        {

            if (!(fds[i].revents & POLLIN))
            {
                continue;
            }

            if (fds[i].fd == STDIN_FILENO)
            {

                if (fgets(message, sizeof(message), stdin) == NULL)
                {
                    break;
                }
                if (send_all(sockfd, message, strlen(message)) == -1)
                {
                    printf("发送失败\n");
                    break;
                }
            }
            else if (fds[i].fd == sockfd)
            {
                char buffer[1024];
                ssize_t rec = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                if (rec == 0)
                {
                    printf("对方关闭了连接\n");
                    close(sockfd);
                    goto cleanup;
                }
                else if (rec == -1)
                {
                    if (errno == EINTR)
                    {
                        continue;
                    }
                    else if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        continue;
                    }
                    perror("recv");
                    break;
                }
                buffer[rec] = '\0';
                printf("server%d:%s\n", sockfd, buffer);
              
                fflush(stdout);
            }
        }
    }
cleanup:
    close(sockfd);
    return 0;
}