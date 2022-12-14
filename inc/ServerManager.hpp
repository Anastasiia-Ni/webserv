                                                           #ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

#include "Webserv.hpp"
#include "Client.hpp"
#include "Response.hpp"


static char internal_server_error[] =
"HTTP/1.1 500 Internal Server Error\r\n"
"Content-Length = 124\r\n"
"Content-Type = text/plain\r\n\r\n"
"<html>\r\n"
"<head><title>500 Internal Server Error</title></head>\r\n"
"<body>\r\n"
"<center><h1>500 Internal Server Error</h1></center>\r\n"
;

/**
 * ServerManager
 * * operates the webserver and is responsible for
 * - runing servers with configration extracted from config file
 * - establishing new connections with clients and receive/send requests/responses.
 */
class ServerManager
{
    public:                 
        ServerManager();
        ~ServerManager();
        void    setupServers(std::vector<ServerConfig>);
        void    runServers();
        
    private:
        std::vector<ServerConfig> _servers;
        std::map<int, ServerConfig> _servers_map;
        std::map<int, Client> _clients_map;
        fd_set     _recv_fd_pool;
        fd_set     _write_fd_pool;
        int        _biggest_fd;

        void acceptNewConnection(ServerConfig &);
        void setupSelect();
        void readRequest(int &);
        void sendResponse(int &);
        void checkTimeout();
        void closeConnection(int);
        void assignServer(int &);
        void sendCgiBody(int &, int &);
        void readCgiResponse(int &, int &);
        void addToSet(int , fd_set &);
        void removeFromSet(int , fd_set &);
};


#endif // SERVERMANAGER_HPP