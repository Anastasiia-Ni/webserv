# include "../inc/Client.hpp"

Client::Client() : _total_bytes_read(0) {}

Client::~Client() {}

Client::Client(ServerConfig &server): _total_bytes_read(0)
{
    _response.setServer(server);
    _request.setMaxBodySize(_server.getClientMaxBodySize());
    _last_msg_time = time(NULL);
}

void    Client::setSocket(int &sock)
{
    _client_socket = sock;
}

void    Client::setAddress(sockaddr_in &addr)
{
    _client_address =  addr;
}

int     Client::getSocket()
{
    return (_client_socket);
}

HttpRequest     &Client::getRequest()
{
    return (_request);
}

struct sockaddr_in    Client::getAddress()
{
    return (_client_address);
}

size_t      Client::getTotalBytes()
{
    return(_total_bytes_read);
}

time_t     Client::getLastTime()
{
    return _last_msg_time;
}

void        Client::feedData(char *data, size_t size)
{
    _request.feed(data, size);
}

bool        Client::parsingCompleted()
{
    return (_request.parsingCompleted());
}

short       Client::requestError()
{
    return (_request.errorCode());
}

void        Client::clearRequest()
{
    _request.clear();
}

bool        Client::keepAlive()
{
    return (_request.keepAlive());
}

void        Client::buildResponse()
{
    _response.setRequest(_request);
    _response.buildResponse();
}

char     *Client::getResponse()
{
    return (_response.getRes());
}

size_t          Client::getResponseLength()
{
    return (_response.getLen());
}

void             Client::clearResponse()
{
    _response.clearResponse();
}

int              Client::getResponseCode()
{
    return (_response.getCode());
}


void             Client::printReq()
{
    _request.printMessage();
}

void             Client::updateTime()
{
    _last_msg_time = time(NULL);
}