# include "../inc/ServerManager.hpp"


ServerManager::ServerManager() {}
ServerManager::~ServerManager(){}

/**
 * Start all servers on ports specified in the config file
 */
void    ServerManager::setupServers(std::vector<ServerConfig> servers)
{
    _servers = servers;
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
    timer.tv_sec = 0;
    timer.tv_usec = 0;
    while(true)
    {
        recv_set_cpy = _recv_fd_pool;
        write_set_cpy = _write_fd_pool;
        if( select(FD_SETSIZE, &recv_set_cpy, &write_set_cpy, NULL, &timer) < 0 )
        {
            std::cerr << " webserv: select error " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        if(_clients_map.empty())
            _biggest_fd = _servers.back().getFd();
        else
            _biggest_fd = (--_clients_map.end())->first;

        for (int i = 0; i <= _biggest_fd; ++i)
        {
            // std::cerr << "BIGGEST FD  = " << _biggest_fd << std::endl;
                // std::cerr << "i write = " << i << std::endl;
            if(FD_ISSET(i, &write_set_cpy))
            {
                // std::cerr << "i write = " << i << std::endl;
                sendResponse(i);
            }
            else if(FD_ISSET(i, &recv_set_cpy))
            {
                // std::cerr << "i read = " << i << std::endl;

                if(_servers_map.count(i))
                    acceptNewConnection(_servers_map.find(i)->second);
                else
                    readRequest(i);
            }
        }
        checkTimeout();
    }
}

void    ServerManager::checkTimeout()
{
        for(std::map<int, Client>::iterator it = _clients_map.begin() ; it != _clients_map.end(); ++it)
        {
            if(time(NULL) - it->second.getLastTime() > 10)
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


    if ( (client_sock = accept(serv.getFd(), (struct sockaddr *)&client_address,
     (socklen_t*)&client_address_size)) == -1)
    {
        std::cerr << " webserv: accept error " << strerror(errno) <<std::endl;
        exit(EXIT_FAILURE);
    }

    //Assgin Client to Server here

    FD_SET(client_sock, &_recv_fd_pool);

    if(fcntl(client_sock, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << " webserv: fcntl error" << strerror(errno) <<std::endl;
        exit(EXIT_FAILURE);
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
        std::cerr << " LISTEN" << std::endl;

        if (listen(it->getFd(), 512) == -1)
        {
            std::cerr << " webserv: listen error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        if(fcntl(it->getFd(), F_SETFL, O_NONBLOCK) < 0)
        {
            std::cerr << " webserv: fcntl error" << strerror(errno) <<std::endl;
            exit(EXIT_FAILURE);
        }
        FD_SET(it->getFd(), &_recv_fd_pool);
        _servers_map.insert(std::make_pair(it->getFd(), *it));
    }
    // at this stage _biggest_fd will belong to the last server created.
    _biggest_fd = _servers.back().getFd();
}

void    ServerManager::closeConnection(int i)
{
    if(FD_ISSET(i, &_write_fd_pool))
        FD_CLR(i, &_write_fd_pool);
    if(FD_ISSET(i, &_recv_fd_pool))
        FD_CLR(i, &_recv_fd_pool);
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
    _clients_map[i].buildResponse();
    char *resp = _clients_map[i].getResponse();
    send(i, resp, _clients_map[i].getResponseLength(), 0);
    std::ofstream  file("text_response.txt", std::ios_base::app);
    file << resp << std::endl;
    
    if(_clients_map[i].keepAlive() == false || _clients_map[i].requestError())
        closeConnection(i);
    else
    {
        FD_CLR(i, &_write_fd_pool);
        FD_SET(i, &_recv_fd_pool);
        _clients_map[i].clearRequest();
        _clients_map[i].clearResponse();
    }
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
    std::cout << "FD is " << i << std::endl;
    std::ofstream  file("text.txt", std::ios_base::app);
    file << buffer << std::endl;
    if(bytes_read == 0)
        closeConnection(i);
    if(bytes_read < 0)
    {
        std::cerr << "fd= " << i << "- webserv1: read error" << strerror(errno) << std::endl;
        closeConnection(i);
    }
    else if(bytes_read != 0)
    {
        _clients_map[i].feedData(buffer, bytes_read);
        memset(buffer, 0, sizeof(buffer));
        _clients_map[i].updateTime();
    }

    if (_clients_map[i].parsingCompleted() || _clients_map[i].requestError()) // 1 = parsing completed and we can work on the response.
    {
        assignServer(i);
        FD_CLR(i, &_recv_fd_pool);
        FD_SET(i, &_write_fd_pool); // move socket i from recive fd_set to write fd_set so response can be sent on next iteration
    }
}
