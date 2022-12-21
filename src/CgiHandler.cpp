#include "../inc/CgiHandler.hpp"

/* Converetor functions */
template<typename T>
std::string toString(const T &arr)
{
	std::ostringstream str;
	str << arr;
	return str.str();
}

unsigned int fromHexToDec(const std::string& nb)
{
	unsigned int x;
	std::stringstream ss;
	ss << nb;
	ss >> std::hex >> x;
	return (x);
}

std::string    fromDecToHex(int num)
{
    std::stringstream sstream;
    sstream << std::hex << num;
    std::string res = sstream.str();
    return res;
}

/* Translation of parameters for QUERY_STRING environment variable */
std::string CgiHandler::decode(std::string &path)
{
	size_t token = path.find("%");
	while (token != std::string::npos)
	{
		if (path.length() < token + 2)
			break ;
		char decimal = fromHexToDec(path.substr(token + 1, 2));
		path.replace(token, 3, toString(decimal));
		token = path.find("%");
	}
	return (path);
}

/* Constructor */
CgiHandler::CgiHandler() {
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = "";
	this->_ch_env = NULL;
	this->_argv = NULL;
}

CgiHandler::CgiHandler(std::string path)
{
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = path;
	this->_ch_env = NULL;
	this->_argv = NULL;
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

void CgiHandler::initEnvCgi(HttpRequest& req, const std::vector<Location>::iterator it_loc)
{
	int poz;

	std::string cgi_exec = ("cgi-bin/" + it_loc->getCgiPath()[0]).c_str();
	// cgi_exec = "/usr/bin/python3";
	// this->_cgi_path = "cgi-bin/env.py";

	// this->_cgi_path = "/dkjk";
	char    *cwd = getcwd(NULL, 0);
	if(_cgi_path[0] != '/')
	{
		std::string tmp(cwd);
		tmp.append("/");
		if(_cgi_path.length() > 0)
			_cgi_path.insert(0, tmp);
	}

	Logger::logMsg(ERROR, CONSOLE_OUTPUT, "CWD IS %s", cwd);
	Logger::logMsg(ERROR, CONSOLE_OUTPUT, "CGI_EXEC PATH IS %s", cgi_exec.c_str() );
	Logger::logMsg(ERROR, CONSOLE_OUTPUT, "CGI_SCRIPT PATH IS %s", _cgi_path.c_str() );


	// this->_env["AUTH_TYPE"] = "Basic";
	if(req.getMethod() == POST)
	{
		char buf[1024];
		std::stringstream out;
		out << req.getBody().length();
		this->_env["CONTENT_LENGTH"] = out.str();
		Logger::logMsg(ERROR, CONSOLE_OUTPUT, "Content-Length Passed to cgi is %s", _env["CONTENT_LENGTH"].c_str());
		
		this->_env["CONTENT_TYPE"] = req.getHeader("content-type");
	}
	
    this->_env["GATEWAY_INTERFACE"] = std::string("CGI/1.1");
	// poz = findStart(this->_cgi_path, "cgi-bin/");
	this->_env["SCRIPT_NAME"] = cgi_exec;//
    this->_env["SCRIPT_FILENAME"] = this->_cgi_path;
    this->_env["PATH_INFO"] = this->_cgi_path;//
    this->_env["PATH_TRANSLATED"] = this->_cgi_path;//
    this->_env["REQUEST_URI"] = this->_cgi_path;//
    // this->_env["QUERY_STRING"] = decode(req.getQuery());
    // this->_env["REMOTE_ADDR"] = req.getHeader("host");
	// poz = findStart(req.getHeader("host"), ":");
    this->_env["SERVER_NAME"] = req.getHeader("host");
    // this->_env["SERVER_PORT"] = (poz > 0 ? req.getHeader("host").substr(poz + 1, req.getHeader("host").size()) : "");
    this->_env["SERVER_PORT"] ="8002";
    // this->_env["SERVER_PORT"] = (poz > 0 ? req.getHeader("host").substr(poz + 1, req.getHeader("host").size()) : "");
    this->_env["REQUEST_METHOD"] = req.getMethodStr();
    // this->_env["HTTP_COOKIE"] = req.getHeader("cookie");
    // this->_env["DOCUMENT_ROOT"] = it_loc->getRootLocation();
	// this->_env["PATH"] = extension; - This thing breaks everything
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SERVER_SOFTWARE"] = "AMANIX";

	std::map<std::string, std::string> request_headers = req.getHeaders();
	for(std::map<std::string, std::string>::iterator it = request_headers.begin();
		it != request_headers.end(); ++it)
	{
		std::string name = it->first;
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);
		std::string key = "HTTP_" + name;
		_env[key] = it->second;
	}
	this->_ch_env = (char **)calloc(sizeof(char *), this->_env.size() + 1);
	std::map<std::string, std::string>::const_iterator it = this->_env.begin();
	for (int i = 0; it != this->_env.end(); it++, i++)
	{
		std::string tmp = it->first + "=" + it->second;
		this->_ch_env[i] = strdup(tmp.c_str());
	}

	// for (int i = 0; this->_ch_env[i]; i++)	//delete PRINT ENV
	// 	std::cout << this->_ch_env[i] << std::endl;

	this->_argv = (char **)malloc(sizeof(char *) * 3);
	this->_argv[0] = strdup(cgi_exec.c_str());
	this->_argv[1] = strdup(this->_cgi_path.c_str());
	this->_argv[2] = NULL;
}


