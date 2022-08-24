#ifndef CONFIGFile_HPP
#define CONFIGFile_HPP

#include "../inc/Webserv.hpp"

class ConfigFile {
    private:
		std::string		_path;
		size_t			_size;

    public:
		ConfigFile();
		ConfigFile(std::string const path);
		~ConfigFile();

		int getTypePath(std::string const path);
		int checkFile(std::string const path);

		std::string getPath();
		int getSize();
};

#endif