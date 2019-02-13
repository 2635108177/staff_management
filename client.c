#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <poll.h>
#include <string.h>

#define PORT 9527
#define IPADDR "127.0.0.1"
#define ERROR_EXIT(arg) do\
{       \
    perror(arg);\
    exit(EXIT_FAILURE);\
}while(0)
#define SIZEBUFF 1024
#define USEBUFF 32

typedef struct
{
    char name[USEBUFF];
    char age[USEBUFF];
    char address[USEBUFF];
    char salary[USEBUFF];
}staff_t;

int flags = 0;
int main(int argc, const char *argv[])
{
    int fd;
    int ret;
    char data[SIZEBUFF];
    char command[USEBUFF];
    char user[USEBUFF];
    char pass[USEBUFF];
    char repass[USEBUFF];
    bzero(data,SIZEBUFF);
    fd = socket(AF_INET,SOCK_STREAM,0);
    if (0 > fd) {
        ERROR_EXIT("socket");
    }
    struct sockaddr_in peeraddr;
    peeraddr.sin_family = AF_INET;
    peeraddr.sin_port = htons(PORT);
    peeraddr.sin_addr.s_addr = inet_addr(IPADDR);
    ret = connect(fd,(struct sockaddr *)&peeraddr,sizeof(peeraddr));
    if (0 > ret) {
        ERROR_EXIT("connect");
    }

    while (1) {
        int u = 0;
        printf("choose root[1],user[2] or exit[3]?\n");
        scanf("%d",&u);
        if (1 == u) {

            send(fd,"1",1,0);
            puts("进入管理模式");
comma:
            puts("login or register ?");
            fgets(command,USEBUFF,stdin);
            /*root登录*/
            if (0 == strncmp(command,"login",5)) {
                while (1) {
                    printf("请输入用户名:");
                    fflush(stdout);
                    fgets(user,USEBUFF,stdin);
                    printf("请输入密码:");
                    fflush(stdout);
                    fgets(pass,USEBUFF,stdin);
                    sprintf(data,"%s %s %s",command,user,pass);
                    send(fd,data,SIZEBUFF,0);
                    recv(fd,data,SIZEBUFF,0);
                    if (0 == strncmp(data,"successful",10)) {
                        printf("登录成功!\n");
                        break;
                    } else {
                        printf("用户名或密码错误,请重新输入!\n");
                        continue;
                    }
                }
            }
            /*root注册*/
            else if (0 == strncmp(command,"register",8)) {
                while (1) {
                    staff_t info;
                    printf("请输入用户名:");
                    fflush(stdout);
                    fgets(user,USEBUFF,stdin);
                    printf("请输入密码:");
                    fflush(stdout);
                    fgets(pass,USEBUFF,stdin);
                    sprintf(data,"%s %s %s",command,user,pass);
                    send(fd,data,SIZEBUFF,0);
                    recv(fd,data,SIZEBUFF,0);
                    if (0 == strncmp(data,"successful",10)) {//输入个人信息
                        printf("请输入姓名:");
                        fflush(stdout);
                        fgets(info.name,USEBUFF,stdin);
                        printf("请输入性别:");
                        fflush(stdout);
                        fgets(info.age,USEBUFF,stdin);
                        printf("请输入地址:");
                        fflush(stdout);
                        fgets(info.address,USEBUFF,stdin);
                        printf("请输入薪资:");
                        fflush(stdout);
                        fgets(info.salary,USEBUFF,stdin);
                        sprintf(data,"%s %s %s %s",info.name,info.age,info.address,info.salary);
                        send(fd,data,SIZEBUFF,0);
                        recv(fd,data,SIZEBUFF,0);
                        if (0 == strncmp(data,"successful",10)){
                            printf("注册成功!\n");
                            goto comma;
                        } else {
                            printf("注册失败!\n");
                            exit(EXIT_FAILURE);
                        }
                        break;
                    } else {
                        printf("用户名或密码错误,请重新输入!\n");
                        continue;
                    }
                }

            } else {
                printf("命令错误,请重新输入!\n");
                goto comma;
            }

            /*登录之后 进入增删改查界面*/
            printf("请选择增添(insert),删除(delete),修改(update),查找(select)),退出(exit)\n");
            printf("输入命令:");
            fflush(stdout);
            fgets(command,USEBUFF,stdin);
            if (0 == strncmp(command,"instert",7)) {

            } else if (0 == strncmp(command,"delete",6)) {

            } else if (0 == strncmp(command,"update",6)) {

            } else if (0 == strncmp(command,"select",6)) {

            } else if (0 == strncmp(command,"exit",4)) {
                close(fd);
                return 0;
            }

        } else if (2 == u){

            send(fd,"2",1,0);
            puts("进入用户模式");
            getchar();
commausr:
            puts("login or register ?");
            fgets(command,USEBUFF,stdin);
            /*user登录*/
            if (0 == strncmp(command,"login",5)) {
                while (1) {
                    printf("请输入用户名:");
                    fflush(stdout);
                    fgets(user,USEBUFF,stdin);
                    printf("请输入密码:");
                    fflush(stdout);
                    fgets(pass,USEBUFF,stdin);
                    sprintf(data,"%s %s %s",command,user,pass);
                    puts(data),
                    send(fd,data,SIZEBUFF,0);
                    recv(fd,data,SIZEBUFF,0);
                    if (0 == strncmp(data,"successful",10)) {
                        printf("登录成功!\n");
                        break;
                    } else {
                        printf("用户名或密码错误,请重新输入!\n");
                        continue;
                    }
                }
            }
            /*user注册*/
            else if (0 == strncmp(command,"register",8)) {
                while (1) {
                    staff_t info;
                    printf("请输入用户名:");
                    fflush(stdout);
                    fgets(user,USEBUFF,stdin);
                    printf("请输入密码:");
                    fflush(stdout);
                    fgets(pass,USEBUFF,stdin);
                    sprintf(data,"%s %s %s",command,user,pass);
                    send(fd,data,SIZEBUFF,0);
                    recv(fd,data,SIZEBUFF,0);
                    if (0 == strncmp(data,"successful",10)) {//输入个人信息
                        printf("请输入姓名:");
                        fflush(stdout);
                        fgets(info.name,USEBUFF,stdin);
                        printf("请输入性别:");
                        fflush(stdout);
                        fgets(info.age,USEBUFF,stdin);
                        printf("请输入地址:");
                        fflush(stdout);
                        fgets(info.address,USEBUFF,stdin);
                        printf("请输入薪资:");
                        fflush(stdout);
                        fgets(info.salary,USEBUFF,stdin);
                        sprintf(data,"%s %s %s %s",info.name,info.age,info.address,info.salary);
                        send(fd,data,SIZEBUFF,0);
                        recv(fd,data,SIZEBUFF,0);
                        if (0 == strncmp(data,"successful",10)){
                            printf("注册成功!\n");
                            goto comma;
                        } else {
                            printf("注册失败!\n");
                            exit(EXIT_FAILURE);
                        }
                        break;
                    } else {
                        printf("用户名或密码错误,请重新输入!\n");
                        continue;
                    }
                }

            } else {
                printf("命令错误,请重新输入!\n");
                goto comma;
            }

            /*登录之后操作*/
            printf("请选择修改(update),查找(select)),退出(exit)\n");
            printf("输入命令:");
            fflush(stdout);
            fgets(command,USEBUFF,stdin);
            if (0 == strncmp(command,"update",6)) {

            } else if (0 == strncmp(command,"select",6)) {

            } else if (0 == strncmp(command,"exit",4)) {
                close(fd);
                return 0;
            }


        } else if (3 == u) {
            close(fd);
            return 0;
        }

    } //while(1)
    close(fd);
    return 0;
}
