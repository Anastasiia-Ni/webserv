#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Webserv.hpp"
#include "HttpRequest.hpp"

class HttpRequest;
class CgiHandler {
	private:
		std::map<std::string, std::string>	_env;
		char**								_ch_env;
		char**								_argv;
		std::string							_cgi_path;
		pid_t								_cgi_pid;
		int								_exit_status;

	public:
		CgiHandler();
		CgiHandler(std::string path);
		~CgiHandler();
		CgiHandler(CgiHandler const &other);
		CgiHandler &operator=(CgiHandler const &rhs);

		void initEnv(HttpRequest& req, const std::vector<Location>::iterator it_loc);
		void execute(HttpRequest& req, int &fd, std::string &, short &error_code);
		void sendHeaderBody(int &pipe_out, int &fd, std::string &);
		void fixHeader(std::string &header);
		std::string setCookie(const std::string& str);

		void setCgiPid(pid_t cgi_pid);
		void setCgiPath(const std::string &cgi_path);

		const std::map<std::string, std::string> &getEnv() const;
		const pid_t &getCgiPid() const;
		const std::string &getCgiPath() const;

		std::string	getAfter(const std::string& path, char delim);
		std::string	getBefore(const std::string& path, char delim);
		std::string	getPathInfo(std::string& path, std::vector<std::string> extensions);
		int	countCookies(const std::string& str);
		int findStart(const std::string path, const std::string delim);
		std::string decode(std::string &path);
};


#endif
