#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Webserv.hpp"

static std::string	serverParametrs[] = {"server_name", "listen", "root", "index", "allow_methods", "client_body_buffer_size"};

class Location;

class ServerConfig
{
	private:
		uint16_t						_port;
		in_addr_t						_host; //from #include <machine/types.h>  base type of internet address
		std::string						_server_name;
		// std::string						_root;
		unsigned long					_client_max_body_size;
		//bool							_autoindex;
		//std::set<std::string> 			_allowed_methods;
	public:

		std::map<std::string, Location> _locations;
		std::map<short, std::string>	_error_pages;


		ServerConfig();
		~ServerConfig();
		ServerConfig(const ServerConfig &other);
		ServerConfig &operator=(const ServerConfig & rhs);

		void setServerName(std::string server_name);
		void setHost(std::string parametr);
		//void setRoot(std::string root);
		void setPort(std::string parametr);
		void setClientMaxBodySize(std::string parametr);
		void setErrorPages(std::vector<std::string> &parametr);
		void setLocation(std::string nameLocation, std::vector<std::string> parametr);
		//void setAutoindex(bool autoindex);

		bool isValidHost() const;
		bool isValidErrorPages() const;
		bool isValidLocations() const;

		std::string getServerName();
		uint16_t getPort();
		in_addr_t getHost();
		size_t getClientMaxBodySize();
		//bool getAutoindex();
		//std::string getRoot();
		//std::set<std::string> getAllowedMethods();

		void checkToken(std::string &parametr);

		public:
		class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "SERVER CONFIG ERROR: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};
};

#endif
