#pragma once 

#include "sock.hpp"
#include <iostream>


class server
{
private:
    sock socket;
public:
    server();
    ~server();
    int get_serv_fd() const;
    void init();
};


