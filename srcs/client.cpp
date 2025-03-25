#include "client.hpp"

client::client()
{
}
client::client(int serv_fd) : _time_of_last_request(0)
{
    _socket.accept_client(serv_fd);
    (void)_time_of_last_request;
}

client::~client()
{
}

int client::get_client_fd() const
{
    return _socket.get_fd();
}
