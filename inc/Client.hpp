#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Webserv.hpp"
#include "HttpRequest.hpp"
#include "Response.hpp"

/**
     Client
   - Stores all information related to the client such as socket and address
     along with request and response objects. each Client also have an object of the server it's conntected to.
**/
class Client
{
    public:
        Client();
        Client(const Client &other);
        Client(ServerConfig &);
		    Client &operator=(const Client & rhs);
        ~Client();

        int                 getSocket();
        struct sockaddr_in  getAddress();
        HttpRequest         &getRequest();
        time_t              getLastTime();
        const std::string &getServerName();
        const uint16_t &getPort();
        const in_addr_t &getHost();
        std::string getReqServerName();

        void                setSocket(int &);
        void                setAddress(sockaddr_in &);
        void                setServer(ServerConfig &);
        void                feedData(char *, size_t);
        bool                parsingCompleted();
        short               requestError();
        bool                keepAlive();
        void                clearRequest();
        void                buildResponse();
        void                printReq();
        void                updateTime();
        size_t              getPacketSize();

        std::string         getResponse();
        size_t              getResponseLength();
        size_t              getTotalBytes();
        void                clearResponse();
        int                 getResponseCode();
        void                clearClient();
        bool                isCgi();
        Response            _response;
    private:
        int                 _client_socket;
        struct sockaddr_in  _client_address;
        HttpRequest         _request;
        ServerConfig        _server;
        size_t              _total_bytes_read;
        size_t              bytes_to_send;
        size_t              total_bytes_sent;
        time_t              _last_msg_time;
};


#endif // CLIENT_HPP