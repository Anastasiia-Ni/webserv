#include "../inc/ConfigParser.hpp"

ConfigParser::ConfigParser() { }

ConfigParser::~ConfigParser() { }

int ConfigParser::createCluster(const std::string &config_file) 
{
	ConfigFile	file(config_file);

	if (file.getTypePath(file.getPath()) != 1 || file.checkFile(file.getPath())) // check file is valid
		return (-1);    // throw error
	return (0);

}

int ConfigParser::print() 
{
	std::cout<< "------------- Config -------------" << std::endl;

	return (0);
}