/* initialization environment variable */
void CgiHandler::initEnv(HttpRequest& req, const std::vector<Location>::iterator it_loc)
{
	int poz;
	std::string extension;
	std::string ext_path;

	extension = this->_cgi_path.substr(this->_cgi_path.find("."));
	std::map<std::string, std::string>::iterator it_path = it_loc->_ext_path.find(extension);
    if (it_path == it_loc->_ext_path.end())
        return ;
	ext_path = it_loc->_ext_path[extension];

	this->_env["AUTH_TYPE"] = "Basic";
	this->_env["CONTENT_LENGTH"] = req.getHeader("content-length");
	this->_env["CONTENT_TYPE"] = req.getHeader("content-type");
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	poz = findStart(this->_cgi_path, "cgi-bin/");
	this->_env["SCRIPT_NAME"] = this->_cgi_path;
    this->_env["SCRIPT_FILENAME"] = ((poz < 0 || poz + 8 > this->_cgi_path.size()) ? "" : this->_cgi_path.substr(poz + 8, this->_cgi_path.size())); // check dif cases after put right parametr from the response
    this->_env["PATH_INFO"] = getPathInfo(req.getPath(), it_loc->getCgiExtension());
    this->_env["PATH_TRANSLATED"] = it_loc->getRootLocation() + (this->_env["PATH_INFO"] == "" ? "/" : this->_env["PATH_INFO"]);
    this->_env["QUERY_STRING"] = decode(req.getQuery());
    this->_env["REMOTE_ADDR"] = req.getHeader("host");
	poz = findStart(req.getHeader("host"), ":");
    this->_env["SERVER_NAME"] = (poz > 0 ? req.getHeader("host").substr(0, poz) : "");
    this->_env["SERVER_PORT"] = (poz > 0 ? req.getHeader("host").substr(poz + 1, req.getHeader("host").size()) : "");
    this->_env["REQUEST_METHOD"] = req.getMethodStr();
    this->_env["HTTP_COOKIE"] = req.getHeader("cookie");
    this->_env["DOCUMENT_ROOT"] = it_loc->getRootLocation();
	this->_env["REQUEST_URI"] = req.getPath() + req.getQuery();
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

	this->_argv = (char **)malloc(sizeof(char *) * 3);
	this->_argv[0] = strdup(ext_path.c_str());
	this->_argv[1] = strdup(this->_cgi_path.c_str());
	this->_argv[2] = NULL;
}

/* Pipe and execute CGI */
void CgiHandler::execute(HttpRequest& req, int &fd, std::string &response_content, short &error_code)
{

	if (this->_argv[0] == NULL || this->_argv[1] == NULL)
	{
		error_code = 500;
		return ;
	}
	if (pipe(pipe_in) < 0)
	{
        Logger::logMsg(ERROR, CONSOLE_OUTPUT, "pipe failed");

		error_code = 500;
		return ;
	}
	if (pipe(pipe_out) < 0)
	{
        Logger::logMsg(ERROR, CONSOLE_OUTPUT, "pipe failed");
		
		close(pipe_in[0]);
		close(pipe_in[1]);
		error_code = 500;
		return ;
	}
	this->_cgi_pid = fork();

	if (this->_cgi_pid == 0)
	{
		dup2(pipe_in[0], STDIN_FILENO);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
	
		// if(_req.isBody())
		// 	write(pipe_in[1], _req.getBody(), _reg.getBodyLength());

		this->_exit_status = execve(this->_argv[0], this->_argv, this->_ch_env);
		// std::cout << "EXECVE FAILED \n" << std::endl;
		//this->_exit_status = execve(this->_argv[0], this->_argv, this->_ch_env);
		// std::cout<< "exit: " << this->_exit_status << strerror(errno) << std::endl; //delete

		exit(this->_exit_status);
	}
	else if (this->_cgi_pid > 0){}
	else
	{
        std::cout << "Fork failed" << std::endl;
		error_code = 500;
	}
}


