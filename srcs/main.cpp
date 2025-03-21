#include "server.hpp"

int main()
{

    try
    {
        server server;
        server.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}