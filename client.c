#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
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

    char msg[] = "hello TCp";
    ssize_t n_s = send(sockfd, msg, sizeof(msg), 0);
    if (n_s == -1)
    {
        perror("send");
        close(sockfd);
        exit(1);
    }
    else
    {
        printf("send%ld bytes\n", n_s);
    }
    char buffer[1024] = {0};

    ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

    if (n > 0)
    {
        buffer[n] = '\0';
        printf("Server says: %s\n", buffer);
    }
    else if (n == 0)
    {
        printf("Server closed connection\n");
    }
    else
    {
        perror("recv");
    }
    close(sockfd);
    return 0;
}