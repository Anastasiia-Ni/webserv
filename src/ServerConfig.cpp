#include "../inc/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	// this->_server_name;
	// this->_root;
	// this->_port;
	// this->_client_max_body_size;
	// this->_autoindex;
}

ServerConfig::~ServerConfig()
{

}




/* Set functions */

void ServerConfig::setServerName(std::string server_name)
{
	this->_server_name = server_name;
}

void ServerConfig::setRoot(std::string root)
{
	this->_root = root;
}

void ServerConfig::setPort(size_t port)
{
	this->_port = port;
}

void ServerConfig::setClientMaxBodySize(size_t client_max_body_size)
{
	this->_client_max_body_size = client_max_body_size;
}

void ServerConfig::setAutoindex(bool autoindex)
{
	this->_autoindex = autoindex;
}

/* Get functions */
std::string ServerConfig::getServerName()
{
	return (this->_server_name);
}

std::string ServerConfig::getRoot()
{
	return (this->_root);
}

size_t ServerConfig::getPort()
{
	return (this->_port);
}

size_t ServerConfig::getClientMaxBodySize()
{
	return (this->_client_max_body_size);
}

bool ServerConfig::getAutoindex()
{
	return (_autoindex);
}

std::set<std::string> ServerConfig::getAllowedMethods()
{
	return (_allowed_methods);
}