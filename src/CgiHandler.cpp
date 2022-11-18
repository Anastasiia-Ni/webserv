#include "../inc/CgiHandler.hpp"

/* Constructor */

CgiHandler::CgiHandler() {
    std::cout << "CgiHandler constructor" << std::endl;
}

CgiHandler::CgiHandler(std::string path)
{
    this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = path;
	this->_ch_env = NULL;
	this->_argv = NULL;
	std::cout << "CgiHandler: " << this->_cgi_path << std::endl;
}

CgiHandler::~CgiHandler() {

	if (this->_ch_env)
	{
		for (int i = 0; this->_ch_env[i]; i++) // dobavit proverku
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

   	std::cout << "CGI DIED" << std::endl; // delete
//    std::cout << "Exit status was " << WEXITSTATUS(_exit_status) << std::endl;

}

CgiHandler::CgiHandler(const CgiHandler &other)
{
		this->_env = other._env;
		this->_ch_env = other._ch_env;
		this->_argv = other._argv;
		this->_cgi_path = other._cgi_path;
		this->_cgi_pid = other._cgi_pid;
		this->_exit_status = other._exit_status;
}

CgiHandler &CgiHandler::operator=(const CgiHandler &rhs)
{
    if (this != &rhs)
	{
		this->_env = rhs._env;
		this->_ch_env = rhs._ch_env;
		this->_argv = rhs._argv;
		this->_cgi_path = rhs._cgi_path;
		this->_cgi_pid = rhs._cgi_pid;
		this->_exit_status = rhs._exit_status;
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
    return (this->_env);
}

const pid_t &CgiHandler::getCgiPid() const
{
    return (this->_cgi_pid);
}

const std::string &CgiHandler::getCgiPath() const
{
    return (this->_cgi_path);
}

/* initialization environment variable */
void CgiHandler::initEnv(HttpRequest& req, const std::vector<Location>::iterator it_loc)
{
	int poz;

	this->_env["AUTH_TYPE"] = "Basic";
	this->_env["CONTENT_LENGTH"] = req.getHeader("Content-Length");
	this->_env["CONTENT_TYPE"] = req.getHeader("Content-Type");
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	poz = findStart(this->_cgi_path, "cgi-bin/");
	this->_env["SCRIPT_NAME"] = ((poz < 0 || poz + 8 > this->_cgi_path.size()) ? "" : this->_cgi_path.substr(poz + 8, this->_cgi_path.size())); // check dif cases after put right parametr from the response
    this->_env["SCRIPT_FILENAME"] = this->_cgi_path; 
    this->_env["PATH_INFO"] = getPathInfo(req.getPath(), it_loc->getCgiExtension());
    this->_env["PATH_TRANSLATED"] = it_loc->getRootLocation() + "/cgi-bin" + (this->_env["PATH_INFO"] == "" ? "/" : this->_env["PATH_INFO"]); 
    this->_env["QUERY_STRING"] = req.getQuery();
    this->_env["REMOTE_ADDR"] = req.getHeader("Host");
	poz = findStart(req.getHeader("Host"), ":");
    this->_env["SERVER_NAME"] = (poz > 0 ? req.getHeader("Host").substr(0, poz) : "");
    this->_env["SERVER_PORT"] = (poz > 0  ? req.getHeader("Host").substr(poz, req.getHeader("Host").size()) : "");
    this->_env["REQUEST_METHOD"] = req.getMethodStr();
    this->_env["HTTP_COOKIE"] = req.getHeader("Cookie");
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SERVER_SOFTWARE"] = "AMANIX";

	std::vector<std::string> ext_path = it_loc->getCgiPath();
	std::string extension;

	for (std::vector<std::string>::iterator it_ext = ext_path.begin(); it_ext != ext_path.end(); it_ext++)
	{
		extension = *it_ext;
		if (this->_env["PATH_INFO"].find(".py") != std::string::npos && it_ext->find("python") != std::string::npos)
			break ;
		else if (this->_env["PATH_INFO"].find(".sh") != std::string::npos && it_ext->find("bash") != std::string::npos)
			break ;
	}

	std::cout << "extension" << extension << std::endl; // delete

	this->_ch_env = (char **)calloc(sizeof(char *), this->_env.size() + 1);
	std::map<std::string, std::string>::const_iterator it = this->_env.begin();
	for (int i = 0; it != this->_env.end(); it++, i++)
	{
		std::string tmp = it->first + "=" + it->second;
		this->_ch_env[i] = strdup(tmp.c_str());
	}

	for (int i = 0; this->_ch_env[i]; i++)
		std::cout << this->_ch_env[i] << std::endl;

	this->_argv = (char **)malloc(sizeof(char *) * 3);
	// this->_argv[0] = strdup(this->_cgi_path.c_str());
	// this->_argv[1] = strdup(this->_env["SCRIPT_FILENAME"].c_str());
	//this->_argv[2] = NULL;

	std::cout << "extension path: " << extension << std::endl; //delete
	// this->_argv[0] = strdup(extension.c_str());
	this->_argv[0] = strdup("/usr/bin/python3");
	this->_argv[1] = strdup(this->_cgi_path.c_str());
	this->_argv[2] = NULL; //strdup(arg2.c_str());
	// this->_argv[3] = strdup(arg3.c_str());
	// this->_argv[4] = NULL;
}


/* */
void CgiHandler::execute(HttpRequest& req, int &fd)
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
		write(pipe_in[1], req.getHeader("Body").c_str(), atoi(this->_env["CONTENT_LENGTH"].c_str()));
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
		// std::cout<< "argv[0]:" << this->_argv[0] << std::endl; //delete
		// std::cout<< "argv[1]:" << this->_argv[1] << std::endl; //delete
		// std::cout<< "argv[2]:" << this->_argv[2] << std::endl; //delete
		// std::cout<< "argv[3]:" << this->_argv[3] << std::endl; //delete
		
		// if(_req.isBody())
		// 	write(pipe_in[1], _req.getBody(), _reg.getBodyLength());

		this->_exit_status = execve(this->_argv[0], this->_argv, this->_ch_env);
		//this->_exit_status = execve(this->_argv[0], this->_argv, this->_ch_env);
		std::cout<< "exit: " << this->_exit_status << strerror(errno) << std::endl; //delete
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
		sendHeaderBody(pipe_out[0], fd); // add fd from responce
		close(pipe_out[0]);
        close(pipe_in[0]);
	}
	else
        std::cout << "Fork failed" << std::endl; // std::cerr <<
}

