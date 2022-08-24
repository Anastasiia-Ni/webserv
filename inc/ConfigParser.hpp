#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "../inc/Webserv.hpp"

class ConfigParser {
	private:

	public:
		ConfigParser();
		~ConfigParser();

		int createCluster(const std::string &config_file);
		int print();
};

#endif