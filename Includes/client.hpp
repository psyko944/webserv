#pragma once

#include "sock.hpp"

class client
{
private:
    sock _socket;
    long int _time_of_last_request;
public:
    client();
    client(int serv_fd);
    ~client();
    int get_client_fd() const;
};


