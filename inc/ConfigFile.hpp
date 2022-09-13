#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include "Webserv.hpp"

class ConfigFile {
    private:
		std::string		_path;
		size_t			_size;

    public:
		ConfigFile();
		ConfigFile(std::string const path);
		~ConfigFile();

		int getTypePath(std::string const path);
		static int checkFile(std::string const path);
		std::string	readFile(std::string path);

		std::string getPath();
		int getSize();
};

#endif