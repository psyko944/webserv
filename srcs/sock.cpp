#include "sock.hpp"

sock::sock() : _fd(-1)
{
    memset(&addr, 0, sizeof(addr));
    _len = sizeof(addr);
}

sock::~sock()
{
   
}

int sock::get_fd() const
{
    return _fd;
}

void sock::init_listener_socket()
{
    int opt = 1;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw std::runtime_error("socket failed");
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if ((bind(_fd, (struct sockaddr *)&addr, _len)) < 0)
        throw std::runtime_error("bind socket failed");
    if (listen(_fd, 100) < 0)
        throw std::runtime_error("listen socket failed");
    fcntl(_fd, F_SETFL, O_NONBLOCK);
}

void sock::accept_client(int serv_fd)
{
    _fd = accept(serv_fd, (struct sockaddr *)&addr, &_len);
    if (_fd < 0)
        throw std::runtime_error("accept error");
    fcntl(_fd, F_SETFL, O_NONBLOCK);
}