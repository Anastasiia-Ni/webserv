# include "../inc/ServerManager.hpp"


ServerManager::ServerManager() {}
ServerManager::~ServerManager(){}

/**
 * Start all servers on ports specified in the config file
 */
void    ServerManager::setupServers(std::vector<ServerConfig> servers)
{
    _servers = servers;
    char buf[INET_ADDRSTRLEN];
    bool    serverDub;
    for(std::vector<ServerConfig>::iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
        serverDub = false;
        for(std::vector<ServerConfig>::iterator it2 = _servers.begin(); it2 != it; ++it2)
        {
            if (it2->getHost() == it->getHost() && it2->getPort() == it->getPort())
            {
                it->setFd(it2->getFd());
                serverDub = true;
            }
        }
        if(!serverDub)
            it->setupServer();
        Logger::logMsg(INFO, CONSOLE_OUTPUT, "Server Created -- Host:%s Port:%d", inet_ntop(AF_INET, &it->getHost(), buf, INET_ADDRSTRLEN), it->getPort());
    }
}

/**
 * Runs main loop that goes through all file descriptors from 0 till the biggest fd in the set.
 * - check file descriptors returend from select():
 *      if server fd --> accept new client
 *      if client fd in read_set --> read message from client
 *      if client fd in write_set --> send response to client
 *
 * - servers and clients sockets will be added to _recv_set_pool initially,
 *   after that, when a request is fully parsed, socket will be moved to _write_set_pool
 */
void    ServerManager::runServers()
{
    // servers and clients sockets will be added to _recv_set_pool initially,
    // after that, when a request is fully parsed, socket will be moved to _write_set_pool
    fd_set recv_set_cpy;
    fd_set write_set_cpy;

    _biggest_fd = 0;
    setupSelect();

    struct timeval      timer;
    while(true)
    {
        timer.tv_sec = 1;
        timer.tv_usec = 0;
        recv_set_cpy = _recv_fd_pool;
        write_set_cpy = _write_fd_pool;
        int max_select_fd = _biggest_fd;
        // std::cout << "Biggest FD is " << _biggest_fd << std::endl;
        // for(int i = 0; i < _clients_map.size(); ++i)
        // {
        //     if(_clients_map[i]._response.isCgi())
        //     {
        //         if(_clients_map)
        //     }

        // }
        if( select(_biggest_fd + 1, &recv_set_cpy, &write_set_cpy, NULL, &timer) < 0 )
        {
            std::cerr << " webserv: select error " << strerror(errno) << std::endl;
            exit(1);
            continue;
        }
        // if(_clients_map.empty())
        //     _biggest_fd = _servers.back().getFd();
        // else
        //     _biggest_fd = (--_clients_map.end())->first;

        for (int i = 0; i <= _biggest_fd; ++i)
        {
            if(FD_ISSET(i, &recv_set_cpy) && _servers_map.count(i))
            {
                    // std::cout << "I = " << i << std::endl;
                    // std::cout << "acceptNewConnection()" << std::endl;
                    acceptNewConnection(_servers_map.find(i)->second);

            }
            else if(FD_ISSET(i, &recv_set_cpy) && _clients_map.count(i))
            {
                // std::cout << "I = " << i << std::endl;
                // std::cout << "readRequest()" << std::endl;
                readRequest(i);
            }
            else if(FD_ISSET(i, &write_set_cpy) && _clients_map.count(i))
            {

                int state = _clients_map[i]._response.getCgiState();
                if(state == 1 && FD_ISSET(_clients_map[i]._response._cgi_obj.pipe_in[1], &write_set_cpy))
                {
                    // std::cout << "sendCgiBody()" << std::endl;
                    sendCgiBody(i, _clients_map[i]._response._cgi_obj.pipe_in[1]);
                }
                else if(state == 1 && FD_ISSET(_clients_map[i]._response._cgi_obj.pipe_out[0], &recv_set_cpy))
                {
                    // std::cout << "readCgiResponse()" << std::endl;
                    readCgiResponse(i, _clients_map[i]._response._cgi_obj.pipe_out[0]);
                }
                else if((state == 0 || state == 2)  && FD_ISSET(i, &write_set_cpy))
                {
                    // std::cout << "sendReponse()" << std::endl;
                    sendResponse(i);
                }                   
            }
                
        }
        checkTimeout();
    }
}

/* Checks time passed for clients since last message, If more than CONNECTION_TIMEOUT, close connection */
void    ServerManager::checkTimeout()
{
        for(std::map<int, Client>::iterator it = _clients_map.begin() ; it != _clients_map.end(); ++it)
        {
            if(time(NULL) - it->second.getLastTime() > CONNECTION_TIMEOUT)
            {
                closeConnection(it->first);
                return;
            }
        }
}

