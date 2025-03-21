#pragma once 

#include "sock.hpp"
#include "epollManager.hpp"
#include <iostream>


class server
{
private:
    sock socket;
    epollManager epollMgr;

public:
    server();
    ~server();
    void start();
    int get_serv_fd() const;
};


