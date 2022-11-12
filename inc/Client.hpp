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
        Client(ServerConfig &);
        ~Client();

        int                 getSocket();
        struct sockaddr_in  getAddress();
        HttpRequest         &getRequest();
        time_t              getLastTime();

        void                setSocket(int &);
        void                setAddress(sockaddr_in &);

        void                feedData(char *, size_t);
        bool                parsingCompleted();
        short               requestError();
        bool                keepAlive();
        void                clearRequest();
        void                buildResponse();
        void                printReq();
        void                updateTime();
        void                handleCgi();

        char                *getResponse();
        size_t              getResponseLength();
        size_t              getTotalBytes();    
        void                clearResponse();
        int                 getResponseCode();


    private:
        int                 _client_socket;
        struct sockaddr_in  _client_address;
        HttpRequest         _request;
        Response            _response;
        ServerConfig        _server;
        size_t              _total_bytes_read;
        time_t              _last_msg_time;
};


#endif // CLIENT_HPP