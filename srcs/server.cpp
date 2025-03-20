#include "server.hpp"


server::server() {
    socket.init_listener_socket();
}

server::~server(){
}

void server::init(){


}
int server::get_serv_fd() const
{
    return (socket.get_fd());
}