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

// 函数定义
void showLanHostAndIP();
int tcpPortScan(char *ip, int port);

// 定义结构体用于存储主机名和IP
typedef struct {
    char name[40];
    char ip[40];
} Host;

// 全局变量
// 用于存储局域网主机名和IP的数组
Host lanHosts[MAX_LAN_HOST_NUM];
int lanHostsNum = 0;

int main(int argc, char *argv[]) {
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
        // TODO
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
        strcpy(lanHosts[lanHostsNum].name, buffer1);
        strcpy(lanHosts[lanHostsNum++].ip, buffer2);
    }

    // 关闭文件
    fclose(hostNameFile);
    fclose(hostIpFile);

    // 先输出局域网主机信息
    printf("All host info in LAN: \n");
    printf("No\t\tName\t\tIP\n");
    for (i = 0; i < lanHostsNum; i++) {
        printf("%d\t\t%s\t\t%s\n", i + 1, lanHosts[i].name, lanHosts[i].ip);
    };
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
}
