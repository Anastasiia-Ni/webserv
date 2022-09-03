#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "../inc/Webserv.hpp"

static std::string	serverParametrs[] = {"server_name", "listen", "root", "index", "allow_methods", "client_body_buffer_size"};

class ServerConfig
{
	private:
		std::string	_server_name;
		std::string	_root;
		size_t		_port;
		size_t		_client_max_body_size;
		bool		_autoindex;

		std::set<std::string> _allowed_methods;


	public:
		ServerConfig();
		~ServerConfig();
		ServerConfig(const ServerConfig &other);
		ServerConfig &operator=(const ServerConfig & rhs);

		void setServerName(std::string server_name);
		void setRoot(std::string root);
		void setPort(size_t root);
		void setClientMaxBodySize(size_t client_max_body_size);
		void setAutoindex(bool autoindex);

		std::string getServerName();
		std::string getRoot();
		size_t getPort();
		size_t getClientMaxBodySize();
		bool getAutoindex();
		std::set<std::string> getAllowedMethods();
};

#endif
