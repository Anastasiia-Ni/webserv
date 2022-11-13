# include "../inc/Response.hpp"

Mime Response::_mime;

Response::Response(): _code(0), _res(NULL), _target_file(""), _body_length(0) {}

Response::~Response()
{
    if(_res)
        delete [] _res;
}

Response::Response(HttpRequest &req): _request(req), _code(0), _res(NULL), _target_file(""), _body_length(0) {}


void   Response::contentType()
{
    std::cout << "File is = " << _target_file << std::endl;
    _response_content.append("Content-Type: ");
    if(_target_file.rfind(".", std::string::npos) != std::string::npos)
        _response_content.append(_mime.getMimeType(_target_file.substr(_target_file.rfind(".", std::string::npos))) );
    else
        _response_content.append(_mime.getMimeType("default"));
    _response_content.append("\r\n");

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

void    Response::location()
{
    if(_location.length())
        _response_content.append("Location: "+ _location +"\r\n");
}
void    Response::setHeaders()
{
    //date();
    contentType();
    contentLength();
    connection();
    server();
    location();
    _response_content.append("\r\n");
}

bool fileExists (const std::string& f) {
    std::ifstream file(f.c_str());
    return (file.good());
}

static bool    isDirectory(std::string path)
{
    struct stat file_stat;
    if(stat(path.c_str(), &file_stat) != 0)
        return (false); 

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
    
    // std::cout << "URI is = |" << _request.getPath()<< "|" << std::endl;
    std::string location_key;
    getLocationKey(_request.getPath(), _server.getLocations(), location_key);

    // If URI matches with a Location block
    if (location_key.length() > 0)
    {
        std::vector<short> methods = _server.getLocationKey(location_key)->getMethods();
        if(_request.getMethod() == GET && !methods[0] || _request.getMethod() == POST && !methods[1] || 
           _request.getMethod() == DELETE && !methods[3])
        {
            _code = 405;
            return (1);
        }
        if (_server.getLocationKey(location_key)->getReturn().length())
        {
            _code = 301;
            _location = _server.getLocationKey(location_key)->getReturn();
            return (1);
        }

        //decide here to use alias or root
        std::string root_path = _server.getLocationKey(location_key)->getRootLocation();

        if (root_path.back() == '/' && _request.getPath()[0] == '/')
            _request.getPath().erase(0, 1);
        _target_file = _server.getLocationKey(location_key)->getRootLocation() +
        _request.getPath();

        if (isDirectory(_target_file))
        {
            if (_target_file.back() != '/')
            {
                 _code = 301;
                _location = _request.getPath() + "/";
                return (1);
            }
            _target_file += _server.getLocationKey(location_key)->getIndexLocation();
            if(!fileExists(_target_file))
            {
                _code = 403;
                return (1);
            }
        }
        // std::cout << "TARGET FILE = " << _target_file << std::endl;
    }
    else
    {
        if (_request.getPath().compare("/") == 0 || isDirectory(_target_file))
        {
            _target_file = _server.getRoot() + _server.getIndex();
            if(!fileExists(_target_file))
            {
                _code = 403;
                return (1);
            }
        }
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
        // if(_code == 301)
        //     return;
        // instead check here if error codes contains .css or just plain text. if it contains style then set _code to 302
        if(_code >= 400 && _code < 500 && _code != 405) 
        {
            _location = _server.getErrorPages().at(_code);
            _code = 302;
        }
        _target_file = _server.getRoot() +_server.getErrorPages().at(_code);
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
    // std::cerr << "HERE" << std::endl;
    if(reqError() || buildBody())
        buildErrorBody();
    
    setStatusLine();
    setHeaders();
}

/* Returns the entire reponse ( Headers + Body)*/
char  *Response::getRes(){ 

    _res = new(std::nothrow) char[_response_content.length() + _body_length];
    if(!_res)
    {
        std::cerr << "new Failed" << std::endl;
        exit(1);
    }   
    memcpy(_res, _response_content.data(), _response_content.length());
    memcpy(_res + _response_content.length(), &_body[0], _body_length);
    return _res;
}

/* Returns the length of entire reponse ( Headers + Body) */
size_t Response::getLen() const { return (_response_content.length() + _body_length); }

/* Constructs Status line based on status code. */
void        Response::setStatusLine()
{
    _response_content.append("HTTP/1.1 " + std::to_string(_code));
    _response_content.append(statusCodeString(_code));
    _response_content.append("\r\n");

}

int    Response::buildBody()
{
    if (handleTarget())
        return (1);
    if(readFile())
        return (1);
    _code = 200;
    return (0);
}

int     Response::readFile()
{
    std::ifstream file(_target_file.c_str());
    
    if (file.fail())
    {
        std::cout << "FILE READ FAILED, PATH is: " + _target_file << std::endl;
        _code = 404;
        return (1);
    }
    std::ostringstream ss;
	if(!(ss << file.rdbuf()))
    {
        std::cout << "FILE READ FAILED, PATH is: " + _target_file << std::endl;
        _code = 404;
        return (1);
    }
    std::string temp_str = ss.str();
    _body.insert(_body.begin(), temp_str.begin(), temp_str.end());
    _body_length = _body.size();
    // _code = 200;
    return (0);
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
    _location = "";
    if(_res)
    {
        delete [] _res;
        _res = NULL;
    }
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
