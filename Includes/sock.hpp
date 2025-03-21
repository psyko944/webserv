#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>        
#include <netinet/tcp.h>
#include <iostream>
#include <string.h>
#include <exception>
#include <unistd.h>
#include <fcntl.h>

class sock
{
private:
    int _fd;
    socklen_t _len;
    struct sockaddr_in addr;

public:
    sock();
    ~sock();
    int get_fd() const;
    void init_listener_socket();
    void accept_client(int serv_fd);
};
