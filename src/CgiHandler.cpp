#include "../inc/CgiHandler.hpp"
/* Constructor */

CgiHandler::CgiHandler() { 
    std::cout << "CgiHandler constructor" << std::endl;
}

CgiHandler::CgiHandler(std::string msg) // request msg
{
	std::cout << "CgiHandler constructor: " << msg << std::endl;
    this->_cgi_pid = -1;
	this->_exit_status = 0;
}

CgiHandler::~CgiHandler() {
	if (this->_ch_env)
	{
		for (int i = 0; this->_ch_env[i]; i++)
			free(this->_ch_env[i]);
		free(this->_ch_env);
	}
	if (this->_argv)
	{
		for (int i = 0; this->_argv[i]; i++)
			free(_argv[i]);
		free(_argv);
	}
	this->_env.clear();
	// if (waitpid(_cgi_pid, &_exit_status, WNOHANG) == 0)
	// 	kill(_cgi_pid, SIGKILL);
	// if (_response_pipe != -1)
	// 	close(_response_pipe);
	// if (_request_pipe != -1)
	// 	close(_request_pipe);
//    std::cout << "CGI DIED" << std::endl;
//    std::cout << "Exit status was " << WEXITSTATUS(_exit_status) << std::endl;
}

CgiHandler::CgiHandler(const CgiHandler &other)
{
	//TODO
}

CgiHandler &CgiHandler::operator=(const CgiHandler &rhs) 
{
    if (this != &rhs)
	{
		this->_env = rhs._env;
		// TODO
	}	
	return (*this);
}

/*Set functions */

void CgiHandler::setCgiPid(pid_t cgi_pid) 
{
    this->_cgi_pid = cgi_pid;
}

void CgiHandler::setCgiPath(const std::string &cgi_path) 
{
    this->_cgi_path = cgi_path;
}

/* Get functions */
const std::map<std::string, std::string> &CgiHandler::getEnv() const 
{
    return (this-> _env);
}

const pid_t CgiHandler::getCgiPid() const 
{
    return (this->_cgi_pid);
}

const std::string &CgiHandler::getCgiPath() const 
{
    return (this->_cgi_path);
}

/* initialization environment variable */
void CgiHandler::initEnv(std::string msg)
{
	this->_env["AUTH_TYPE"] = ""; // or "Basic"
	this->_env["CONTENT_LENGTH"] = msg; //getHeader("Content-Length") // проверить если не находит то через итератор и find
	this->_env["CONTENT_TYPE"] = msg; // getHeader("Content-Type")
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["SCRIPT_NAME"] = "";//location->getCgiPass()
    this->_env["SCRIPT_FILENAME"] = ""; //full path
    this->_env["PATH_INFO"] = "" ; // Request Uri
    this->_env["PATH_TRANSLATED"] = ""; //root from reguest + this->_env["PATH_INFO"]
    this->_env["QUERY_STRING"] = msg; //getHeader("Query_string");
    this->_env["REMOTE_ADDR"] = msg; //getHeader("Host"); like 172.17.42.1
    this->_env["SERVER_NAME"] = msg; //getBeforeColon(from request ["Host"], ':'); - will write a funct or check getHeader
    this->_env["SERVER_PORT"] = msg; //getAfterColon(from request ["Host"], ':');  - will write a funct or check getHeader
    this->_env["REQUEST_METHOD"] = msg; // getHeader("Request");
    this->_env["HTTP_COOKIE"] = msg; // getHeader("Cookie");
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SERVER_SOFTWARE"] = "AMANIX";

	this->_ch_env = (char **)calloc(sizeof(char *), this->_env.size() + 1);
	std::map<std::string, std::string>::const_iterator it = this->_env.begin();
	for (int i = 0; it != this->_env.end(); it++, i++)
	{
		std::string tmp = it->first + "=" + it->second;
		this->_ch_env[i] = strdup(tmp.c_str());
	}

	// for (int i = 0; this->_ch_env[i]; i++)
	// {
	// 	std::cout << i << " " << this->_ch_env[i] <<std::endl;
	// }
	this->_argv = (char **)calloc(sizeof(char *), 3);
	// this->_argv[0] = strdup(this->_cgi_path.c_str());
	// this->_argv[1] = strdup(this->_env["SCRIPT_FILENAME"].c_str());
	//this->_argv[2] = NULL;

	//for check//
	this->_argv[0] = strdup("/Users/anastasiianifantova/Desktop/ft-server/cgi_bin/env");
	this->_argv[1] = strdup("chto za hernya");
	this->_argv[2] = NULL;

}


