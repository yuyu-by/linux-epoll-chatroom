#include <stdio.h> //perror printf
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> //close
#include <string.h> //strlen.memeset
#include <arpa/inet.h>
#include <stdlib.h> //exit
#include "net_utils.h"
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include"Link.h"
int main()
/*网络socket*/
{
Link clt_head=NULL;

    // 准备一个服务器地址，
    struct sockaddr_in addr; // struct sockaddr_in addr={0};
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 创建网络通信接口（套接字），设置为Ipv4，流式套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }
    // printf("server socket creat success\n");

    // 绑定ip地址和端口号
    if ((bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) == -1)
    {
        perror("bind");
        close(sockfd);
        exit(1);
    }
    printf("server bind id/port success\n");

    // 监听
    if ((listen(sockfd, 5)) == -1)
    {
        perror("listen");
        close(sockfd);
        exit(1);
    }

    // 将socket设置成非阻塞模式
    if ((set_nonblock(sockfd)) == -1)
    {
        // printf("socked nonblock error\n");
        close(sockfd);
        return 1;
    }
    printf("socked nonblock success\n");
    /*监听epoll*/
    int epfd = epoll_create1(0);
    if (epfd == -1)
    {
        perror("epoll creat1");
        return 1;
    }
    Client listener = {0};
    listener.fd = sockfd;

    // 准备监听事件
    struct epoll_event ev={0};
    ev.events = EPOLLIN;
    ev.data.ptr = &listener;
    // 把监听的文件和对应的事件放入监听列表
    if ((epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev)) == -1)
    {
        // printf("set ctl error");
        perror("epool ctl");
        close(epfd);
        close(sockfd);
        return 1;
    }

    struct epoll_event events[10];
    while (1)
    {
        int e_w = epoll_wait(epfd, events, 10, -1); // 等到出现就绪事件
        if (e_w == -1)
        {
            if (errno == EINTR)
            {
                // printf("Recving again....\n");
                continue;
            }

            perror("epoll wait");
            break;
        }

        for (int i = 0; i < e_w; i++)
        {
            Client *client = events[i].data.ptr;
            int fd = client->fd;

            // 判断是否为新客户端
            if (fd == sockfd)
            {
                while (1)
                {
                
                    int clientfd = accept(sockfd, NULL, NULL);
                    if (clientfd == -1)
                    {
                        if (errno == EINTR)
                        {
                            continue; // 重新执行循环
                        }
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            break;
                        }

                        perror("accept");
                        break;
                    }
                    else if (clientfd >= 0)
                    {
                        if ((set_nonblock(clientfd)) == -1) // 将对应clientfd设置成非阻塞模式
                        {
                            perror("set clinet nonoblock");
                            close(clientfd);
                            continue;
                        }
                        printf("client:%d已连接\n", clientfd);
                        // 这个客户端还没有注册到 epoll！
                        // 这意味着服务器虽然接受了连接，却还不能通过 epoll_wait() 监听这个客户端发送的消息。
                       Link client = malloc(sizeof(Client));
                        if (client == NULL)
                        {
                            perror("malloc");
                            close(clientfd);

                            continue;
                        }
                        client->fd=clientfd;
                        client->used=0;

                        struct epoll_event clilent_ev;
                        clilent_ev.events = EPOLLIN;
            
                        // clilent_ev.data.fd = clientfd;
                        clilent_ev.data.ptr = client;
                        if ((epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &clilent_ev)) == -1)
                        {
                            printf("set ctl client error");
                            perror("epool ctl client");
                            close(clientfd);
                            free(client);
                            continue;
                        }
                       insert_client(&clt_head,client);
                    }
                }
            }
            // 处理已经连接的客户端
            else
            {

                while (1)
                {
                    ssize_t ret = recv(fd, client->buffer + client->used, sizeof(client->buffer) - client->used, 0);
                    if (client->used == sizeof(client->buffer))
                    {
                        printf("消息超过字数限制\n");
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                        close(fd);
                        free(client);
                        break;
                    }

                    if (ret == 0)
                    {
                        printf("对方已离线\n");
                        // 将这个fd从epoll的关注列表移除
                        
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL); // 删除注册
                        remove_client(&clt_head,client);
                        close(fd);
                        free(client); // 关闭客户端 Socket，释放文件描述符。
                        break;
                    }
                    else if (ret == -1)
                    {
                        if (errno == EINTR)
                        {

                            continue;
                        }
                        else if (errno == EAGAIN || errno == EWOULDBLOCK) // 暂时没有数据
                        {
                            break; // 退出接收循环不等于关闭连接
                        }

                        perror("recv");
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                        remove_client(&clt_head, client);
                        close(fd);
                        break;
                    }
                    else
                    {
                        client->used += (size_t)ret;
                        while (1)
                        {
                            char *newline = memchr(client->buffer, '\n', client->used);
                            if (newline == NULL)
                            {
                                break;
                            }
                            else if (newline != NULL)
                            {
                                size_t message_len = newline - client->buffer + 1;
                                printf("找到一条完整消息 长度为：%zu字节\n", message_len);
                                fwrite(client->buffer, 1, message_len, stdout);
                                fflush(stdout);
                                
                                printf("请输入要发送的消息：");
                                ssize_t n_s = send(fd, client->buffer, message_len, 0);
                                {
                                    if (n_s == -1)
                                    {
                                        perror("send");
                                    }
                                    else
                                    {
                                        printf("服务器本次发送了 %zd 字节\n", n_s);
                                    }
                                }
                                size_t remaining = client->used - message_len;
                                memmove(client->buffer, client->buffer + message_len, remaining);
                                client->used = remaining;
                            }
                        }
                    }
                }
            }
        }
    }
    close(epfd);
    close(sockfd);
    return 0;
}
#if 0
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
    }
#endif
