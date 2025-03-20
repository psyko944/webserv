#include "server.hpp"

int main()
{

    try
    {
        server server;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}