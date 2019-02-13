#include "socketinit.h"

/*绑定服务器socket*/

int socket_init(void)
{
    int fd;
    int ret;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(MYPORT);
    addr.sin_addr.s_addr = inet_addr(MYIPADDR);
    char data[SIZEBUFF];
    fd = socket(AF_INET,SOCK_STREAM,0);
    if (0 > fd) {
        ERROR_EXIT("socket");
    }
    int val = 1;
    ret = setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(void *)&val,sizeof(val));
    if (0 > ret) {
        ERROR_EXIT("setsockopt");
    }
    ret = bind(fd,(struct sockaddr *)&addr,sizeof(addr));
    if (0 > ret) {
        ERROR_EXIT("bind");
    }
    return fd;
}

