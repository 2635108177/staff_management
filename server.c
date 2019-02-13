#include "socketinit.h"
#include <pthread.h>
#include <string.h>
#include <sqlite3.h>
pthread_t servpthread;
typedef struct 
{
    char id[32];
    char name[32];
    char age[32];
    char address[32];
    char salary[32];
}staff_t;

void *pthread_handle(void *arg)
{
    int fd_accept = (long)arg;
    int ret;
    char data[SIZEBUFF];
    struct pollfd client[2];
    char command[SIZEBUFF];
    char user[SIZEBUFF];
    char pass[SIZEBUFF];
    char repass[SIZEBUFF];
    sqlite3 *db;        
    char **resultp;
    int nrow;
    int ncol;
    int i;
    staff_t info;
    ret = sqlite3_open("staffinfo.db", &db);
	if(ret != SQLITE_OK)
	{
        printf("error :%s\n",sqlite3_errmsg(db));
        exit(-1);  
	}	
    char *sql = "create table staff( \
            ID integer primary key autoincrement,\
            NAME    ,\
            AGE     ,\
            ADDRESS ,\
            SALARY  \
            )";
    char *reg = "create table reg(\
                 USER TEXT ,\
                 PASS TEXT \
                 )";
    char *rootreg = "create table rootreg(\
                 USER TEXT ,\
                 PASS TEXT \
                 )";

    sqlite3_exec(db,sql,NULL,NULL,NULL);
    sqlite3_exec(db,reg,NULL,NULL,NULL);
    sqlite3_exec(db,rootreg,NULL,NULL,NULL);
                    ncol = 0;
    
    
    while (1) {

        recv(fd_accept,data,SIZEBUFF,0);
        printf("%s\n",data);
        
        /*进入root操作界面*/

        if (0 == strncmp(data,"1",1)) {
            
            /*选择register,login还是修改密码*/
rootrepeat:            
            recv(fd_accept,data,SIZEBUFF,0);
            sscanf(data,"%s %s %s %s",command,user,pass,repass);
           
            if (0 == strncmp(command,"register",8)) {      //注册
                
                /*检查用户名是否有重复*/
                sprintf(data,"SELECT USER FROM rootreg WHERE USER = '%s';",user);
                sqlite3_get_table(db,data,&resultp,&nrow,&ncol,NULL);
                if (0 !=nrow) {
                    printf("该用户已经存在,请重新输入用户名!\n");
                    goto rootrepeat;
                }

                /*注册账号*/
                sprintf(data,"INSERT INTO rootreg (USER,PASS) VALUES ('%s','%s');",user,pass);
                sqlite3_exec(db,data,NULL,NULL,NULL);
                puts("register successful!");

                /*填写个人基本信息*/
                recv(fd_accept,data,SIZEBUFF,0);
                sscanf(data,"%s %s %s %s",info.name,info.age,info.address,info.salary);
                printf("%s %s %s %s\n",info.name,info.age,info.address,info.salary);
                sprintf(data,"INSERT INTO staff VALUES (null,'%s','%s','%s','%s');",\
                                        info.name,info.age,info.address,info.salary);
                sqlite3_exec(db,data,NULL,NULL,NULL);

                goto rootrepeat;
            } else if (0 == strncmp(command,"login",5)) {  //登录
                
                /*检查用户名或密码有无错误*/
                sprintf(data,"SELECT PASS FROM rootreg WHERE USER = '%s';",user);
                sqlite3_get_table(db,data,&resultp,&nrow,&ncol,NULL);
                printf("nrow = %d,ncol = %d \n",nrow,ncol);
                if (0 == nrow) {
                    printf("用户名错误,请重新输入!\n");
                    goto rootrepeat;
                } else if (0 != strcmp(resultp[1],pass)) {
                    printf("密码错误,请重新输入!\n");
                    goto rootrepeat;
                }
                puts("login successful!");
            
            } else if (0 == strcmp(command,"update")) { //修改密码
            
                /*检查用户名或密码有无错误*/
                sprintf(data,"SELECT PASS FROM rootreg WHERE USER = '%s';",user);
                sqlite3_get_table(db,data,&resultp,&nrow,&ncol,NULL);
                printf("nrow = %d,ncol = %d \n",nrow,ncol);
                if (0 == nrow) {
                    printf("用户名错误,请重新输入!\n");
                    goto rootrepeat;
                } else if (0 != strcmp(resultp[1],pass)) {
                    printf("密码错误,请重新输入!\n");
                    goto rootrepeat;
                }

                sprintf(data,"UPDATE rootreg SET PASS = \"%s\" WHERE USER = \"%s\";",repass,user);
                sqlite3_exec(db,data,NULL,NULL,NULL);
                puts("update successful!");
                goto rootrepeat;
            }

            /*登录之后进入增删改查选择界面*/
            while (0 != strncmp(command,"exit",4)) {
                recv(fd_accept,data,SIZEBUFF,0);
                sscanf(data,"%s %s %s %s %s %s",command,info.id,info.name,info.age,info.address,info.salary);

                if (0 == strcmp(command,"insert")){     //增

                    sprintf(data,"INSERT INTO staff VALUES (null,'%s','%s','%s',%s);",\
                            info.name,info.age,info.address,info.salary);
                    sqlite3_exec(db,data,NULL,NULL,NULL);
                    puts("insert successful!");

                } else if(0 == strcmp(command,"delete")) {//删

                    sprintf(data,"DELETE FROM staff WHERE ID = '%s';",info.id);
                    sqlite3_exec(db,data,NULL,NULL,NULL);
                    puts("delete successful!");
                } else if (0 == strcmp(command,"update")) {//改

                    sprintf(data,"UPDATE staff SET NAME = '%s',AGE = '%s',ADDRESS = '%s',SALARY = '%s' WHERE ID = '%s';",\
                            info.name,info.age,info.address,info.salary,info.id);
                    sqlite3_exec(db,data,NULL,NULL,NULL);
                    puts("update successful!");

                } else if (0 == strcmp(command,"select")) {//查

                    sprintf(data,"SELECT * FROM staff WHERE ID = '%s';",info.id);//按照id单个查询
                    sqlite3_get_table(db,data,&resultp,&nrow,&ncol,NULL);
                    sprintf(data,"ID:%s 姓名:%s 年龄:%s 地址:%s 薪资%s",resultp[5],resultp[6],resultp[7],resultp[8],resultp[9]);
                    send(fd_accept,data,SIZEBUFF,0);
                    puts("select successful!");

                }
            }
        } 
        
        /*进入user操作界面*/
        else if (0 == strncmp(data,"2",1)) {
userepeat:           
            recv(fd_accept,data,SIZEBUFF,0);
            sscanf(data,"%s %s %s %s",command,user,pass,repass);
            
            /*选择register还是login*/
            if (0 == strcmp(command,"register")) {  //注册
                
                /*检查用户名*/
                sprintf(data,"SELECT USER FROM reg WHERE USER = '%s';",user);
                sqlite3_get_table(db,data,&resultp,&nrow,&ncol,NULL);
                if (0 !=nrow) {
                    printf("该用户已经存在,请重新输入用户名!\n");
                    goto userepeat;
                }

                /*注册账号*/
                sprintf(data,"INSERT INTO reg (USER,PASS) VALUES ('%s','%s');",user,pass);
                sqlite3_exec(db,data,NULL,NULL,NULL);
                puts("register successful!");

                /*填写个人基本信息*/
                recv(fd_accept,data,SIZEBUFF,0);
                sscanf(data,"%s %s %s %s",info.name,info.age,info.address,info.salary);
                sprintf(data,"INSERT INTO staff VALUES (null,'%s','%s','%s',%s);",\
                                        info.name,info.age,info.address,info.salary);
                sqlite3_exec(db,data,NULL,NULL,NULL);

                goto userepeat;
            } else if (0 == strcmp(command,"login")) {//登录
                /*检查用户名或密码有无错误*/
                sprintf(data,"SELECT PASS FROM reg WHERE USER = '%s';",user);
                sqlite3_get_table(db,data,&resultp,&nrow,&ncol,NULL);
                printf("nrow = %d,ncol = %d \n",nrow,ncol);
                if (0 == nrow) {
                    printf("用户名错误,请重新输入!\n");
                    goto rootrepeat;
                } else if (0 != strcmp(resultp[1],pass)) {
                    printf("密码错误,请重新输入!\n");
                    goto userepeat;
                }
                puts("login successful!");
            }

            /*登录成功进入查询修改界面,以及密码修改和退出*/
            while (1) {
                recv(fd_accept,data,SIZEBUFF,0);
                sscanf(data,"%s %s %s %s %s %s",command,info.id,info.name,info.age,info.address,info.salary);
                if (0 == strcmp(command,"select")) {    //查询信息
                    if ((0 != strncmp(info.id,"*",1)) && (0 != strncmp(info.id,"#",1))) {//按照id单个查询
                    
                        sprintf(data,"SELECT * FROM staff WHERE ID = '%s';",info.id);
                        sqlite3_get_table(db,data,&resultp,&nrow,&ncol,NULL);
                        sprintf(data,"ID:%s 姓名:%s 年龄:%s 地址:%s 薪资%s",resultp[5],resultp[6],resultp[7],resultp[8],resultp[9]);
                        send(fd_accept,data,SIZEBUFF,0);
                    } else if (0 == strncmp(info.id,"*",1)) {                           //查询所有的信息
                        
                        sprintf(data,"SELECT * FROM staff;");
                        sqlite3_get_table(db,data,&resultp,&nrow,&ncol,NULL);
                        for (i = 0; i < (nrow + 1) * ncol; i += 5) {
                            sprintf(data,"%10s %10s %10s %10s %10s",resultp[i],resultp[i+1],resultp[i+2],resultp[i+3],resultp[i+4]);
                            send(fd_accept,data,SIZEBUFF,0);
                        }

                    } else if (0 == strncmp(info.id,"#",1)) {                           //查询奇数id
                        sprintf(data,"SELECT * FROM staff;");
                        sqlite3_get_table(db,data,&resultp,&nrow,&ncol,NULL);
                        for (i = 0; i < (nrow + 1) * ncol; i += 5) {
                            if (0 == atoi(resultp[i]) % 2)
                            {
                                if (0 != i) {
                                    continue;    
                                }
                            }
                            sprintf(data,"%10s %10s %10s %10s %10s",resultp[i],resultp[i+1],resultp[i+2],resultp[i+3],resultp[i+4]);
                            send(fd_accept,data,SIZEBUFF,0);
                        }
                    }

                } else if (0 == strcmp(command,"update")) {//修改信息

                    sprintf(data,"UPDATE staff SET NAME = '%s',AGE = '%s',ADDRESS = '%s',SALARY = '%s' WHERE ID = '%s';",\
                            info.name,info.age,info.address,info.salary,info.id);
                    sqlite3_exec(db,data,NULL,NULL,NULL);

                } else if (0 == strcmp(command,"passupdate")) {//修改密码
                    
                    recv(fd_accept,data,SIZEBUFF,0);
                    sscanf(data,"%s %s %s",user,pass,repass);

                    /*检查用户名是否存在*/
                    sprintf(data,"SELECT USER FROM reg WHERE USER = '%s';",user);
                    sqlite3_get_table(db,data,&resultp,&nrow,&ncol,NULL);
                    if (0 == nrow) {
                        printf("该用户不存在,请重新输入!\n");
                        continue;
                    }

                    sprintf(data,"UPDATE reg SET PASS = '%s' WHERE USER = '%s';",repass,user);
                    sqlite3_exec(db,data,NULL,NULL,NULL);
                    puts("update successful!");
                    goto userepeat;
                } else if (0 == strcmp(command,"exit")) {
                    return 0;
                }
            }
        }
    }
}

int main(int argc, const char *argv[])
{
    int fd,fd_accept;
    int ret;
    struct sockaddr_in peeraddr;
    int len = sizeof(peeraddr);
    fd = socket_init();
    while (1) {

        ret = listen(fd,10);
        if (0 > ret) {
            ERROR_EXIT("listen");
        }
        fd_accept = accept(fd,(struct sockaddr *)&peeraddr,&len);
        if (0 > fd_accept) {
            ERROR_EXIT("accept");
        }
        printf("come from --%d--%s--\n",ntohs(peeraddr.sin_port),inet_ntoa(peeraddr.sin_addr));
        ret = pthread_create(&servpthread,NULL,pthread_handle,(void *)(long)fd_accept);
        if (0 > ret) {
            ERROR_EXIT("pthread");
        }
    }

    close(fd);
    close(fd_accept);
    return 0;
}
