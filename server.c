#include <stdio.h> //perror printf
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> //close
#include <string.h> //strlen.memeset
#include <arpa/inet.h>
#include <stdlib.h> //exit
#include "net_utils.h"

#if 0
struct sockaddr_in
{
    sa_family_t sin_family;
    in_port_t sin_port;
    struct in_addr sin_addr;
};
#endif

int main()
{                            // 准备一个服务器地址，
    struct sockaddr_in addr; // struct sockaddr_in addr={0};
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 创建网络接口，设置为Ipv4，流式套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }
    // 绑定ip地址和端口号
    if ((bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) == -1)
    {
        perror("bind");
        close(sockfd);
        exit(1);
    }
    // 监听
    if ((listen(sockfd, 5)) == -1)
    {
        perror("listen");
        close(sockfd);
        exit(1);
    }
    // socklen_t len=sizeof(buf);
    // accept(sockfd,(struct sockaddr*)&addr,&len);
    //接收客户端
    printf("waiting for clinet connect.....\n");
    int clientfd = accept(sockfd, NULL, NULL); // 不需要获取客户端的地址信息

    if (clientfd == -1)
    {
        perror("accept");
        close(sockfd);
        exit(1);
    }

    while (1)
    {
        // 服务器接受客户端信息
        char buffer[1024] = {0};

        int recv = recv_line(clientfd, buffer, sizeof(buffer));
        if (recv == 0)
        {
            printf("Client shut down");
            break;
        }
        else if (recv == -1)
        {
            perror("rec_line");
            close(clientfd);
            close(sockfd);
            exit(1);
        }
        else
        {
            printf("client:%s\n", buffer);
        }

#if 0
        int n_s=send(clientfd,reply,sizeof(reply),0);
        if(n_s==-1)
        {
            perror("send");
            close(sockfd);
            close(clientfd);
            exit(1);
        }
        else
        {
            printf("send message%d bytes\n",n_s);
        }
#endif
#if 0
        for (int i = 0; i < rc; i++)
        {
            char ch = buffer[i]; // 取出本次收到的字节
            if (ch == '\n')
            {
                line[used] = '\0';
                printf("收到完整信息：%s\n", line);
                used = 0;
            }
            else
            {
                // 防止缓冲区越界
                if (used < sizeof(line) - 1)
                {
                    line[used] = ch;
                    used++;
                }
                else
                {
                    printf("消息过长，停止接收\n");
                    close(clientfd);
                    close(sockfd);
                    return 1;
                }
                    
            }
#endif

        // 服务器输入回复
        char reply[1024];
        printf("please input server msg:");
        fflush(stdout); // 立刻刷新缓冲区，尽快显示pritnf 的内容
        if ((fgets(reply, sizeof(reply), stdin)) == NULL)
        {
            break;
        }

        int ret = send_all(clientfd, reply, strlen(reply));

        if (ret == -1)
        {
            printf("send_all failed\n");
            break;
        }
        else if (ret == 1)
        {
            printf("server:%s\n", reply);
        }

#if 0
        // 发送消息
        size_t len = strlen(reply);
        size_t sent = 0;   // sent不能一次性发送所有字节，已经发送的字节
        while (sent < len) // 因为tcp是字节流协议，需要消息边界处理
        {
            ssize_t n_s = send(clientfd, reply + sent, len - sent, 0);
            if (n_s <= 0)
            {
                perror("Send");
                break;
            }
            sent = sent + (size_t)n_s;
            
        }
#endif
    }

    close(clientfd);
    close(sockfd);
    return 0;
}