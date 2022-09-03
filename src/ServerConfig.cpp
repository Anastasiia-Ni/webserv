#include "../inc/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	this->_server_name = "";
	this->_root = "";
	this->_port = 0;
	this->_client_max_body_size = 0;
	this->_autoindex = false;
}

ServerConfig::~ServerConfig()
{

}

ServerConfig::ServerConfig(const ServerConfig &other)
{
	if (this != &other)
	{
		this->_server_name = other._server_name;
		this->_root = other._root;
		this->_port = other._port;
		this->_client_max_body_size = other._client_max_body_size;
		this->_autoindex = other._autoindex;
	}
	return ;
}

ServerConfig &ServerConfig::operator=(const ServerConfig & rhs)
{
	if (this != &rhs)
	{
		this->_server_name = rhs._server_name;
		this->_root = rhs._root;
		this->_port = rhs._port;
		this->_client_max_body_size = rhs._client_max_body_size;
		this->_autoindex = rhs._autoindex;
	}
	return (*this);
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