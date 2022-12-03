#include "../inc/CgiHandler.hpp"

//move function in the utils

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
    std::cout << "CgiHandler constructor" << std::endl;
}

CgiHandler::CgiHandler(std::string path)
{
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = path;
	this->_ch_env = NULL;
	this->_argv = NULL;
	// std::cout << "CgiHandler: " << this->_cgi_path << std::endl;
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

   	// std::cout << "CGI DIED" << std::endl; // delete
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
	std::string extension;

	std::vector<std::string> ext_path = it_loc->getCgiPath();
	for (std::vector<std::string>::iterator it_ext = ext_path.begin(); it_ext != ext_path.end(); it_ext++)
	{
		extension = *it_ext;
		if (this->_cgi_path.find(".py") != std::string::npos && it_ext->find("python") != std::string::npos)
			break ;
		else if (this->_cgi_path.find(".sh") != std::string::npos && it_ext->find("bash") != std::string::npos)
			break ;
	}

	this->_env["AUTH_TYPE"] = "Basic";
	this->_env["CONTENT_LENGTH"] = req.getHeader("Content-Length");
	this->_env["CONTENT_TYPE"] = req.getHeader("Content-Type");
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	poz = findStart(this->_cgi_path, "cgi-bin/");
	this->_env["SCRIPT_NAME"] = this->_cgi_path;
    this->_env["SCRIPT_FILENAME"] = ((poz < 0 || poz + 8 > this->_cgi_path.size()) ? "" : this->_cgi_path.substr(poz + 8, this->_cgi_path.size())); // check dif cases after put right parametr from the response
    this->_env["PATH_INFO"] = getPathInfo(req.getPath(), it_loc->getCgiExtension());
    this->_env["PATH_TRANSLATED"] = it_loc->getRootLocation() + (this->_env["PATH_INFO"] == "" ? "/" : this->_env["PATH_INFO"]);
    this->_env["QUERY_STRING"] = decode(req.getQuery());
    this->_env["REMOTE_ADDR"] = req.getHeader("Host");
	poz = findStart(req.getHeader("Host"), ":");
    this->_env["SERVER_NAME"] = (poz > 0 ? req.getHeader("Host").substr(0, poz) : "");
    this->_env["SERVER_PORT"] = (poz > 0 ? req.getHeader("Host").substr(poz + 1, req.getHeader("Host").size()) : "");
    this->_env["REQUEST_METHOD"] = req.getMethodStr();
    this->_env["HTTP_COOKIE"] = req.getHeader("Cookie");
	// this->_env["PATH"] = extension; - This thing breaks everything
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

	// for (int i = 0; this->_ch_env[i]; i++)	//delete PRINT ENV
	// 	std::cout << this->_ch_env[i] << std::endl;

	this->_argv = (char **)malloc(sizeof(char *) * 3);
	this->_argv[0] = strdup(extension.c_str());
	this->_argv[1] = strdup(this->_cgi_path.c_str());
	this->_argv[2] = NULL;

	// std::cout << "ARGV0     " << this->_argv[0] << std::endl;

}


/* */
void CgiHandler::execute(HttpRequest& req, int &fd)
{
	int pipe_in[2], pipe_out[2];
	int out_file;

	//check argv[0] argv[1]

	// if (req.getMethodStr() == "POST")
	// {		
	// 	pipe_in[0] = open(file_name.c_str(), O_RDWR, 0777); // pipe?
	// 	if (pipe_in[0] < 0)
	// 		return ; // with error
	// }

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
	// std::cout<< "pid: " << this->_cgi_pid << std::endl; //delete

	if (this->_cgi_pid == 0)
	{
		dup2(pipe_in[0], STDIN_FILENO);
		dup2(pipe_out[1], STDOUT_FILENO);
		write(pipe_in[1], req.getHeader("Body").c_str(), atoi(this->_env["CONTENT_LENGTH"].c_str()));
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);

		this->_exit_status = execve(this->_argv[0], this->_argv, this->_ch_env);
		//this->_exit_status = execve(this->_argv[0], this->_argv, this->_ch_env);
		// std::cout<< "exit: " << this->_exit_status << strerror(errno) << std::endl; //delete

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
	size_t	pos;

	res = read(pipe_out, tmp, 4000); // make loop
	tmp[res] = '\0';
	std::string header(tmp);
	std::string body;


	std::cout << "-----------------HEADER BEFORE---------\n"; //delete
	std::cout << header << std::endl;


	fixHeader(header);
	pos = header.find("\r\n\r\n");
    if (pos != std::string::npos){
        body = header.substr(pos + 4);
        header.erase(pos + 4);
    }


	write(fd, header.c_str(), header.size());

	std::cout << "-----------------HEADER-----------------\n"; //delete
	std::cout << header << std::endl;
	std::cout << "------------------BODY------------------\n";
	// std::cout << body << std::endl;
	size_t num_ch = 0; //delete

	// std::cout << "PATH INFO   " << this->_env["PATH_INFO"] << std::endl;
    // std::cout << "PATH TRANSLATED   " <<this->_env["PATH_TRANSLATED"] << std::endl;

	// for (int i = 0; this->_ch_env[i]; i++)	//delete PRINT ENV
	// 	std::cout << this->_ch_env[i] << std::endl;

	do
	{

		// res = read(pipe_out, tmp, 4000);
		// tmp[res] = '\0';
		// body.append(tmp);

		std::string chunk;
        chunk = toString(fromDecToHex(body.length()));
        chunk += "\r\n";
        chunk += body;
        chunk += "\r\n";
        std::cout << chunk << std::endl;
		num_ch++; // delete
        write(fd, chunk.c_str(), chunk.length());
		// std::cout << "CHUNK" << std::endl; // delete
        if (res <= 0)
            break;
        res = read(pipe_out, tmp, 4000);
        tmp[res] = '\0';
        body = toString(tmp);
	}while (res > 0);

	std::cout << "Number of chunks = " << num_ch << std::endl;
	std::cout << "CONTENT_TYPE   " << this->_env["CONTENT_TYPE"] << std::endl;
	std::cout << "CONTENT_LENGTH   " << this->_env["CONTENT_LENGTH"] << std::endl;

	write(fd, tmp, res);
	write(fd, "0\r\n\r\n", 5);
	close(fd);
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
	if (_env.count("HTTP_COOKIE") && header.find("Set-cookie") == std::string::npos)
		tmp.append("HTTP_COOKIE: " + setCookie(_env["HTTP_COOKIE"]));
	pos = header.find("\r\n\r\n", 10);
	if (pos  == std::string::npos)
    {
		tmp.append("\r\n\r\n");
        pos = header.find("\r\n") + 2;
    }
	else
		tmp.insert(0, "\r\n");

	// std::cout << "НОМЕР ПОЗИЦИИ: " << pos << "			"<< std::cout; // delete
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