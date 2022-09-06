#include "../inc/ConfigParser.hpp"

ConfigParser::ConfigParser()
{
	this->_nb_server = 0;
}

ConfigParser::~ConfigParser() { }



int ConfigParser::print()
{
	std::cout << "------------- Config -------------" << std::endl;
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "Server #" << i + 1 << std::endl;
		std::cout << "Server name: " << _servers[i].getServerName() << std::endl;
		std::cout << "Host: " << _servers[i].getHost() << std::endl;
		std::cout << "Port: " << _servers[i].getPort() << std::endl;
		std::cout << "Max BSize: " << _servers[i].getClientMaxBodySize() << std::endl;
		std::cout << "Error pages: " << std::endl;
		std::map<short, std::string>::iterator it = _servers[i]._error_pages.begin();
		while (it != _servers[i]._error_pages.end())
		{
			std::cout << it->first << " - " << it->second << std::endl;
			++it;
		}
		std::cout << "-----------------------------" << std::endl;
	}
	return (0);
}

int ConfigParser::createCluster(const std::string &config_file)
{
	std::string		content;
	ConfigFile		file(config_file);

	if (file.getTypePath(file.getPath()) != 1)
		throw ErrorException("File is invalid");
	if (file.checkFile(file.getPath()) == -1)
		throw ErrorException("File is not accessible");
	content = file.readFile(config_file);
	if (content.empty())
		throw ErrorException("File is empty");
	removeComments(content);
	removeWhiteSpace(content);
	splitServers(content);
	if (this->_server_config.size() != this->_nb_server)
		throw ErrorException("Somthing with size"); //rewrite the sentence
	for (size_t i = 0; i < this->_nb_server; i++)
	{
		ServerConfig server;
		createServer(this->_server_config[i], server);
		this->_servers.push_back(server);
	}
	return (0);
}

/*remove comments from char # to \n */
void ConfigParser::removeComments(std::string &content)
{
	size_t pos;

	pos = content.find('#');
	while (pos != std::string::npos)
	{
		size_t pos_end;
		pos_end = content.find('\n', pos);
		content.erase(pos, pos_end - pos);
		pos = content.find('#');
	}

}

/* deleting whitespaces in the start, end and in the content if more than one */
void ConfigParser::removeWhiteSpace(std::string &content)
{
	size_t	i = 0;

	while (content[i] && isspace(content[i]))
		i++;

	content = content.substr(i);
	// for (i = 0; content[i]; i++) // will think need it
	// {
	// 	if (isspace(content[i]) && content[i + 1] && isspace(content[i + 1]))
	// 		content.erase(i, 1);
	// }
	i = content.length() - 1;
	while (i > 0 && isspace(content[i]))
		i--;
	content = content.substr(0, i + 1);
}

/* spliting servers on separetly strings in vector */
void ConfigParser::splitServers(std::string &content)
{
	size_t start = 0, end = 1; //, count_server = 0;

	if (content.find("server", 0) == std::string::npos)
		throw ErrorException("Server did not find");

	while (start != end && start < content.length())
	{
		start = findStartServer(start, content);
		// if (start == 0)
		// {
		// 	std::cout << "no server" << std::endl; //throw
		// 	return ;
		// }
		end = findEndServer(start, content);
		if (start == end)
			throw ErrorException("problem with scope");
		this->_server_config.push_back(content.substr(start, end - start + 1));
		this->_nb_server++;
		start = end + 1;
	}
	std::cout << "size: " << _nb_server << std::endl; //delete
}

/* finding a server begin and return the index of { start of server */
size_t ConfigParser::findStartServer (size_t start, std::string &content)
{
	size_t i;

	for (i = start; content[i]; i++)
	{
		if (content[i] == 's')
			break ;
		if (!isspace(content[i]))
			throw  ErrorException("Wrong character out of server scope{}");
	}
	if (!content[i])
		return (start);
	if (content.compare(i, 6, "server") != 0)
		throw ErrorException("Wrong character out of server scope{}");
	i += 6;
	while (content[i] && isspace(content[i]))
		i++;
	if (content[i] == '{')
		return (i);
	else
		throw  ErrorException("Wrong character out of server scope{}");

}

/* finding a server end and return the index of } end of server */
size_t ConfigParser::findEndServer (size_t start, std::string &content)
{
	size_t i, scope = 0;

	for (i = start + 1; content[i]; i++)
	{
		if (content[i] == '{')
			scope++;
		if (content[i] == '}')
		{
			if (!scope)
				return (i);
			scope--;
		}
	}
	return (start);
}

