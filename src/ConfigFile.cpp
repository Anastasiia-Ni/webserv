#include "../inc/ConfigFile.hpp"

ConfigFile::ConfigFile() : _size(0) { }

ConfigFile::ConfigFile(std::string const path) : _path(path), _size(0) { }

ConfigFile::~ConfigFile() { }


/* define is path is file(1), folder(2) or something else(3) */
int ConfigFile::getTypePath(std::string const path) 
{
	struct stat	buffer;
	int			result;

	result = stat(path.c_str(), &buffer);
	if (result == 0)
	{
		if (buffer.st_mode & S_IFREG)
		{
			this->_size = buffer.st_size;
			return (1);
		}
		else if (buffer.st_mode & S_IFDIR)
			return (2);
		else
			return (3);
    }
	else
		return (-1);
}

/* checks is the file exists and accessable */
int  ConfigFile::checkFile(std::string const path)
{
	if (access(path.c_str(), 0) != -1)
		return (0);
	else
		return (-1); //throw FileError
}
// access()

std::string ConfigFile::getPath()
{
	return (this->_path);
}

int ConfigFile::getSize()
{
	return (this->_size);
}