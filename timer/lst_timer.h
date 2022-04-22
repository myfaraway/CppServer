#ifndef LST_TIMER
#define LST_TIMER

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>

#include <time.h>
#include "../log/log.h"

class util_timer;

struct client_data //客户端定时器
{
    sockaddr_in address;
    int sockfd;
    util_timer *timer;
};

class util_timer //定时器链表节点
{
public:
    util_timer() : prev(NULL), next(NULL) {}

public:
    time_t expire; //期待时间，超时时间辍

    void (*cb_func)(client_data *);
    client_data *user_data;
    util_timer *prev;
    util_timer *next;
};

class sort_timer_lst //定时器链表
{
public:
    sort_timer_lst();
    ~sort_timer_lst();

    void add_timer(util_timer *timer);    //添加定时器
    void adjust_timer(util_timer *timer); //调整timer的位置
    void del_timer(util_timer *timer);
    void tick(); //链表上卸下所有超时定时器，关闭客户端连接

private:
    void add_timer(util_timer *timer, util_timer *lst_head); //插入定时器

    util_timer *head;
    util_timer *tail;
};

class Utils
{
public:
    Utils() {}
    ~Utils() {}

    void init(int timeslot);

    //对文件描述符设置非阻塞
    int setnonblocking(int fd);

    //将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
    void addfd(int epollfd, int fd, bool one_shot, int TRIGMode);

    //信号处理函数
    static void sig_handler(int sig);

    //设置信号函数
    void addsig(int sig, void(handler)(int), bool restart = true);

    //定时处理任务，重新定时以不断触发SIGALRM信号
    void timer_handler();

    void show_error(int connfd, const char *info);

public:
    static int *u_pipefd;
    sort_timer_lst m_timer_lst; //定时器链表
    static int u_epollfd;       // epfd：由epoll_create产生的epoll句柄
    int m_TIMESLOT;             ////最小超时单位
};

void cb_func(client_data *user_data);

#endif