std::vector<std::string> splitParametrs(std::string line, std::string sep)
{
	std::vector<std::string>	str;
	std::string::size_type start, end;

	start = end = 0;
	while (1)
	{
		end = line.find_first_of(sep, start);
		if (end == std::string::npos)
			break;
		std::string tmp = line.substr(start, end - start);
		str.push_back(tmp);
		start = line.find_first_not_of(sep, end);
		if (start == std::string::npos)
			break;
	}
	return (str);
}

void ConfigParser::createServer(std::string &config, ServerConfig &server)
{
	std::vector<std::string>parametrs;

	parametrs = splitParametrs(config += ' ', std::string(" \n\t"));

	for (size_t i = 0; i < parametrs.size(); i++)
	{
		if (parametrs[i] == "listen" && (i + 1) < parametrs.size())
		{
			server.setHost(parametrs[++i]);
		}
	// 	if (parametrs[i] == "location")
	// 	{
	// 		std::string	nameLocation;
	// 		// check directive
	// 		i++;
	// 		if (parametrs[i] == "{" || parametrs[i] == "}")
	// 			throw  ErrorException("Wrong character in server scope{}");
	// 		nameLocation = parametrs[i];
	// 		i++;
	// 		if (!parseLocation(parametrs, i))
	// 			throw  ErrorException("Wrong character in server scope{}");
	// 		//server.setLocation(nameLocation);
	// 		if (i < parametrs.size() && parametrs[i] == "}")
	// 			continue ;
	// 	}
		if (parametrs[i] == "port" && (i + 1) < parametrs.size())
		{
			server.setPort(parametrs[++i]);
		}
		if (parametrs[i] == "error_page" && (i + 1) < parametrs.size())
		{
			std::vector<std::string> codes;
			while (++i < parametrs.size())
			{
				codes.push_back(parametrs[i]);
				if (parametrs[i].find(';') != std::string::npos)
					break;
				if (i + 1 >= parametrs.size())
					throw ErrorException("Wrong character out of server scope{}"); //check the case
			}
			server.setErrorPages(codes);
		}
		if (parametrs[i] == "client_max_body_size" && (i + 1) < parametrs.size())
		{
			server.setClientMaxBodySize(parametrs[++i]);
		}
		if (parametrs[i] == "server_name" && (i + 1) < parametrs.size())
		{
			server.setServerName(parametrs[++i]);
		}
	// 	std::cout << parametrs[i] << std::endl; // delete
	}

	// идем по каждому слову
	// если location

	(void) server;
	//проверить обязательные параметры, если ок создаем и заполняем. может проверить раньше??

	// проверяем и заполняем дополнительные
}

int ConfigParser::parseLocation(std::vector<std::string> &parametrs, size_t &pos)
{
	//TODO
	return (parametrs[pos] == "}");
}

int	ConfigParser::stringCompare(std::string str1, std::string str2, size_t pos)
{
	size_t	i;

	i = 0;
	while (pos < str1.length() && i < str2.length() && str1[pos] == str2[i])
	{
		pos++;
		i++;
	}
	if (i == str2.length() && pos <= str1.length() && (str1.length() == pos || isspace(str1[pos])))
		return (0);
	return (1);
}

int ConfigParser::isValidAdress(std::string parametr)
{
	(void)parametr;
	// size_t index;
	// std::string ip;
	// std::string port;

	// index = parametr.find(":");
	// if (parametr.find(":") != std::string::npos)
	// {
	// 	ip = parametr.substr(0, index);
	// 	port = parametr.substr(index + 1);
	// }
	// else
	// {
	// 	// ip =
	// 	port = parametr.substr();
	// }
	return (0);
}

int ConfigParser::isValidPort(std::string parametr)
{
	size_t	port;

	for (size_t i = 0; i < parametr.length(); i++)
	{
		if (parametr[i] < '0' || parametr[i] > '9')
			throw ErrorException("Wrong syntax: port");
	}
	port = atoi(parametr.c_str());
	if (!port || port > 65636)
		throw ErrorException("Wrong syntax: port");
	return (0);
}

int ConfigParser::isValidMethods(std::string parametr)
{
	(void)parametr;
	return (0);
}
// int isValidLocation(std::string location)
// int isValidRoot(std::string root)