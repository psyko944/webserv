#include "includes/webServer.hpp"
#include <signal.h>
int g_loop = 1;
webServer::webServer(): epfd(-1), nfds(0), ev(), events(){}
webServer::~webServer(){}
webServer::webServer(const webServer &src)
{
	*this = src;
}

webServer::webServer(std::list<Server> serversList): epfd(-1), nfds(0), ev(), events(), serversList(serversList){}
webServer &webServer::operator=(const webServer &rhs)
{
	if (this != &rhs)
	{
		this->epfd = rhs.epfd;
		this->nfds = rhs.nfds;
		this->ev = rhs.ev;
		std::copy(rhs.events, rhs.events + MAX_EVENTS, events);
		this->serversList = rhs.serversList;
	}
	return *this;
}


std::string errorString()
{
	int err = errno;
	return strerror(err);
}

void	handle_signal(int signal)
{
	if (signal == SIGQUIT || signal == SIGINT || signal == SIGTERM || signal == SIGHUP || signal == SIGTSTP)
		g_loop = 0;
}

void webServer::initEpoll()
{
	epfd = epoll_create(1);
	if (epfd == -1)
		throw std::runtime_error("epoll_create failed");
	memset(&ev, 0, sizeof(ev));
    memset(events, 0, sizeof(struct epoll_event) * MAX_EVENTS);
}


void webServer::setupServers()
{
	for (std::list<Server>::iterator it = serversList.begin(); it != serversList.end(); it++)
	{
		it->initSocket();
		ev.events = EPOLLIN;
		ev.data.fd = it->getSockfd();
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
			throw std::runtime_error("epoll_ctl failed");
		std::cout << GREEN << "Server fd added: " << ev.data.fd << RESET << '\n';
		server_fds.push_back(ev.data.fd);
	}
}

void webServer::closeWebServer()
{
	for (std::vector<int>::iterator it = server_fds.begin(); it != server_fds.end(); it++)
	{
		if (epoll_ctl(epfd, EPOLL_CTL_DEL, *it, NULL) == -1)
		throw std::runtime_error("epoll_ctl_del failed for server_fds");
	}
	for (std::list<Server>::iterator it = serversList.begin(); it != serversList.end(); it++)
	{
		it->closeSocket();
	}
	for(std::vector<int>::iterator it = client_fds.begin(); it != client_fds.end(); it++)
	{
		if (epoll_ctl(epfd, EPOLL_CTL_DEL, *it, NULL) == -1)
			throw std::runtime_error("epoll_ctl_del failed for client_fds");
		close(*it);
	}
	close(epfd);
}

void	webServer::acceptConnection(Server const &server)
{
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int client_fd = accept(server.getSockfd(), (struct sockaddr *)&addr, &addrlen);
	if (client_fd == -1)
		throw std::runtime_error("accept failed");
	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(client_fd);
		throw std::runtime_error("Error: set client socket to non-blocking mode failed");
	}
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = client_fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
	{
		close(client_fd);
		throw std::runtime_error("epoll_ctl failed");
	}
	std::cout << YELLOW << "Client connected on fd: " << client_fd << RESET << '\n';
	client_fds.push_back(client_fd);
}

void webServer::start()
{
	signal(SIGQUIT, handle_signal); // ctrl backslash
	signal(SIGINT, handle_signal); // ctrl c
	signal(SIGTSTP, handle_signal); // ctrl z
	signal(SIGTERM, handle_signal); // si on kill notre pid avec kill -15 PID
	signal(SIGHUP, handle_signal); // si on kill notre pid avec kill -1 PID
	try
	{
		initEpoll();
		setupServers();
		while (g_loop)
		{
			nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
			std::cerr << GREEN << "WebServer wait an event\n" << RESET;
			if (nfds == -1)
			{
				if (errno == EINTR)
					continue;
				throw std::runtime_error("epoll_wait failed");
			}
			std::cerr << MAGENTA << "nfds: " << nfds << RESET <<'\n';
			if (nfds > 0)
			{
				std::cerr << CYAN << "event triggered\n" << RESET;
			}
			for (int i = 0; i < nfds; ++i)
			{
				if (events[i].events & EPOLLIN)
				{
					for (std::list<Server>::iterator it = serversList.begin(); it != serversList.end(); it++)
					{
						if (events[i].data.fd == it->getSockfd())
						{
							acceptConnection(*it);
						}
					}
				}
				else
				{
					//Client socket event: read data.
				}
			}
		}
		closeWebServer();
	}
	catch(const std::exception& e)
	{
		closeWebServer();
		std::cerr << RED << e.what() << '\n';
	}
	
}