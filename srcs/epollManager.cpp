#include "epollManager.hpp"

#include <stdio.h>

#include <errno.h>


epollManager::epollManager()
{
    memset(&ev, 0, sizeof(ev));
    memset(&events, 0, sizeof(events));
}
epollManager::~epollManager() {}

void epollManager::init(int fd)
{
    if ((_epoll_fd = epoll_create(1)) < 0)
        throw std::runtime_error("epoll create failed");
    add_fd(fd);
}

void epollManager::add_fd(int fd)
{
    std::cout << "FD = " << fd << std::endl;
    ev.events = EPOLLIN | EPOLLET | EPOLLHUP;
    ev.data.fd = fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0)
        throw std::runtime_error("epoll ctl failed1");
}
void epollManager::del_fd(int fd)
{
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
        throw std::runtime_error("epoll ctl failed2");
}

void epollManager::waitingForEvents()
{
    num_events = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
    if (num_events < 0)
        throw std::runtime_error("epoll wait failed");
}
