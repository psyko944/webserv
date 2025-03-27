#include <request.hpp>
#include <utils.hpp>
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <mime.hpp>

static t_errcodes getFileContent(std::string path, std::string &res)
{
	std::stringstream ress;
	std::ifstream file;
	size_t mark = path.find("?"); // Pour les requetes avec argument plus tard

	if (mark != std::string::npos)
		path = path.substr(0, mark);
	if (Utils::pathIsDir(path))
		path += "/index.html"; // Dans fichier config
	file.open(path.c_str());
	if (!file.is_open())
	{
		res = "<!DOCTYPE html><html><h1>500</h1><p>Internal server error</p></html>";
		return (INTERNAL_SERVER_ERROR);
	}
	ress << file.rdbuf();
	res = ress.str();
	file.close();
	return OK;
}

static t_errcodes getBody(const std::string &path, std::string &res)
{
	if (access(path.c_str(), F_OK))
	{
		if (getFileContent("./pages/404.html", res)) // Ajouter des checks pour le fichier (Body placeholder pour ces cas?)
			return (INTERNAL_SERVER_ERROR);
		return (NOT_FOUND);
	}
	if (access(path.c_str(), R_OK))
	{
		if (getFileContent("./pages/403.html", res) != OK)
			return (INTERNAL_SERVER_ERROR);
		return (FORBIDDEN);
	}
	std::cout << res << std::endl;
	return (getFileContent(path, res));
}

void Request::getReq()
{
	getRessourcePath();
	_errcode = getBody(_path, _responseBody);


	_responseHeader = "HTTP/1.1 " + Utils::itos(_errcode) + ' ' + get_errcode_string(_errcode) + "\r\n";
	_responseHeader += "Server: Webserv\r\n"; // Config file dependent
	_responseHeader += Utils::time_string();
	_responseHeader += "Content-Length: " + Utils::itos(_responseBody.size()) + "\r\n";
	_responseHeader += "Content-Type: " + get_mime(Utils::getExtension(_path)) + "\r\n";
	_responseHeader += "Cache-Control: no-store\r\n\r\n";

	std::cout << "--------- REQUEST ---------" << std::endl;
	std::cout << "Request for " << _path << std::endl;
	std::cout << _responseHeader;
	std::cout << _responseBody << std::endl;
	std::cout << "------- REQUEST END -------" << std::endl;
}
