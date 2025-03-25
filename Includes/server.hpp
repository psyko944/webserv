#pragma once 

#include "sock.hpp"
#include "epollManager.hpp"
#include "client.hpp"
#include <iostream>
#include <map>



class server
{
private:
    sock _socket;
    epollManager _epollMgr;
    std::map <int, client*> clients;

public:
    server();
    ~server();
    void start();
    int get_serv_fd() const;
    //--------------------------
    void print_clients();
    void delete_client(int fd);
    bool print_request(struct epoll_event *event);
};


