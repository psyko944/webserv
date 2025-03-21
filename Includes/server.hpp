#pragma once 

#include "sock.hpp"
#include "epollManager.hpp"
#include "client.hpp"
#include <iostream>
#include <vector>



class server
{
private:
    sock _socket;
    epollManager _epollMgr;

public:
    server();
    ~server();
    void start();
    int get_serv_fd() const;
};


