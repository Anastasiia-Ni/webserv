#include "../inc/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	this->_server_name = "";
	this->_host = 0;
	this->_root = "";
	this->_port = 0;
	this->_client_max_body_size = 0;
	this->_sgi_path = "";
	this->initErrorPages();
	this->_index = "";
}

ServerConfig::~ServerConfig() { }

/* copy constructor */
ServerConfig::ServerConfig(const ServerConfig &other)
{
	if (this != &other)
	{
		this->_server_name = other._server_name;
		this->_root = other._root;
		this->_port = other._port;
		this->_port = other._port;
		this->_sgi_path = other._sgi_path;
		this->_client_max_body_size = other._client_max_body_size;
		this->_index = other._index;
		this->_error_pages = other._error_pages;
		this->_locations = other._locations;
	}
	return ;
}

/* assinment operator */
ServerConfig &ServerConfig::operator=(const ServerConfig & rhs)
{
	if (this != &rhs)
	{
		this->_server_name = rhs._server_name;
		this->_root = rhs._root;
		this->_port = rhs._port;
		this->_port = rhs._port;
		this->_sgi_path = rhs._sgi_path;
		this->_client_max_body_size = rhs._client_max_body_size;
		this->_index = rhs._index;
		this->_error_pages = rhs._error_pages;
		this->_locations = rhs._locations;
	}
	return (*this);
}

/* init error page by default */
void ServerConfig::initErrorPages(void)
{
	char dir[1024];
	getcwd(dir, 1024);
	std::string root = dir;

	_error_pages[400] = root + "/website/error_pages/400.html";
	_error_pages[403] = root + "/website/error_pages/403.html";
	_error_pages[404] = root + "/website/error_pages/404.html";
	_error_pages[405] = root + "/website/error_pages/405.html";
	_error_pages[500] = root + "/website/error_pages/500.html";
	_error_pages[505] = root + "/website/error_pages/505.html";
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
	if (parametr == "localhost")
		parametr = "127.0.0.1";
	if (!isValidHost(parametr))
		throw ErrorException("Wrong syntax: host");
	this->_host = inet_addr(parametr.data()); // проверить
	//std::cout << "Host: " << this->_host << std::endl; // delete
	// if (this->_host != INADDR_NONE);
}

void ServerConfig::setRoot(std::string root)
{
	//if (root != "/")
	checkToken(root);

	if (ConfigFile::getTypePath(root) == 2)
	{
		this->_root = root;
		return ;
	}
	char dir[1024];
	getcwd(dir, 1024);
	std::string full_root = dir + root;
	if (ConfigFile::getTypePath(full_root) != 2)
		throw ErrorException("Wrong syntax: root");
	this->_root = full_root;
}