void CgiHandler::sendHeaderBody(int &pipe_out, int &fd) // add fd freom responce
{
	char	tmp[4001];
	int 	res;

	res = read(pipe_out, tmp, 4000); // make loop 
	
	// std::string header(tmp);
	// std::string body;
	// size_t      pos;

	// fixHeader(header);
	// pos = header.find("\r\n\r\n");
	// if (pos != std::string::npos)
	// {
	// 	body = header.substr(pos + 4);
	// 	header.erase(pos + 4);
	// }
	// send(fd, header.c_str(), header.size(), 0);
	// std::cout << "-----------------HEADER-----------------\n"; //delete
	// std::cout << header << std::endl;
	// std::cout << "------------------BODY------------------\n";
	// std::cout << body << std::endl;
	//add chunk send

	write(fd, tmp, res);
	close(fd);
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

int CgiHandler::findStart(const std::string path, const std::string delim)
{
	if (path.empty())
		return (-1);
	size_t poz = path.find(delim);
	if (poz != std::string::npos)
		return (poz);
	else
		return (-1);
}


std::string CgiHandler::getPathInfo(std::string& path, std::vector<std::string> extensions)
{
	std::string tmp;
	size_t start, end;

	for (std::vector<std::string>::iterator it_ext = extensions.begin(); it_ext != extensions.end(); it_ext++)
	{
		start = path.find(*it_ext);
		if (start != std::string::npos)
			break ;
	}	
	if (start == std::string::npos)
		return "";
	if (start + 3 >= path.size())
		return "";
	tmp = path.substr(start + 3, path.size());
	if (!tmp[0] || tmp[0] != '/')
		return "";
	//tmp.erase(0, 1);
	end = tmp.find("?");
	return (end == std::string::npos ? tmp : tmp.substr(0, end));	
}

// Convert from Hex to Dec
// unsigned int fromHexToDec(const std::string& nb)
// {
//     unsigned int x;
//     std::stringstream ss;
//     ss << nb;
//     ss >> std::hex >> x;
//     return (x);
// }

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



