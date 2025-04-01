#include "includes/Configuration.hpp"
#include <sys/stat.h> 
#include <unistd.h>


bool	isValidFormat(const std::string &filename) {
	struct stat file_stat;
	if (stat(filename.c_str(), &file_stat))
		return false;
	if (file_stat.st_mode & S_IFDIR)
		return false;
	if (file_stat.st_size == 0)
		return false;
	return true;
}

bool	isValidFile(const std::string &filename)
{
	if (!isValidFormat(filename) || access(filename.c_str(), R_OK))
		return false;
	return true;
}


bool	isValidDir(const std::string &path)
{
	struct stat file_stat;
	if (stat(path.c_str(), &file_stat))
		return false;
	return (file_stat.st_mode & S_IFDIR) != 0;
}

std::string formatPath(const std::string &path)
{
	std::string newPath = path;
	if (newPath[0] == '/')
		newPath = newPath.substr(1);
	return newPath;
}