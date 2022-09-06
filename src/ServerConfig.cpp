#include "../inc/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	this->_server_name = "";
	this->_host = 0;
	//this->_root = "";
	this->_port = 0;
	this->_client_max_body_size = 0;
	//this->_autoindex = false;
}

ServerConfig::~ServerConfig()
{

}

ServerConfig::ServerConfig(const ServerConfig &other)
{
	if (this != &other)
	{
		this->_server_name = other._server_name;
		//this->_root = other._root;
		this->_port = other._port;
		this->_port = other._port;
		this->_client_max_body_size = other._client_max_body_size;
		//this->_autoindex = other._autoindex;
	}
	return ;
}

ServerConfig &ServerConfig::operator=(const ServerConfig & rhs)
{
	if (this != &rhs)
	{
		this->_server_name = rhs._server_name;
		//this->_root = rhs._root;
		this->_port = rhs._port;
		this->_port = rhs._port;
		this->_client_max_body_size = rhs._client_max_body_size;
		//this->_autoindex = rhs._autoindex;
	}
	return (*this);
}




/* Set functions */

void ServerConfig::setServerName(std::string server_name)
{
	checkToken(server_name);
	this->_server_name = server_name;
}

void ServerConfig::setHost(std::string parametr)
{
	checkToken(parametr);
	this->_host = inet_addr(parametr.data()); // проверить
	std::cout << "Host: " << this->_host << std::endl; // delete
}

// void ServerConfig::setRoot(std::string root)
// {
// 	this->_root = root;
// }

void ServerConfig::setPort(std::string parametr)
{
	unsigned int port = 0;
	checkToken(parametr);
	port = std::stoi((parametr));
	this->_port = (uint16_t) port;
}

void ServerConfig::setClientMaxBodySize(std::string parametr)
{
	unsigned long body_size = 0;
	checkToken(parametr);
	body_size = std::stoi(parametr) * 1000000; //value is given in mb
	this->_client_max_body_size = body_size;

}

void ServerConfig::setErrorPages(std::vector<std::string> parametr) //может ли быть только страница?
{
	std::string path = parametr[parametr.size() - 1];
	for (size_t i = 0; i < parametr.size() - 1; i++)
	{
		short code_error = (short)std::stoi(parametr[i]);
		this->_error_pages.insert(std::make_pair(code_error, path));

	}
}

// void ServerConfig::setAutoindex(bool autoindex)
// {
// 	this->_autoindex = autoindex;
// }

/* Get functions */
std::string ServerConfig::getServerName()
{
	return (this->_server_name);
}

// std::string ServerConfig::getRoot()
// {
// 	return (this->_root);
// }

in_addr_t ServerConfig::getHost()
{
	return (this->_host);
}

uint16_t ServerConfig::getPort()
{
	return (this->_port);
}

size_t ServerConfig::getClientMaxBodySize()
{
	return (this->_client_max_body_size);
}

/* utils */

void ServerConfig::checkToken(std::string &parametr)
{
	size_t pos = parametr.rfind(';');
	if (pos != parametr.size() - 1)
	{
		throw ErrorException("Token is invalid"); // change sentence
	}
	parametr.erase(pos);
}

// bool ServerConfig::getAutoindex()
// {
// 	return (_autoindex);
// }

// std::set<std::string> ServerConfig::getAllowedMethods()
// {
// 	return (_allowed_methods);
// }