/**
 * Accept new incomming connection.
 * Create new Client object and add it to _client_map
 * Add client socket to _recv_fd_pool
*/
void    ServerManager::acceptNewConnection(ServerConfig &serv)
{
    struct sockaddr_in client_address;
    long  client_address_size = sizeof(client_address);
    int client_sock;
    Client  new_client(serv);
    char buf[INET_ADDRSTRLEN];

    if ( (client_sock = accept(serv.getFd(), (struct sockaddr *)&client_address,
     (socklen_t*)&client_address_size)) == -1)
    {
        Logger::logMsg(ERROR, CONSOLE_OUTPUT, "webserv: accept error %s", strerror(errno));
        return ;
    }
    Logger::logMsg(INFO, CONSOLE_OUTPUT, "New Connection From %s, Assigned Socket %d",inet_ntop(AF_INET, &client_address, buf, INET_ADDRSTRLEN), client_sock);

    addToSet(client_sock, _recv_fd_pool);

    if(fcntl(client_sock, F_SETFL, O_NONBLOCK) < 0)
    {
        Logger::logMsg(ERROR, CONSOLE_OUTPUT, "webserv: fcntl error %s", strerror(errno));
        removeFromSet(client_sock, _recv_fd_pool);
        close(client_sock);
        return;
    }

    new_client.setSocket(client_sock);
    if(_clients_map.count(client_sock) != 0)
        _clients_map.erase(client_sock);
    _clients_map.insert(std::make_pair(client_sock, new_client));
}