void CgiHandler::sendHeaderBody(int &pipe_out, int &fd, std::string &response_content) // add fd freom responce
{				
	char	tmp[8192];
	int 	res;
	// size_t	pos;
	while( (res = read(pipe_out, tmp, 8192) ) > 0)
	{
		std::cout << res << " bytes read succesfully !" << std::endl;
		response_content.append(tmp, res);
		memset(tmp, 0, sizeof(tmp));
	}
	if(res == 0)
		std::cout << "READ DONE !" << std::endl;
	else if(res < 0)
	{
		std::cout << "Error Reading From CGI Script" << std::endl;
		// Set Error to 500 here .
	}

	// std::cout << "RESPONSE FROM BINARY IS = " << res << std::endl;
	// tmp[res] = '\0';
	// std::string header(tmp);
	// std::string body;

	// fixHeader(header);
	// pos = header.find("\r\n\r\n");
    // if (pos != std::string::npos){
    //     body = header.substr(pos + 4);
    //     header.erase(pos + 4);
    // }

	// write(fd, header.c_str(), header.size());

	// std::cout << "-----------------HEADER-----------------\n"; //delete
	// std::cout << header << std::endl;
	// std::cout << "------------------BODY------------------\n";
	// // std::cout << body << std::endl;
	// size_t num_ch = 0; //delete
	

	// while (res > 0)
	// {
		
	// 	// res = read(pipe_out, tmp, 4000);
	// 	// tmp[res] = '\0';
	// 	// body.append(tmp);
		
	// 	std::string chunk;
    //     chunk = toString(fromDecToHex(body.length()));
    //     chunk += "\r\n";
	
    //     chunk += "\r\n";
    //     // std::cout << chunk << std::endl;
	// 	num_ch++; // delete
    //     write(fd, chunk.c_str(), chunk.length());
	// 	// std::cout << "CHUNK" << std::endl; // delete
    //     if (res <= 0)
    //         break;
    //     res = read(pipe_out, tmp, 400);
    //     tmp[res] = '\0';
    //     body = toString(tmp);
	// } 

	// std::cout << "Number of chunks = " << num_ch << std::endl;
	if (response_content.find("HTTP/1.1") == std::string::npos)
		response_content.insert(0, "HTTP/1.1 200 OK\r\n");
	std::cout << "LENGTH OF RESPONSE IS = " << response_content.length() << std::endl;
	// std::cout << "Response Content IS  " << response_content << std::endl;
	// write(fd, response_content.c_str(), response_content.length());
	// std::cout << "LENGTH OF RESPONSE IS = " << response_content.length() << std::endl;
	// write(fd, "0\r\n\r\n", 5);
	close(fd);
	waitpid(this->_cgi_pid, &this->_exit_status, 0);// maybe wait after finish reading from child
	// if _ext_status is not 0 --> set Error to 500
	if (this->_exit_status != 0)
	{
		response_content.clear();
		response_content = "HTTP/1.1 500 Server Error\r\nContent-Type: text/plain\r\n\r\n 500 Server Error !!";
		return ;
	}
}

void CgiHandler::fixHeader(std::string &header)
{
	std::string	tmp;
	size_t		pos;

	if (header.find("HTTP/1.1") == std::string::npos)
		header.insert(0, "HTTP/1.1 200 OK\r\n");
	if (header.find("Content-Type:") == std::string::npos)
        tmp.append("Content-type: text/html\r\n");
	if (header.find("Transfer-Encoding:") == std::string::npos)
		tmp.append("Transfer-Encoding: chunked\r\n");
    if (header.find("Connection:") == std::string::npos)
        tmp.append("Connection: keep-alive\r\n");
	if (_env.count("HTTP_COOKIE") && header.find("Cookie") == std::string::npos)
		tmp.append("HTTP_COOKIE: " + setCookie(_env["HTTP_COOKIE"]));
	pos = header.find("\r\n\r\n", 10);
	if (pos  == std::string::npos)
    {
		tmp.append("\r\n\r\n");
        pos = header.find("\r\n") + 2;
    }
	else
		tmp.insert(0, "\r\n");
	header.insert(pos, tmp);
}

std::string CgiHandler::setCookie(const std::string& str)
{
	std::string cook = str;
	return (cook);
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

/* Isolation PATH_INFO environment variable */
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
	end = tmp.find("?");
	return (end == std::string::npos ? tmp : tmp.substr(0, end));
}

void		CgiHandler::clear()
{
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = "";
	this->_ch_env = NULL;
	this->_argv = NULL;
	this->_env.clear();
}