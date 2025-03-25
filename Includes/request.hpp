#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include "errcodes.hpp"
# include <sys/socket.h>
# include <sys/types.h>

class Request
{
private:
	std::string							_method;
	std::string							_path;
	std::string							_version;
	std::map<std::string, std::string>	_data;
	std::string							_body;
	
	t_errcodes							_errcode;

	std::string							_responseHeader;
	std::string							_responseBody;

	void	generateResponse();

	void	getRessourcePath();
public:
	Request(int fd);
	~Request() {std::cerr << "Closing Request" << std::endl;};

	void	notImplemented();
	void	getReq();
	void	postReq();
	void	deleteReq();
	void	send(int fd);

};

#endif
