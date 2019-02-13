#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <poll.h>

#define MYPORT 9527
#define MYIPADDR "127.0.0.1"
#define ERROR_EXIT(arg) do\
        {\
            perror(arg);\
            exit(EXIT_FAILURE);\
        }while(0)
#define SIZEBUFF 1024

int socket_init(void);

#endif
