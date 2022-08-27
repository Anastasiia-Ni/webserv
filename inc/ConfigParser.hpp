#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "../inc/Webserv.hpp"

#define WHITESPACE "\t\n\v\f\r "

class ConfigParser {
	private:

	public:
		public:
    	//std::vector<ServerConfig> servers;
		std::vector<std::string> server_config;
		unsigned int number_server;
		
		ConfigParser();
		~ConfigParser();

		int createCluster(const std::string &config_file);

		void splitServers(std::string &content);
		void removeComments(std::string &content);
		void removeWhiteSpace(std::string &content);
		size_t findStartServer(size_t start, std::string &content);
		size_t findEndServer(size_t start, std::string &content);

		int print();

		public:
		class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "CONFIG PARSER ERROR: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};
};

// class ServerConfig
// {
// 	private:
// 		port;
// 		client_max_body_size;
// 		root;
// 		autoindex;

// };

#endif