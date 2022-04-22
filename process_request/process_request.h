#ifndef PROCESS_REQUEST_H
#define PROCESS_REQUEST_H
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <map>

//请求参数类
class request_parmer
{
private:
    /* data */

    enum METHOD
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATH

    };

    enum ERROR
    {
        handle_error = 0,
        parmer_erroe,
        path_error
    };
    enum STATUE
    {
        html = 0,
        josn
    };

private:
    char *path;
    METHOD method;
    ERROR error;
    STATUE statue;
    request_parmer (*fun)(const request_parmer &re_p);

public:
    static std::list<request_parmer *> *get_function_list(METHOD method)
    {
        switch (method)
        {
        case 0:
            static std::list<request_parmer *> get_function_list;
            return &get_function_list;
            break;

        case 1:
            static std::list<request_parmer *> post_function_list;
            return &post_function_list;
            break;

        default:
            static std::list<request_parmer *> other_function_list;
            return &other_function_list;
            break;
        }
    };

    bool add_function(request_parmer *re_p)
    {
        switch (re_p->method)
        {
        case 0:
            std::list<request_parmer *> *get_function_list = re_p->get_function_list(re_p->method);
            get_function_list->push_back(re_p);
            break;
        case 1:
            std::list<request_parmer *> *post_function_list = re_p->get_function_list(re_p->method);
            post_function_list->push_back(re_p);

        default:
            std::list<request_parmer *> *other_function_list = re_p->get_function_list(re_p->method);
            other_function_list->push_back(re_p);
            break;
        }
        return true;
    };
    static std::list<request_parmer *> *get_process_request_list()
    {
        static std::list<request_parmer *> process_request_list; //处理请求队列
        return &process_request_list;
    };
    bool match_request_to_function(request_parmer *re_p)
    {
        std::list<request_parmer *> *function_list = re_p->get_function_list(re_p->method);
        std::list<request_parmer *>::iterator it;

        for (it = function_list->begin(); it != function_list->end(); ++it)
        {
            request_parmer *rp = *it;
            if (strcmp(re_p->path, rp->path) == 0)
            {
                re_p->fun = rp->fun;
                // TODO::请求处理队列化预计需要线程处理，但前处理使用了线程池，所以此处使用 路径匹配处理函数来处理请求
                //若进行后处理：json 或者 html格式化，则利用处理队列可以再次调用进行统一处理流程
                // std::list<request_parmer *> *process_request_list = re_p->get_process_request_list();
                // process_request_list->push_back(re_p);
                return true;
            };
            ++it;
        }
        return false;
    };

public:
    char *parmer;

public:
    request_parmer(/* args */);
    ~request_parmer();
};

#endif