/* */
void CgiHandler::execute(std::string msg)
{
	int pipe_in[2], pipe_out[2];

	if (pipe(pipe_in) < 0)
	{
		std::cout << "pipe failed" << std::endl; // properly exit
		return ;
	}
	if (pipe(pipe_out) < 0)
	{
		std::cout << "pipe failed" << std::endl; // properly exit
		close(pipe_in[0]);
		close(pipe_in[1]);
		return ;
	}
	this->_cgi_pid = fork();
	std::cout<< "pid: " << this->_cgi_pid << std::endl; //delete
	if (this->_cgi_pid == 0)
	{
		dup2(pipe_in[0], STDIN_FILENO);
		dup2(pipe_out[1], STDOUT_FILENO);
		//std::cout<< "HERE" << std::endl; //delete
		// записать в pipe_in[1] "Body"
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
		//std::cout<< "argv[0]:" << this->_argv[0] << std::endl; //delete
		this->_exit_status = execve(this->_argv[0], this->_argv, this->_ch_env);
		//std::cout<< "exit: " << this->_exit_status << std::endl; //delete
		exit(this->_exit_status);
	}
	else if (this->_cgi_pid > 0)
	{
		close(pipe_in[1]);
		waitpid(this->_cgi_pid, &this->_exit_status, 0);
		close(pipe_out[1]);
		if (this->_exit_status < 0)
		{
			close(pipe_out[0]);
			close(pipe_in[0]);
			return ;
		}
		sendHeaderBody(pipe_out[0]); // add fd from responce
		close(pipe_out[0]);
        close(pipe_in[0]);
	}
	else
        std::cout << "Fork failed" << std::endl; // std::cerr <<
}

void CgiHandler::sendHeaderBody(int &pipe_out) // add fd freom responce
{
	char	tmp[4001];
	int 	res;
	
	res = read(pipe_out, tmp, 4000);
	tmp[res] = '\0';
	std::string header(tmp);
	std::string body;
	size_t      pos;

	fixHeader(header);
	pos = header.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		body = header.substr(pos + 4);
		header.erase(pos + 4);
	}
	//send(fd, header.c_str(), header.size(), 0);
	std::cout << "-----------------HEADER-----------------\n"; //delete
	std::cout << header << std::endl;
	std::cout << "------------------BODY------------------\n";
	std::cout << body << std::endl;
	//add chunk send
	
	//send(fd, "0\r\n\r\n", 5, 0);
}

void CgiHandler::fixHeader(std::string &header)
{
	std::string	tmp;
	size_t		pos;

	if (header.find("HTTP/1.1") == std::string::npos)
		header.insert(0, "HTTP/1.1 200 OK\r\n");
	if (header.find("Content-type:") == std::string::npos)
        tmp += "Content-type: text/html\r\n";
	if (_env.count("HTTP_COOKIE") && header.find("Set-cookie") == std::string::npos)
		tmp += setCookie(_env["HTTP_COOKIE"]);
	if ((pos = header.find("\r\n\r\n")) == std::string::npos)
    {
        tmp += "\r\n\r\n";
        pos = header.find("\r\n") + 2;
    }
	else
		tmp.insert(0, "\r\n");
	header.insert(pos, tmp);
}

std::string CgiHandler::setCookie(const std::string& str)
{
	std::string cook = str;
	//size_t		pos; 
	return cook;
}
// decode(std::string& path)
// {
//     size_t token = path.find("%");
//     while (token != std::string::npos)
//     {
//         //replace token with ASCII
//         //check if there are at least 2 symbols after %
//         if (path.length() < token + 2)
//             break ;
//         char decimal = fromHexToDec(path.substr(token + 1, 2));
//         path.replace(token, 3, ft::to_string(decimal));
//         token = path.find("%");
//     }
//     return (path);
// }

// Convert from Hex to Dec
unsigned int fromHexToDec(const std::string& nb)
{
    unsigned int x;   
    std::stringstream ss;
    ss << nb;
    ss >> std::hex >> x;
    return (x);
}