# include "../inc/Response.hpp"

Response::Response(){}

Response::~Response(){}

Response::Response(HttpRequest &req): _request(req), _code(0) {}

void   Response::errResponse(short error_code)
{
    _target_file = _server.getErrorPages().at(error_code);
    _code = error_code;
    
}

void   Response::contentType()
{
    // std::cout << "File is = " << _target_file << std::endl;
    if(_target_file.rfind(".html", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: text/html\r\n");
    else if(_target_file.rfind(".jpg", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: image/jpeg\r\n");
    else if(_target_file.rfind(".png", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: image/png\r\n");
    else if(_target_file.rfind(".ico", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: image/x-icon\r\n");
    else if(_target_file.rfind(".css", std::string::npos) != std::string::npos)
        _response_content.append("Content-Type: text/css\r\n");
    else
    {
        _response_content.append("Content-Type: text/plain\r\n");
    }
}

void   Response::contentLength()
{
    std::stringstream ss;
    ss << _body_length;
    _response_content.append("Content-Length: ");
    _response_content.append(ss.str());
    _response_content.append("\r\n");

}

void   Response::connection()
{
    if(_request.getHeader("Connection") == "keep-alive")
        _response_content.append("Connection: keep-alive\r\n");
}

void   Response::server()
{
        _response_content.append("Server: AMAnix\r\n");
}

void    Response::setHeaders()
{
    contentType();
    contentLength();
    connection();
    server();
    _response_content.append("\r\n");
}

bool fileExists (const std::string& f) {
    std::ifstream file(f.c_str());
    return (file.good());
}

static bool    isDirectory(std::string path)
{
    struct stat file_stat;

    stat(path.c_str(), &file_stat);

    return (S_ISDIR(file_stat.st_mode));
        
}
/* 
    Compares URI with locations from config file and tries to find the best match. 
    If match found, then location_key is set to that location, otherwise location_key will be an empty string.
*/
void     getLocationKey(std::string &path, std::vector<Location> locations, std::string &location_key)
{
    int biggest_match = 0;
    
    for(std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it)
    {
        if(path.find_first_of(it->getPath()) == 0)
        {
               if(path.length() == it->getPath().length() || path[it->getPath().length()] == '/')
               {
                    if (it->getPath().length() > biggest_match)
                    {
                        biggest_match = it->getPath().length();
                        location_key = it->getPath(); 
                        // std::cerr << "Loc = " << it->getPath() << " and root = " << it->getRootLocation() <<
                        //  " and index is = " << it->getIndexLocation() << std::endl;
                    }
               }
        }
    }
}
int    Response::handleTarget()
{
    // if (_request.getPath().find("..") != std::string::npos)
    // {
    //     _code = 403;
    //     return (1);
    // }
    std::cout << "URI is = " << _request.getPath() << std::endl;
    std::string location_key;
    getLocationKey(_request.getPath(), _server.getLocations(), location_key);
    std::cerr << "LOCATION KEY WINNER ISSSS = " << location_key << std::endl; 
    // If URI matches with a Location block
    if (location_key.length() > 0)
    {
        //decide here to use alias or root
        std::string root_path = _server.getLocationKey(location_key)->getRootLocation();

        if (root_path.back() == '/' && _request.getPath()[0] == '/')
            _request.getPath().erase(0, 1);
        _target_file = _server.getLocationKey(location_key)->getRootLocation() +
        _request.getPath();
        if (isDirectory(_target_file))
        {
            if (_target_file.back() != '/' && _server.getLocationKey(location_key)->getIndexLocation()[0] != '/')
                _target_file += '/';
            _target_file += _server.getLocationKey(location_key)->getIndexLocation();
        }
        std::cout << "TARGET FILE = " << _target_file << std::endl;
    }
    else
    {
        if (_request.getPath().compare("/") == 0 || isDirectory(_target_file))
            _target_file = _server.getRoot() + _server.getIndex();
        else
            _target_file = _server.getRoot() +_request.getPath().substr(1, _request.getPath().length() - 1);
    }
    // remove this later, and check while reading file.
    // if(!fileExists(_target_file))
    // {

    //     // std::cout << "err targer file = " << _target_file << std::endl;
    //     _code = 404;
    // }
    return (0);
}

bool    Response::reqError()
{
    if(_code = _request.errorCode())
        return (1);
    return (0);
}
void    Response::buildErrorBody()
{
        _target_file = _server.getErrorPages().at(_code);
        readFile();
}

void    Response::buildResponse()
{
/*  if(checkReqError() || buildBody())
        buildErrorBody()
    setStatusLine()
    setHeaders();
    
*/
    //checkConfig() // 
    //constructTarget() // If there is an error then target file will be error html file, either default or from config.
    //buildBody()
    //buildStatusLine
    //buildHeader
    std::cerr << "HERE" << std::endl;
    if(reqError() || buildBody())
        buildErrorBody();
    
    setStatusLine();
    setHeaders();
}


std::string Response::getContent() const { return _response_content; }
char*       Response::getBody() { return reinterpret_cast<char*> (&_body[0]); }
size_t      Response::getBodyLength() const { return _body_length; }

/* Check if there is any error and assign the correct status code to response message */
void        Response::setStatusLine()
{
    if(_code == 200)
        _response_content.append("HTTP/1.1 200 OK\r\n");
    else if(_code == 400)
        _response_content.append("HTTP/1.1 400 BadRequest\r\n");
    else if(_code == 403)
        _response_content.append("HTTP/1.1 403 Forbidden\r\n");   
    else
        _response_content.append("HTTP/1.1 404 Not Found\r\n");
    

}

// size_t Response::file_size() 
// {
//     FILE* fin = fopen(_target_file.c_str(), "rb");
//     if (fin == NULL) {
//         _code = 404;
//         std::cerr << " webserv: open error 1 " << strerror(errno) << std::endl;
//         std::cerr << "Target file = |" << _target_file << "|" << std::endl; 
//         return (-1);
//     }

//     fseek(fin, 0L, SEEK_END);
//     size_t size = ftell(fin);
//     fclose(fin);
//     return size;
// }

int    Response::buildBody()
{
    if (handleTarget())
        return (1);
    if(readFile())
        return (1);

    return (0);
}

int     Response::readFile()
{
    std::ifstream file(_target_file.c_str());
    
    if (file.fail())
    {
        _code = 404;
        return (1);
    }
    std::ostringstream ss;
	if(!(ss << file.rdbuf()))
    {
        _code = 404;
        return (1);
    }
    std::string temp_str = ss.str();
    _body.insert(_body.begin(), temp_str.begin(), temp_str.end());
    _body_length = _body.size();
    _code = 200;
    return (0);
}

int      Response::getErrorCode() const
{
    return (_code);
}

void     Response::setServer(ServerConfig &server)
{
    _server = server;
}

void    Response::setRequest(HttpRequest &req)
{
    _request = req;
}

void   Response::clearResponse()
{
    _body_length = 0;
    _response_content.clear();
    _body.clear();
    _code = 0;
}

int      Response::getCode() const
{
    return (_code);
}


void        Response::handleCgi()
{
    CgiHandler obj(_request.getPath());
    obj.initEnv(_request.getPath(), _request.getQuery());
    obj.execute();
}