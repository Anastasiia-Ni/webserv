#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Webserv.hpp"
# include "HttpRequest.hpp"


/* 
    Takes a string object that contain the whole request message and parse it into 3 Variables 
    _request_line, _request_headers, _response_body. And build the response message.
*/
class Response
{
    public:

        static     Mime _mime;
        Response();
        Response(HttpRequest&);
        ~Response();


        char  *getRes();
        size_t getLen() const;
        int         getCode() const;

        void        setRequest(HttpRequest &);
        void        setServer(ServerConfig &);
        
        void        buildResponse();
        void        clearResponse();
        void        handleCgi();




    private:
        HttpRequest     _request;
        ServerConfig    _server;
        std::string     _target_file;
        std::vector<uint8_t> _body;
        size_t          _body_length;
        std::string     _response_content;
        std::string     _location;
        short           _code;
        char            *_res;

        int     buildBody();
        size_t  file_size();
        void    setStatusLine();
        void    setHeaders();
        int     readFile();
        void    contentType();
        void    contentLength();
        void    connection();
        void    server();
        void    location();
        int     handleTarget();
        void    buildErrorBody();
        bool    reqError();


};

#endif // RESPONSE_HPP