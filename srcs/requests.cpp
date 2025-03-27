#include <request.hpp>
#include <utils.hpp>
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <sstream>
//-------------------------
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "cgiManager.hpp"

void Request::notImplemented()
{
	_errcode = NOT_IMPLEMENTED;
	_responseBody = "<!DOCTYPE html><html>501</html>\r\n";
	_responseHeader = "HTTP/1.1 501 Not Implemented\r\n";
	_responseHeader += "Server: Webserv\r\n";
	_responseHeader += Utils::time_string();
	_responseHeader += "Content-Length: 33\r\n";
	_responseHeader += "Cache-Control: no-store\r\n\r\n";
}

Request::Request(int fd) : test_fd(fd)
{
	char buffer[4096] = {0}; // Body size ici????
	/*int		n = */ read(fd, buffer, sizeof(buffer));

	std::vector<std::string> lines = Utils::split(buffer, "\r\n");
	std::istringstream request_line(lines[0]);
	request_line >> _method >> _path >> _version;
	int i = 1;
	if (lines[0].empty())
		return;
	while (!lines[i].empty())
	{
		size_t sep_pos = lines[i].find(":");
		if (sep_pos != std::string::npos)
		{
			std::string field_name = lines[i].substr(0, sep_pos);
			std::string field_value = lines[i].substr(sep_pos + 2);
			_data[field_name] = field_value;
		}
		i++;
	}
	std::cout << std::string(buffer) << std::endl;
}

void Request::generateResponse()
{
	if (_method == "GET")
		getReq();
	else if (_method == "POST")
		postReq();
	else if (_method == "DELETE")
		deleteReq();
	else
		notImplemented();
}

void Request::send(int fd)
{
	std::cout << "PATH = " << _path << std::endl;
	if (_path == "/scripts/cgi.py")
	{
		cgiManager cgi(_path, test_fd);
		cgi.execute();
		// executeCGI(_path);
		return;
	}
	// std::cerr << "Handling a request :)" << std::endl;
	generateResponse();
	std::cout << _responseHeader << std::endl;
	std::cout << _responseBody << std::endl;
	if (::send(fd, _responseHeader.c_str(), _responseHeader.length(), MSG_NOSIGNAL) < 0)
		perror("send");
	if (::send(fd, _responseBody.c_str(), _responseBody.length(), MSG_NOSIGNAL) < 0)
		perror("send");
}

void Request::getRessourcePath()
{
	_path = "./pages" + _path; // TEMP utiliser path du config
}

//------------------------------------

void Request::executeCGI(const std::string &scriptPath)
{
	std::string new_path = "pages" + scriptPath;
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		perror("pipe failed");
		return;
	}

	int pid = fork();
	if (pid == 0)
	{
		char *const args[] = {(char *)new_path.c_str(), NULL};

		// Prépare les variables d'environnement
		char *const env[] = {
			(char *)"REQUEST_METHOD=GET",
			(char *)"QUERY_STRING=test=123",
			(char *)"CONTENT_TYPE=text/html",
			(char *)"CONTENT_LENGTH=0",
			NULL};

		std::cout << " path dans class cgi = " << new_path << std::endl;
		std::cout << " path dans args[0] = " << args[0] << std::endl;

		dup2(pipefd[1], STDOUT_FILENO);
		dup2(pipefd[0], STDIN_FILENO); // Redirige stdout vers le pipe
		// Processus enfant
		close(pipefd[0]); // Ferme la lecture du pipe
		close(pipefd[1]); // Ferme le descripteur inutilisé

		// Prépare l'argument pour execve
		
		if (execve(new_path.c_str(), args, env) == -1)
		{
			perror("execve failed");
			return;
		}
	}
	else if (pid > 0)
	{
		close(pipefd[1]);
		std::string output;
		char buffer[1024];
		ssize_t bytesRead;
		waitpid(pid, NULL, 0);

		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
		{
			buffer[bytesRead] = '\0';
			output.append(buffer, bytesRead);
		}

		close(pipefd[0]);

		if (!output.empty())
		{
			::send(test_fd, output.c_str(), output.size(), 0);
		}
		else
		{
			std::cerr << "Erreur : Aucune sortie du CGI" << std::endl;
		}
	}
}