/*
    initialize recv+write fd_sets and add all server listening sockets to _recv_fd_pool.
*/
void    ServerManager::setupSelect()
{
    FD_ZERO(&_recv_fd_pool);
    FD_ZERO(&_write_fd_pool);

    // adds servers sockets to _recv_fd_pool set
    for(std::vector<ServerConfig>::iterator it = _servers.begin(); it != _servers.end(); ++it)
    {
        //Now it calles listen() twice on even if two servers have the same host:port
        if (listen(it->getFd(), 512) == -1)
        {
            Logger::logMsg(ERROR, CONSOLE_OUTPUT, "webserv: listen error: %s   Closing....", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if(fcntl(it->getFd(), F_SETFL, O_NONBLOCK) < 0)
        {
            Logger::logMsg(ERROR, CONSOLE_OUTPUT, "webserv: fcntl error: %s   Closing....", strerror(errno));
            exit(EXIT_FAILURE);
        }
        addToSet(it->getFd(), _recv_fd_pool);
        _servers_map.insert(std::make_pair(it->getFd(), *it));
    }
    // at this stage _biggest_fd will belong to the last server created.
    _biggest_fd = _servers.back().getFd();
}

void    ServerManager::closeConnection(int i)
{
    if(FD_ISSET(i, &_write_fd_pool))
        removeFromSet(i, _write_fd_pool);
    if(FD_ISSET(i, &_recv_fd_pool))
        removeFromSet(i, _recv_fd_pool);
    close(i);
    _clients_map.erase(i);
}

/**
 * Build the response and send it to client.
 * If no error was found in request and Connection header value is keep-alive,
 * connection is kept, otherwise connection will be closed.
 */
void    ServerManager::sendResponse(int &i)
{
    int bytes_sent;
    std::string response = _clients_map[i].getResponse();
 
    if(response.length() >= 8192)
        bytes_sent = write(i, response.c_str(), 8192);
    else
        bytes_sent = write(i, response.c_str(), response.length());
    
    if(bytes_sent < 0)
    {
        // std::cout << "ERROR SENDING () :" << strerror(errno) << std::endl;
        closeConnection(i);
    }
    else if(bytes_sent == 0 || bytes_sent == response.length())
    {
        // std::cout << "Done SENDING () :" << strerror(errno) << std::endl;
        Logger::logMsg(INFO, CONSOLE_OUTPUT, "Response Sent To %d, status = %d", i, _clients_map[i]._response.getCode());

        if(_clients_map[i].keepAlive() == false || _clients_map[i].requestError() || _clients_map[i]._response.getCgiState())
        {
            std::cout << "Connection Closed !" << std::endl;
            closeConnection(i);
        }
        else
        {
            removeFromSet(i, _write_fd_pool);
            addToSet(i, _recv_fd_pool);
            _clients_map[i].clearClient();
        }
    }
    else
    {
        // std::cout << "Done SENDING () :" << strerror(errno) << std::endl;
        // std::cout << "Bytes sent are : " << bytes_sent << std::endl;
        _clients_map[i].updateTime();
        _clients_map[i]._response.cutRes(bytes_sent);
    }
        
    // std::ofstream  file("text_response.txt", std::ios_base::app);
    // file << resp << std::endl;
    

}

void    ServerManager::assignServer(int &i)
{
    
    for (std::vector<ServerConfig>::iterator it = _servers.begin();
        it != _servers.end(); ++it)
    {
        if(_clients_map[i].getHost() == it->getHost() &&
           _clients_map[i].getPort() == it->getPort() &&
           _clients_map[i].getReqServerName() == it->getServerName())
        {
            _clients_map[i].setServer(*it);
            return;
        }

    }
}

/**
 * - Reads data from client and feed it to the parser.
 * Once parser is done or an error was found in the request,
 * socket will be moved from _recv_fd_pool to _write_fd_pool
 * and response will be sent on the next iteration of select().
 */
void    ServerManager::readRequest(int &i)
{
    char    buffer[8192];
    int     bytes_read = 0;
    
    bytes_read = read(i, buffer, sizeof(buffer)); // set limit to the total request size to avoid infinite request size.

    if(bytes_read == 0)
    {
        Logger::logMsg(INFO, CONSOLE_OUTPUT, "webserv: Client %d Closed Connection", i);
        closeConnection(i);
        return;
    }
    if(bytes_read < 0)
    { 
        Logger::logMsg(ERROR, CONSOLE_OUTPUT, "webserv: fd %d read error %s", i, strerror(errno));
        closeConnection(i);
        return;
    }
    else if(bytes_read != 0)
    {
        _clients_map[i].feedData(buffer, bytes_read);
        memset(buffer, 0, sizeof(buffer));
        _clients_map[i].updateTime();
    }

    if (_clients_map[i].parsingCompleted() || _clients_map[i].requestError()) // 1 = parsing completed and we can work on the response.
    {
        if(_clients_map[i].requestError())
            Logger::logMsg(INFO, CONSOLE_OUTPUT, "Request From %d Parased --- Error In Request..", i);
        else
            Logger::logMsg(INFO, CONSOLE_OUTPUT, "Request From %d Parased --- Method: %s  Path: %s", i,
                _clients_map[i].request.getMethodStr().c_str(), _clients_map[i].request.getPath().c_str());
        assignServer(i);
        _clients_map[i].buildResponse();
        
        if(_clients_map[i]._response.getCgiState())
            addToSet(_clients_map[i]._response._cgi_obj.pipe_in[1],  _write_fd_pool);

        removeFromSet(i, _recv_fd_pool);
        addToSet(i, _write_fd_pool); // move socket i from recive fd_set to write fd_set so response can be sent on next iteration
    }
}

/*********************************************/

void    ServerManager::sendCgiBody(int &i, int &pipe_in)
{
    int bytes_sent;
    std::string response = _clients_map[i].getResponse();
    std::string req_body = _clients_map[i].request.getBody();

    if(req_body.length() >= 8192)
        bytes_sent = write(pipe_in, req_body.c_str(), 8192);
    else
        bytes_sent = write(pipe_in, req_body.c_str(), req_body.length());
    
    if(bytes_sent < 0)
    {
        removeFromSet(_clients_map[i]._response._cgi_obj.pipe_in[1], _write_fd_pool);
        close(_clients_map[i]._response._cgi_obj.pipe_in[1]);
        close(_clients_map[i]._response._cgi_obj.pipe_out[1]);
        _clients_map[i]._response.setErrorResponse(500);
    }
    else if(bytes_sent == 0 || bytes_sent == req_body.length())
    {
        // std::cout << "Done SENDING () :" << strerror(errno) << std::endl;
        removeFromSet(_clients_map[i]._response._cgi_obj.pipe_in[1], _write_fd_pool);
        addToSet(_clients_map[i]._response._cgi_obj.pipe_out[0],  _recv_fd_pool);
        close(_clients_map[i]._response._cgi_obj.pipe_in[1]);
        close(_clients_map[i]._response._cgi_obj.pipe_out[1]);
    }
    else
    {
        _clients_map[i].request.cutReqBody(bytes_sent);
    } 
}

void    ServerManager::readCgiResponse(int &i, int &pipe_out)
{
    char    buffer[8192];
    int     bytes_read = 0;
    
    bytes_read = read(_clients_map[i]._response._cgi_obj.pipe_out[0], buffer, sizeof(buffer)); // set limit to the total request size to avoid infinite request size.
    if(bytes_read == 0)
    {
        removeFromSet(_clients_map[i]._response._cgi_obj.pipe_out[0], _recv_fd_pool);
        close(_clients_map[i]._response._cgi_obj.pipe_in[0]);
        close(_clients_map[i]._response._cgi_obj.pipe_out[0]);
        _clients_map[i]._response.setCgiState(2);        
        if (_clients_map[i]._response._response_content.find("HTTP/1.1") == std::string::npos)
		    _clients_map[i]._response._response_content.insert(0, "HTTP/1.1 200 OK\r\n");
        return;
    }
    else if(bytes_read < 0)
    { 
        std::cerr << "Error Reading From CGI Script ! " << strerror(errno) << std::endl;
        removeFromSet(_clients_map[i]._response._cgi_obj.pipe_out[0], _recv_fd_pool);
        close(_clients_map[i]._response._cgi_obj.pipe_in[0]);
        close(_clients_map[i]._response._cgi_obj.pipe_out[0]);
        _clients_map[i]._response.setCgiState(2);        
        _clients_map[i]._response.setErrorResponse(500);
        return;
    }
    else
    {
		std::cout << bytes_read << " bytes read succesfully !" << std::endl;
		_clients_map[i]._response._response_content.append(buffer, bytes_read);
		memset(buffer, 0, sizeof(buffer));
    }
}

void	ServerManager::addToSet(int i, fd_set &set)
{
    FD_SET(i, &set);
    if(i > _biggest_fd)
        _biggest_fd = i;
}

void	ServerManager::removeFromSet(int i, fd_set &set)
{
    FD_CLR(i, &set);
    if(i == _biggest_fd)
        _biggest_fd--;
}
