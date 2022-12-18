# include "../inc/Client.hpp"

Client::Client()
{
    _last_msg_time = time(NULL);
    _total_bytes_read = 0;
    bytes_to_send = 0;
    total_bytes_sent = 0;
    _total_bytes_read = 0;
}


Client::~Client() {}

/* copy constructor */
Client::Client(const Client &other)
{
	if (this != &other)
	{
		this->_client_socket = other._client_socket;
		this->_client_address = other._client_address;
		this->request = other.request;
		this->_response = other._response;
		this->_server = other._server;
		this->_total_bytes_read = other._total_bytes_read;
		this->_last_msg_time = other._last_msg_time;

	}
	return ;
}

/* assinment operator */
Client &Client::operator=(const Client & rhs)
{
	if (this != &rhs)
	{
		this->_client_socket = rhs._client_socket;
		this->_client_address = rhs._client_address;
		this->request = rhs.request;
		this->_response = rhs._response;
		this->_server = rhs._server;
		this->_total_bytes_read = rhs._total_bytes_read;
		this->_last_msg_time = rhs._last_msg_time;
	}
	return (*this);
}

Client::Client(ServerConfig &server)
{
    setServer(server);
    request.setMaxBodySize(_server.getClientMaxBodySize());
    _last_msg_time = time(NULL);
    _total_bytes_read = 0;
    bytes_to_send = 0;
    total_bytes_sent = 0;
    _total_bytes_read = 0;
}

void    Client::setSocket(int &sock)
{
    _client_socket = sock;
}

void    Client::setAddress(sockaddr_in &addr)
{
    _client_address =  addr;
}

void    Client::setServer(ServerConfig &server)
{
    _server = server;
    _response.setServer(server);
}

int     Client::getSocket()
{
    return (_client_socket);
}

HttpRequest     &Client::getRequest()
{
    return (request);
}

const in_addr_t       &Client::getHost()
{
    return (_server.getHost());
}

const uint16_t        &Client::getPort()
{
    return (_server.getPort());
}

const std::string    &Client::getServerName()
{
    return (_server.getServerName());
}

std::string    Client::getReqServerName()
{
    return (request.getServerName());
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
    request.feed(data, size);
}

bool        Client::parsingCompleted()
{
    return (request.parsingCompleted());
}

short       Client::requestError()
{
    return (request.errorCode());
}

void        Client::clearRequest()
{
    request.clear();
}

bool        Client::keepAlive()
{
    return (request.keepAlive());
}

void        Client::buildResponse()
{
    _response.setRequest(request);
    _response.buildResponse();
}

std::string     Client::getResponse()
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
    request.printMessage();
}

void             Client::updateTime()
{
    _last_msg_time = time(NULL);
}

void             Client::clearClient()
{
    _response.clearResponse();
    request.clear();
    _total_bytes_read = 0;
    bytes_to_send = 0;
    total_bytes_sent = 0;
}
// size_t             Client::getPacketSize()
// {
//     if ( total_bytes_sent < _response.getLen())
//     {
//         if (_response.getLen() - total_bytes_sent > 8192)
//         {
//             total_bytes_sent += 8192;
//             return (8192);
//         }
//         else
//         {
         
//             return (_response.getLen() - total_bytes_sent > 8192);
//         }
//     }
// }

// bool      Client::isCgi()
// {
//     return (_response.isCgi());
// }