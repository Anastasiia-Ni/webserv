#include "../inc/ConfigParser.hpp"

ConfigParser::ConfigParser() { }

ConfigParser::~ConfigParser() { }

int ConfigParser::print() 
{
	std::cout<< "------------- Config -------------" << std::endl;

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
	
	//TODO...


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
	size_t start = 0, end = 1, count_server = 0;

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
		if (start == end) //check is it happens
		{
			std::cout << "problem with scope" << std::endl; // throw
			return ;
		}
		std::string server_config1 = content.substr(start, end - start + 1); //refactor
		this->server_config.push_back(server_config1);
		std::cout << server_config1 << std::endl; // delete
		count_server++;
		start = end + 1;
	}
	std::cout << "size: " << count_server << std::endl; //delete
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
        throw ErrorException("Wrong character out of server scope{}\n");
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
