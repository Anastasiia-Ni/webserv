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

/* copy constructor */
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

/* assinment operator */
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
	std::cout << "Name: " << this->_server_name << std::endl; // delete
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

	for (size_t i = 0; i < parametr.length(); i++)
	{
		if (parametr[i] < '0' || parametr[i] > '9')
			throw ErrorException("Wrong syntax: port");
	}
	port = std::stoi((parametr));
	if (port < 1 || port > 65636)
		throw ErrorException("Wrong syntax: port");
	this->_port = (uint16_t) port;
}

void ServerConfig::setClientMaxBodySize(std::string parametr)
{
	unsigned long body_size = 0;
	checkToken(parametr);

	for (size_t i = 0; i < parametr.length(); i++)
	{
		if (parametr[i] < '0' || parametr[i] > '9')
			throw ErrorException("Wrong syntax: client_max_body_size");
	}
	body_size = std::stoi(parametr) * 1000000; //value is given in mb
	this->_client_max_body_size = body_size;

}

void ServerConfig::setErrorPages(std::vector<std::string> &parametr) //может ли быть только страница?
{
	std::string path = parametr[parametr.size() - 1];
	checkToken(path);
	for (size_t i = 0; i < parametr.size() - 1; i++)
	{
		short code_error = std::stoi(parametr[i]);
		this->_error_pages.insert(std::make_pair(code_error, path));
	}
	//std::cout << "size error - " << _error_pages.size() << std::endl; //delete
	// std::map<short, std::string>::iterator it = _error_pages.begin();
	// for (; it != _error_pages.end(); it++)
	// {
	// 	std::cout << it->first << " - " << it->second << std::endl;
	// }
}

void ServerConfig::setLocation(std::string path, std::vector<std::string> parametr)
{
	Location new_location;
	std::vector<std::string> methods;

	for (size_t i = 0; i < parametr.size(); i++)
	{
		if (parametr[i] == "root" && (i + 1) < parametr.size())
		{
			checkToken(parametr[++i]);
			new_location.setRoot(parametr[i]);
		}
		if ((parametr[i] == "allow_methods" || parametr[i] == "methods") && (i + 1) < parametr.size())
		{
			while (++i < parametr.size())
			{
				if (parametr[i].find(";") != std::string::npos)
				{
					checkToken(parametr[i]);
					methods.push_back(parametr[i]);
					break ;
				}					
				else	
					methods.push_back(parametr[i]);
			}			
			new_location.setMethods(methods);
		}
		if (parametr[i] == "autoindex" && (i + 1) < parametr.size())
		{
			checkToken(parametr[++i]);
			new_location.setAutoindex(parametr[i]);
		}
		if (parametr[i] == "index" && (i + 1) < parametr.size())
		{			
			checkToken(parametr[++i]);
			new_location.setIndex(parametr[i]);
		}
		if (path[0] == '*' && parametr[i] == "cgi_pass" && (i + 1) < parametr.size())
		{
			checkToken(parametr[++i]);
			new_location.setCgiPass(parametr[i]);
		}
	}
	this->_locations.insert(std::make_pair(path, new_location));
}

// void ServerConfig::setAutoindex(bool autoindex)
// {
// 	this->_autoindex = autoindex;
// }





/* validation of parametrs */
bool ServerConfig::isValidHost() const
{
	return (this->_host != INADDR_NONE);
}

bool ServerConfig::isValidErrorPages() const
{
	std::map<short, std::string>::const_iterator it;
	for (it = this->_error_pages.begin(); it != this->_error_pages.end(); it++)
	{
		if (it->first < 100 || it->first > 599)
			return (false);
		// if (ConfigFile::checkFile(it->second) < 0)  - add this after creation error page files
		// 	return (false);
	}
	return (true);
}

bool ServerConfig::isValidLocations() const
{
	std::map<std::string, Location>::const_iterator it;
	for (it = this->_locations.begin(); it != this->_locations.end(); it++)
	{
		if (it->first[0] == '*')
		{
			std::string path = it->second.getCgiPass();
			if (path.empty() || ConfigFile::checkFile(path.c_str()) < 0)
				return (false);
			continue;
		}
		if (it->first[0] != '/')
			return (false);
		// check root location
	}
	return (true);
}






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

