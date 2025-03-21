#pragma once

#include "sock.hpp"

class client
{
private:
    sock _socket;
    long int time_of_last_request;
public:
    client();
    ~client();
};


