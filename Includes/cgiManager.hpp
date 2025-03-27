#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <string.h>
#include <exception>
#include <stdexcept>

class cgiManager
{
private:
    std::string _path;
    std::string _response;
    int _fd;
    int _pid;
    int _bytesRead;
    char * _args[2];
    char * _env[5];
    int _pipefd[2];
    char _buffer[1024];

public:
    cgiManager();
    cgiManager(std::string path, int fd);
    ~cgiManager();
    void execute();
};

