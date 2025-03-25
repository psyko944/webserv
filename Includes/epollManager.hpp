#pragma once

#include <sys/epoll.h>
#include <exception>
#include <stdexcept>
#include <string.h>
#include <iostream>

#define MAX_EVENTS 1024

class epollManager
{
private:
    int _epoll_fd;

public:
    int num_events;
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];
    epollManager();
    ~epollManager();
    void init(int fd);
    void add_fd(int fd);
    void del_fd(int fd);
    void waitingForEvents();
};
