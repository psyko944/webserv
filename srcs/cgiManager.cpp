#include "cgiManager.hpp"
 #include <stdio.h>
cgiManager::cgiManager()
{
}

cgiManager::~cgiManager()
{
}

cgiManager::cgiManager(std::string path, int fd) : _path("./pages" + path), _response(""), _fd(fd), _pid(-1), _bytesRead(0)
{
    _pipefd[0] = -1;
    _pipefd[1] = -1;
    _args[0] = const_cast<char *>(_path.c_str());
    _args[1] = NULL;
    _env[0] = (char *)"REQUEST_METHOD=GET";
    _env[1] = (char *)"QUERY_STRING=test=123";
    _env[2] = (char *)"CONTENT_TYPE=text/html";
    _env[3] = (char *)"CONTENT_LENGTH=0";
    _env[4] = NULL;
    memset(_buffer, 0, sizeof(_buffer));
}

void cgiManager::execute()
{
    std::cout << " path dans class cgi = " << _path << std::endl;
    std::cout << " path dans args[0] = " << _args[0] << std::endl;
    if (pipe(_pipefd) == -1)
        throw std::runtime_error("pipe failed");
    if ((_pid = fork()) == -1)
        throw std::runtime_error("fork failed");
    if (_pid == 0)
    {
        dup2(_pipefd[1], STDOUT_FILENO);
        close(_pipefd[0]);
        close(_pipefd[1]);
        if (execve(_path.c_str(), _args, _env) == -1)
        {
            perror("");
            throw std::runtime_error("execve failed failed");

        }
    }
    else
    {
        close(_pipefd[1]);
        waitpid(_pid, NULL, 0);
        while ((_bytesRead = read(_pipefd[0], _buffer, sizeof(_buffer) - 1)) > 0)
            _response.append(_buffer, _bytesRead);
        close(_pipefd[0]);
        std::cout << "response = " << _response << std::endl;
        send(_fd, _response.c_str(), _response.size(), 0);
    }
}
