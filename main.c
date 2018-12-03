#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/select.h>

// 宏定义
#define MAX_LAN_HOST_NUM 200
#define BUFFER_SIZE 1024
#define CHAR_BUFFER_SIZE 40
#define DEFAULT_THREAD_NUM 8
#define DEFAULT_START_PORT 1
#define DEFAULT_END_PORT 1024

// 函数定义
void showLanHostAndIP();
int tcpPortScan(char *, int);
int scan(char *, int, int, int);
void *scanThreadFunc(void *args);

// 定义结构体
// 主机结构体
typedef struct {
    char name[40];
    char ip[40];
} Host;
// 扫描线程参数结构体
typedef struct {
    int pthreadNum;
    char *ip;
    int port;
    int end;
} ScanThreadArgs, *ScanThreadArgsPtr;

// 全局变量
// 用于存储局域网主机名和IP的数组
Host lanHosts[MAX_LAN_HOST_NUM];
int lanHostsNum = 0;

int main(int argc, char *argv[]) {
    int startPort = DEFAULT_START_PORT;
    int endPort = DEFAULT_END_PORT;
    int threadNum = DEFAULT_THREAD_NUM;

    // 参数判断
    if (argc < 2) {
        printf("Usage:\n");
        printf("1. ./scanner -lan\n");
        printf("2. ./scanner -ip ipAddress\n");
        return 0;
    }

    // 两种用法
    if (!strcmp(argv[1], "-lan")) {
        // 获取并显示局域网主机名和IP
        showLanHostAndIP();
    } else if (!strcmp(argv[1], "-ip")) {
        // 获取参数
        if (argc > 2) startPort = atoi(argv[3]);
        if (argc > 3) endPort = atoi(argv[4]);
        if (argc > 4) threadNum = atoi(argv[5]);

        // 开始扫描
        scan(argv[2], startPort, endPort, threadNum);
    } else {
        printf("Unknown params.\n");
    }

    return 0;
}

// 显示局域网主机名和IP
void showLanHostAndIP() {
    // 文件结构体
    FILE *hostNameFile, *hostIpFile;
    // 缓冲区
    char buffer1[CHAR_BUFFER_SIZE], buffer2[CHAR_BUFFER_SIZE];
    int i;

    // 执行shell指令获取局域网所有主机名和IP，并将结果输出到.host
    system("arp -a | cut -d \" \" -f 1 > .hname");
    system("arp -a | cut -d \" \" -f 2 | sed \"s/\(//g\" | sed \"s/)//g\" > .hip");

    // 打开文件
    hostNameFile = fopen("./.hname", "r");
    hostIpFile = fopen("./.hip", "r");

    // 不断读取并且存入结构体
    while (fgets(buffer1, CHAR_BUFFER_SIZE, hostNameFile) && fgets(buffer2, CHAR_BUFFER_SIZE, hostIpFile)) {
        // 如果最后一个字符是 \n，将其改成 \0
        if (buffer1[strlen(buffer1) - 1] == '\n') buffer1[strlen(buffer1) - 1] = '\0';
        if (buffer2[strlen(buffer2) - 1] == '\n') buffer2[strlen(buffer2) - 1] = '\0';

        // 拷贝
        strcpy(lanHosts[lanHostsNum].name, buffer1);
        strcpy(lanHosts[lanHostsNum++].ip, buffer2);
    }

    // 关闭文件
    fclose(hostNameFile);
    fclose(hostIpFile);

    // 先输出局域网主机信息
    printf("All host info in LAN: \n");
    printf("No\tName\t\tIP\n");
    for (i = 0; i < lanHostsNum; i++) {
        printf("%d\t%s\t\t%s\n", i + 1, lanHosts[i].name, lanHosts[i].ip);
    };
}

// 扫描函数
int scan(char *ip, int startPort, int endPort, int threadNum) {
    // 线程结构体数组
    pthread_t *pthreads;
    // 线程参数结构体数组
    ScanThreadArgsPtr argsArray;

    int i, pthreadNum = threadNum;

    // 输出日志
    printf("\nScanning %s......\n", ip);

    // 分配空间
    pthreads = (pthread_t *) malloc(sizeof(pthread_t) * pthreadNum);
    argsArray = (ScanThreadArgsPtr) malloc(sizeof(ScanThreadArgs));

    // 建立线程
    for (i = 0; i < pthreadNum; i++) {
        // 设置参数
        memset(&argsArray[i], 0, sizeof(argsArray[i]));
        argsArray[i].pthreadNum = pthreadNum;
        argsArray[i].port = startPort + i;
        argsArray[i].ip = ip;
        argsArray[i].end = endPort;

        if (pthread_create(&pthreads[i], NULL, scanThreadFunc, (void *) &argsArray[i]) == -1) {
            printf("Can;t create pthread! Please try again!\n");
            return 0;
        }
    }

    // 睡一会，让子线程先执行
    sleep(1);

    // 等待线程释放
    for (i = 0; i < pthreadNum; i++) {
        pthread_join(pthreads[i], NULL);
    }

    // 释放空间
    free(pthreads);
    free(argsArray);

    // 日志
    printf("Scan down.\n");

    return 0;
}

// 扫描线程函数
void *scanThreadFunc(void *args) {
    // 获取参数
    ScanThreadArgsPtr temp;
    temp = (ScanThreadArgsPtr) args;

    // 一个线程扫描一部分端口
    while (temp->port <= temp->end) {
        // 如果扫描到了，则在日志中输出
        if (tcpPortScan(temp->ip, temp->port)) printf("Port %d tcp - Open\n", temp->port);
        // TODO UDP

        temp->port += temp->pthreadNum;
    }
}

// tcp 端口扫描
int tcpPortScan(char *ip, int port) {
    // socket文件描述符，文件描述符状态，连接状态
    int sockfd, len, fcntlStatus, connectStatus;
    // socket地址结构体
    struct sockaddr_in addr;
    // 超时
    struct timeval timeout;
    // 读写用的文件句柄
    fd_set fdr, fdw;

    // 置空结构体
    memset(&addr, 0, sizeof(addr));
    memset(&timeout, 0, sizeof(timeout));

    // 设置为IP通信
    addr.sin_family = AF_INET;
    // 设置地址结构体中的IP地址
    addr.sin_addr.s_addr = inet_addr(ip);
    // 设置地址结构体中的端口号
    addr.sin_port = htons(port);

    // 创建 socket 套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return 0;
    }

    // 设置套接字为非阻塞模式
    fcntlStatus = fcntl(sockfd, F_GETFL, 0);
    if (fcntlStatus < 0) {
        close(sockfd);
        return 0;
    }
    fcntlStatus |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, fcntlStatus) < 0) {
        close(sockfd);
        return 0;
    }

    // 尝试连接
    connectStatus = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));
    if (connectStatus != 0) {
        if (errno == EINPROGRESS) {
            FD_ZERO(&fdr);
            FD_ZERO(&fdw);
            FD_SET(sockfd, &fdr);
            FD_SET(sockfd, &fdw);
            // 设置1s超时
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            connectStatus = select(sockfd + 1, &fdr, &fdw, NULL, &timeout);

            // 如果连接超时或者调用失败
            if (connectStatus <= 0 || connectStatus == 2) {
                close(sockfd);
                return 0;
            }

            // 如果连接成功
            if (connectStatus == 1 && FD_ISSET(sockfd, &fdw)) {
                close(sockfd);
                return 1;
            }
            close(sockfd);
            return 0;
        }
    }
}
