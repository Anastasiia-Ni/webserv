# include "../inc/Response.hpp"

Mime Response::_mime;

Response::Response(): _cgi_response_length(0), _code(0), _res(NULL), _target_file(""), _body_length(0),  _cgi(0) {}

Response::~Response()
{
    if(_res)
        delete [] _res;
}

Response::Response(HttpRequest &req): _cgi_response_length(0), _request(req), _code(0), _res(NULL), _target_file(""), _body_length(0), _cgi(0){}


void   Response::contentType()
{
    // std::cout << "File is = " << _target_file << std::endl;
    _response_content.append("Content-Type: ");
    if(_target_file.rfind(".", std::string::npos) != std::string::npos && _code == 200)
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

static bool fileExists (const std::string& f) {
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

static bool    isAllowedMethod(HttpMethod &method, Location &location, short &code)
{
    std::vector<short> methods = location.getMethods();
    if(method == GET && !methods[0] || method == POST && !methods[1] ||
       method == DELETE && !methods[3])
    {
        code = 405;
        return (1);
    }
    return (0);
}

static bool    checkReturn(Location &loc, short &code, std::string &location)
{
    if(!loc.getReturn().empty())
    {
        code = 301;
        location = loc.getReturn();
        return (1);
    }
    return (0);
}

static std::string combinePaths(std::string p1, std::string p2, std::string p3)
{
    std::string res;
    if(p1.back() == '/' && (!p2.empty() && p2[0] == '/') )
        p2.erase(0, 1);
    if(p1.back() != '/' && (!p2.empty() && p2[0] != '/'))
        p1.insert(p1.end(), '/');
    
    if(p2.back() == '/' && (!p3.empty() && p3[0] == '/') )
        p3.erase(0, 1);
    
    if(p2.back() != '/' && (!p3.empty() && p3[0] != '/'))
        p2.insert(p1.end(), '/');
    res = p1 + p2 + p3;

    return(res);
}

static void      replaceAlias(Location &location, HttpRequest &request, std::string &target_file)
{
    target_file = combinePaths(location.getAlias(), request.getPath().substr(location.getPath().length()), "");
    std::cout << "Target_file after replacing alias is " << target_file << std::endl;
}

static void      appendRoot(Location &location, HttpRequest &request, std::string &target_file)
{
    target_file = combinePaths(location.getRootLocation(), request.getPath(), "");
    std::cout << "Target_file after appending root is " << target_file << std::endl;
}

void        Response::handleCgi()
{
    std::cout << "CGI FOUND \n";
    // this->_cgi_obj.setPath();
    CgiHandler obj("cgi-bin/get_hello.py"); //
    _cgi = 1;
    if(pipe(_cgi_fd) < 0)
        std::cout << "Pipe() fail" << std::endl;
    obj.initEnv(_request); // + URI
    obj.execute(_request, this->_cgi_fd[1]);
}

/*
    Compares URI with locations from config file and tries to find the best match.
    If match found, then location_key is set to that location, otherwise location_key will be an empty string.
*/

static void    getLocationMatch(std::string &path, std::vector<Location> locations, std::string &location_key)
{
    int biggest_match = 0;

    for(std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it)
    {
        if(path.find(it->getPath()) == 0)
        {
            std::cout << "URI PATH IS = " << path << " and Location part = " << it->getPath() << std::endl;
               if(path.length() == it->getPath().length() || path[it->getPath().length()] == '/')
               {
                    if(it->getPath().length() > biggest_match)
                    {
                        biggest_match = it->getPath().length();
                        location_key = it->getPath();
                        std::cerr << "Loc = " << it->getPath() << " and root = " << it->getRootLocation() <<
                         " and index is = " << it->getIndexLocation() << std::endl;
                    }
               }
        }
    }
}

int    Response::handleTarget()
{
    std::cout << "URI is = |" << _request.getPath()<< "|" << std::endl;
    std::string location_key;
    getLocationMatch(_request.getPath(), _server.getLocations(), location_key);

    // If URI matches with a Location block
    if (location_key.length() > 0)
    {   
        Location target_location = *_server.getLocationKey(location_key);

        if(isAllowedMethod(_request.getMethod(), target_location, _code))
            return (1);
        if (checkReturn(target_location, _code, _location))
            return (1);
		if(target_location.getPath().find("cgi-bin") != std::string::npos)
		{
            handleCgi();
			return 0;
		}

        if(!target_location.getAlias().empty())
        {
            replaceAlias(target_location, _request, _target_file);
            _target_file = combinePaths(_server.getRoot(), _target_file, "");
        }   
        else
            appendRoot(target_location, _request, _target_file);
        std::cout << "Target file before checking dir is " << _target_file << std::endl;
        if (isDirectory(_target_file))
        {
            if (_target_file.back() != '/')
            {
                _code = 301;
                _location = _request.getPath() + "/";
                std::cout << "LOCATINO IS = " << _location << std::endl;
                return (1);
            }
            if(!target_location.getIndexLocation().empty())
                _target_file += target_location.getIndexLocation();
            else
                _target_file += _server.getIndex();
            std::cout << "target after adding index =  " << _target_file << std::endl;;
            if(!fileExists(_target_file))
            {
                /*
                if(autoindex == on)
                    list files in dirctory :D; 
                 */
                _code = 403;
                return (1);
            }
            if (isDirectory(_target_file))
            {
                _code = 301;
                if(!target_location.getIndexLocation().empty())
                    _location = combinePaths(_request.getPath(), target_location.getIndexLocation(), "");
                else
                    _location = combinePaths(_request.getPath(), _server.getIndex(), "");
                if(_location.back() != '/')
                    _location.insert(_location.end(), '/');
                
                return (1);
            }
        }
        // std::cout << "TARGET FILE = " << _target_file << std::endl;
    }
    else
    {
        _target_file = combinePaths(_server.getRoot(), _request.getPath(), "");
        if(isDirectory(_target_file))
        {

            if (_target_file.back() != '/')
            {
                _code = 301;
                _location = _request.getPath() + "/";
                std::cout << "LOCATINO IS = " << _location << std::endl;
                return (1);
            }
            _target_file += _server.getIndex();
            if(!fileExists(_target_file))
            {
                /* 
                if(autoindex == on)
                    list files in direcort :D
                */
                std::cout << "FORBIDEN !!!!!!!!!!!!!!!\n";
                _code = 403;
                return (1);
            }
            if (isDirectory(_target_file))
            {
                _code = 301;
                _location = combinePaths(_request.getPath(), _server.getIndex(), "");
                if(_location.back() != '/')
                    _location.insert(_location.end(), '/');
                std::cout << "Location =  " << _location << std::endl;;
                return (1);
            }
        }

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
    if(_request.errorCode())
        return (1);
    return (0);
}

void    Response::setServerDefaultErrorPages()
{
    std::string error_body = getErrorPage(_code);
    _body.insert(_body.begin(), error_body.begin(), error_body.end());
    _body_length = _body.size();
    std::cout << "DEFAULLT STRING ERRORS \n";
}

void    Response::buildErrorBody()
{
        // if(_code == 301)
        //     return;
        // instead check here if error codes contains .css or just plain text. if it contains style then set _code to 302
        if(_server.getErrorPages().at(_code).empty())
            setServerDefaultErrorPages();
        else 
        {
            std::cout << "NON_DEFAULT STRING ERRORS \n";

            if(_code >= 400 && _code < 500)
            {
                _location = _server.getErrorPages().at(_code);
                std::cout << "Error Location is  " << _location << std::endl;
                _code = 302;
            }

            _target_file = _server.getRoot() +_server.getErrorPages().at(_code);
            std::cout << "Non Default ErrorPath is " << _target_file << std::endl;
            short old_code = _code;
            if(readFile())
            {
                _code = old_code;
                std::string error_body = getErrorPage(_code);
                _body.insert(_body.begin(), error_body.begin(), error_body.end());
                _body_length = _body.size();
            }
        }

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
    // std::cout << "FINISHED 3 function \n ---------------------- " << std::endl;
	if(_cgi)
		return;
    setStatusLine();
    setHeaders();
}

/* Returns the entire reponse ( Headers + Body)*/
char  *Response::getRes(){

	// if(_cgi)
	// 	return(_cgi_obj.getResponse());
	if(_cgi)
	{
    	char *temp = new(std::nothrow) char[4001];
		_cgi_response_length = read(_cgi_fd[0], temp, 4001);
        close(_cgi_fd[0]);
		return temp;
	}
    else
	{
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
	return NULL;
}

/* Returns the length of entire reponse ( Headers + Body) */
size_t Response::getLen() const {

	if(_cgi)
		return _cgi_response_length;
	return (_response_content.length() + _body_length);

	}

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
	if(_cgi == 1)
		return (0);
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
        std::cout << "FILE READ FAILED1, PATH is: " + _target_file << std::endl;
        _code = 404;
        return (1);
    }
    std::ostringstream ss;
	if(!(ss << file.rdbuf()))
    {
        std::cout << "FILE READ FAILED2, PATH is: " + _target_file << std::endl;
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
	_cgi_response_length = 0;
    _response_content.clear();
    _body.clear();
    _code = 0;
    _location = "";
    if(_res)
    {
        delete [] _res;
        _res = NULL;
    }
	_cgi = 0;
}

int      Response::getCode() const
{
    return (_code);
}

void        Response::handleCgi(HttpRequest& req)
{
    // CgiHandler obj(_request.getPath());
    // // CgiHandler obj("/Users/anifanto/Desktop/ft-server/cgi-bin/env.py");
    // obj.initEnv(req);
    // obj.execute();
}
