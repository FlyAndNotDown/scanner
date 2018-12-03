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

// 函数定义
void showLanHostAndIP();
int tcpPortScan(char *ip, int port);

int main(int argc, char *argv[]) {


    return 0;
}

// 显示局域网主机名和IP
void showLanHostAndIP() {

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
