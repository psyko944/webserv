#include <request.hpp>
#include <utils.hpp>
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <sstream>

void	Request::notImplemented()
{
	_errcode = NOT_IMPLEMENTED;
	_responseBody = "<!DOCTYPE html><html>501</html>\r\n";
	_responseHeader = "HTTP/1.1 501 Not Implemented\r\n";
	_responseHeader += "Server: Webserv\r\n";
	_responseHeader += Utils::time_string();
	_responseHeader += "Content-Length: 33\r\n";
	_responseHeader += "Cache-Control: no-store\r\n\r\n";
}

Request::Request(int fd)
{
	char	buffer[4096] = {0}; // Body size ici????
	/*int		n = */ read(fd, buffer, sizeof(buffer));

	std::vector<std::string>	lines = Utils::split(buffer, "\r\n");
	std::istringstream	request_line(lines[0]);
	request_line >> _method >> _path >> _version;
	int	i = 1;
	if (lines[0].empty())
		return ;
	while (!lines[i].empty()) {
		size_t	sep_pos = lines[i].find(":");
		if (sep_pos != std::string::npos) {
			std::string	field_name = lines[i].substr(0, sep_pos);
			std::string	field_value = lines[i].substr(sep_pos + 2);
			_data[field_name] = field_value;
		}
		i++;
	}
	std::cout << std::string(buffer) << std::endl;

}

void	Request::generateResponse()
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

void	Request::send(int fd)
{
	// std::cerr << "Handling a request :)" << std::endl;
	generateResponse();
	std::cout << _responseHeader << std::endl;
	std::cout << _responseBody << std::endl;
	if (::send(fd, _responseHeader.c_str(), _responseHeader.length(), MSG_NOSIGNAL) < 0)
		perror("send");
	if (::send(fd, _responseBody.c_str(), _responseBody.length(), MSG_NOSIGNAL) < 0)
		perror("send");	
}

void	Request::getRessourcePath()
{
	_path = "./pages/" + _path; // TEMP utiliser path du config
	
}