void ServerConfig::setPort(std::string parametr)
{
	unsigned int port = 0;
	checkToken(parametr);

	for (size_t i = 0; i < parametr.length(); i++)
	{
		if (!std::isdigit(parametr[i]))
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
	if (!std::stoi(parametr))
		throw ErrorException("Wrong syntax: client_max_body_size");
	body_size = std::stoi(parametr) * 1000000; //value is given in mb
	this->_client_max_body_size = body_size;
}

void ServerConfig::setIndex(std::string index)
{
	checkToken(index);
	this->_index = index;
}

void ServerConfig::setCgiPass(std::string parametr)
{
	this->_sgi_path = parametr;
}

/* checks if there is such a default error code. If there is, it overwrites the path to the file,
otherwise it creates a new pair: error code - path to the file */
void ServerConfig::setErrorPages(std::vector<std::string> &parametr)
{
	std::string path = parametr[parametr.size() - 1];
	checkToken(path);
	// if (ConfigFile::getTypePath(path) != 2)
	// {
	// 	char dir[1024];
	// 	getcwd(dir, 1024);
	// 	path = dir + path;
	// 	if (ConfigFile::getTypePath(path) != 1)
	// 		throw ErrorException ("incorrect path for error page file: " + path);
	// 	if (ConfigFile::checkFile(path) == -1)
	// 		throw ErrorException ("error page file :" + path + " is not accessible");
	// }
	for (size_t i = 0; i < parametr.size() - 1; i++)
	{
		for (size_t j = 0; j < parametr[i].size(); j++) {
			if (!std::isdigit(parametr[i][j]))
				throw ErrorException("Error code is invalid");
		}
		short code_error = std::stoi(parametr[i]);
		std::map<short, std::string>::iterator it = this->_error_pages.find(code_error);
		if (it != _error_pages.end())
			this->_error_pages[code_error] = path;
		else
			this->_error_pages.insert(std::make_pair(code_error, path));
	}
}

/* set location */
void ServerConfig::setLocation(std::string path, std::vector<std::string> parametr)
{
	Location new_location;
	std::vector<std::string> methods;
	bool flag_methods = false;
	bool flag_autoindex = false;

	// std::vector<std::string>::iterator it = parametr.begin(); // delete
	// std::cout << "-----------" << std::endl;
	// for (; it != parametr.end(); it++)
	// {
	// 	std::cout << *it << std::endl;
	// }
	new_location.setPath(path);
	for (size_t i = 0; i < parametr.size(); i++)
	{
		if (parametr[i] == "root" && (i + 1) < parametr.size())
		{
			if (!new_location.getRootLocation().empty())
				throw ErrorException("Root of location is duplicated");
			checkToken(parametr[++i]);
			// if (ConfigFile::getTypePath(parametr[++i]) == 2)
			// 	new_location.setRootLocation(parametr[i]);
			// else
				new_location.setRootLocation(this->_root + parametr[i]); // check with print
		}
		else if ((parametr[i] == "allow_methods" || parametr[i] == "methods") && (i + 1) < parametr.size())
		{
			if (flag_methods)
				throw ErrorException("Allow_methods of location is duplicated");
			std::vector<std::string> methods;
			while (++i < parametr.size())
			{
				if (parametr[i].find(";") != std::string::npos)
				{
					checkToken(parametr[i]);
					methods.push_back(parametr[i]);
					break ;
				}
				else
				{
					methods.push_back(parametr[i]);
					if (i + 1 >= parametr.size())
						throw ErrorException("Token is invalid");
				}
			}
			new_location.setMethods(methods);
			flag_methods = true;
		}
		else if (parametr[i] == "autoindex" && (i + 1) < parametr.size())
		{
			if (flag_autoindex)
				throw ErrorException("Autoindex of location is duplicated");
			checkToken(parametr[++i]);
			new_location.setAutoindex(parametr[i]);
			flag_autoindex = true;
		}
		else if (parametr[i] == "index" && (i + 1) < parametr.size())
		{
			if (!new_location.getIndexLocation().empty())
				throw ErrorException("Index of location is duplicated");
			checkToken(parametr[++i]);
			new_location.setIndexLocation(parametr[i]);
		}
		else if (parametr[i] == "return" && (i + 1) < parametr.size())
		{
			checkToken(parametr[++i]);
			new_location.setReturn(parametr[i]);
		}
		else if (path[0] == '*' && parametr[i] == "cgi_pass" && (i + 1) < parametr.size())
		{
			if (!new_location.getCgiPass().empty())
				throw ErrorException("Cgi_pass is duplicated");
			checkToken(parametr[++i]);
			std::cout << "here: " << parametr[i] << std::endl; // delete;
			// setCgiPass(parametr[i]); - дает сегфолт, исправить его
			// может удалить его из локейшенов
		}
		else if (i < parametr.size())
		{
			if (i + 1 < parametr.size())
				ServerConfig::checkToken(parametr[++i]);
			else
				throw ErrorException("Parametr in a location is invalid"); // chenge sentence
		}
		// else error config file
	}
	if (new_location.getRootLocation().empty())
		new_location.setRootLocation(this->_root);
	// добавить проверку существования и чтения индекса
	this->_locations.push_back(new_location);
}

// void ServerConfig::setAutoindex(bool autoindex)
// {
// 	this->_autoindex = autoindex;
// }

/* validation of parametrs */
bool ServerConfig::isValidHost(std::string host) const
{
  	if (host.size() > 15)
		return (false);
	struct sockaddr_in sockaddr;
  	return (inet_pton(AF_INET, host.c_str(), &(sockaddr.sin_addr)) ? true : false);
}

bool ServerConfig::isValidErrorPages() const
{
	std::map<short, std::string>::const_iterator it;
	for (it = this->_error_pages.begin(); it != this->_error_pages.end(); it++)
	{
		if (it->first < 100 || it->first > 599)
			return (false);
		// if (ConfigFile::checkFile(it->second) < 0)  //- add this after creation error page files
		// 	return (false);
	}
	return (true);
}

bool ServerConfig::isValidLocations() const
{
	std::vector<Location>::const_iterator it;
	for (it = this->_locations.begin(); it != this->_locations.end(); it++)
	{
		if (it->getPath()[0] == '*')
		{
	// 		std::string path = it->getCgiPass();
	// 		if (path.empty() || ConfigFile::getTypePath(path) < 0)
	// 			return (false);
			continue;
		}
		if (it->getPath()[0] != '/')
			return (false);
	// 	// check path of location ?
	}
	return (true);
}

/* Get functions */
const std::string &ServerConfig::getServerName()
{
	return (this->_server_name);
}

const std::string &ServerConfig::getRoot()
{
	return (this->_root);
}

const in_addr_t &ServerConfig::getHost()
{
	return (this->_host);
}

const uint16_t &ServerConfig::getPort()
{
	return (this->_port);
}

const size_t &ServerConfig::getClientMaxBodySize()
{
	return (this->_client_max_body_size);
}

const std::vector<Location> &ServerConfig::getLocations()
{
	return (this->_locations);
}

const std::string &ServerConfig::getSgiPass()
{
	return (this->_sgi_path);
}

const std::map<short, std::string> &ServerConfig::getErrorPages()
{
	return (this->_error_pages);
}

const std::string &ServerConfig::getIndex()
{
	return (this->_index);
}

/* the two functions below can be used later for response */
const std::string &ServerConfig::getPathErrorPage(short key)
{
	std::map<short, std::string>::iterator it = this->_error_pages.find(key);
	if (it == this->_error_pages.end())
		throw ErrorException("Error_page does not exist");
	return (it->second);
}

const std::vector<Location>::iterator ServerConfig::getLocationKey(std::string key)
{
	std::vector<Location>::iterator it;
	for (it = this->_locations.begin(); it != this->_locations.end(); it++)
	{
		if (it->getPath() == key)
			return (it);
	}
	throw ErrorException("Error: path to location not found");
}

/* utils */
void ServerConfig::checkToken(std::string &parametr)
{
	size_t pos = parametr.rfind(';');
	if (pos != parametr.size() - 1)
		throw ErrorException("Token is invalid"); // change sentence
	parametr.erase(pos);
}

bool ServerConfig::checkLocaitons() const
{
	if (this->_locations.size() < 2)
		return (false);
	std::vector<Location>::const_iterator it1;
	std::vector<Location>::const_iterator it2;
	for (it1 = this->_locations.begin(); it1 != _locations.end() - 1; it1++) {
		for (it2 = it1 + 1; it2 != _locations.end(); it2++) {
			if (it1->getPath() == it2->getPath())
				return (true);
		}
	}
	return (false);
}
