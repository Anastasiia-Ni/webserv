# include "../inc/Response.hpp"

Mime Response::_mime;

Response::Response(): _cgi_response_length(0), _code(0), _res(NULL), _target_file(""), _body_length(0),
                      _cgi(0), _auto_index(0){}

Response::~Response()
{
    // if(_res)
    //     delete [] _res;
}

Response::Response(HttpRequest &req): _cgi_response_length(0), _request(req), _code(0), _res(NULL)
                                    , _target_file(""), _body_length(0), _cgi(0),  _auto_index(0){}


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
    ss << _response_body.length();
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

void    Response::date()
{
    char date[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    _response_content.append("Date: ");
    _response_content.append(date);
    _response_content.append("\r\n");

}
// uri ecnoding
void    Response::setHeaders()
{
    contentType();
    contentLength();
    connection();
    server();
    location();
    date();

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
    if((method == GET && !methods[0]) || (method == POST && !methods[1]) ||
       (method == DELETE && !methods[2]))
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
    // std::cout << "Target_file after replacing alias is " << target_file << std::endl;
}

static void      appendRoot(Location &location, HttpRequest &request, std::string &target_file)
{
    target_file = combinePaths(location.getRootLocation(), request.getPath(), "");
    // std::cout << "Target_file after appending root is " << target_file << std::endl;
}

/* check a file for CGI (the extension is supported, the file exists and is executable) and run the CGI */
int        Response::handleCgi(std::string &location_key)
{
    std::string path;
    std::string exten;
    size_t      pos;
    
    path = this->_request.getPath();
    if (path[0] && path[0] == '/') // возможно еще отрезать весь кусок после расширения
        path.erase(0, 1);
    if (path == "cgi-bin")
        path += "/" + _server.getLocationKey(location_key)->getIndexLocation();
    else if (path == "cgi-bin/")
        path.append(_server.getLocationKey(location_key)->getIndexLocation());
    
    std::cout << "PATH: " << path << std::endl; // delete
    pos = path.find(".");
    if (pos == std::string::npos)
    {
        std::cout << "Extension is not supported" << std::endl;
        _code = 501; // 501 Not Implemented seems suitable error code for wrong extenisons
        return (1);
    }
    exten = path.substr(pos);
    if (exten != ".py" && exten != ".sh")
    {
        std::cout << "Extension " << exten << " is not supported" << std::endl;
        _code = 501; // 501 Not Implemented seems suitable error code for wrong extenisons
        return (1);
    }
    if (ConfigFile::getTypePath(path) != 1) 
    {
        std::cout << "CGI " << path << " is not exist" << std::endl;
        _code = 404;
        return (1);
    }
    if (ConfigFile::checkFile(path, 1) == -1 || ConfigFile::checkFile(path, 3) == -1)
    {
        std::cout << "CGI file is not executable or unreadable" << std::endl;
        _code = 403;
        return (1);
    }
    if (isAllowedMethod(_request.getMethod(), *_server.getLocationKey(location_key), _code))
        return (1); // проверить еще, после 1 сайт висит
    CgiHandler obj(path);
    _cgi = 1;
    if(pipe(_cgi_fd) < 0)
    {
        std::cout << "Pipe() fail" << std::endl;
        _code = 500;
        return (1);
    }
    obj.initEnv(_request, _server.getLocationKey(location_key)); // + URI
    obj.execute(_request, this->_cgi_fd[1], _response_content, this->_code);

    return (0);
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
               if( it->getPath() == "/" || path.length() == it->getPath().length() || path[it->getPath().length()] == '/')
               {
                std::cout << "LOCATION = " << it->getPath() << std::endl;
                    if(it->getPath().length() > biggest_match)
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
    getLocationMatch(_request.getPath(), _server.getLocations(), location_key);
    // If URI matches with a Location block
    // std::cout << "METHOD IS " <<  _request.getMethod() << " And matched location is |" << location_key << "| and code is " << _code << std::endl;
    if (location_key.length() > 0)
    {
        Location target_location = *_server.getLocationKey(location_key);

        if(isAllowedMethod(_request.getMethod(), target_location, _code))
            return (1);
        std::cout << "after allwoed\n" ;
        if (checkReturn(target_location, _code, _location))
            return (1);
		if(target_location.getPath().find("cgi-bin") != std::string::npos)
		{
            return(handleCgi(location_key)); // If CGI will handle the resoponse --> return 0, If Error found Return 1;
		}

        if(!target_location.getAlias().empty())
        {
            replaceAlias(target_location, _request, _target_file);
            _target_file = combinePaths(_server.getRoot(), _target_file, "");
        }
        else
            appendRoot(target_location, _request, _target_file);
        // std::cout << "Target file before checking dir is " << _target_file << std::endl;
        if (isDirectory(_target_file))
        {
            if (_target_file.back() != '/')
            {
                _code = 301;
                _location = _request.getPath() + "/";
                // std::cout << "LOCATINO IS = " << _location << std::endl;
                return (1);
            }
            if(!target_location.getIndexLocation().empty())
                _target_file += target_location.getIndexLocation();
            else
                _target_file += _server.getIndex();
            // std::cout << "target after adding index =  " << _target_file << std::endl;;
            if(!fileExists(_target_file))
            {
                if(target_location.getAutoindex())
                {
                    _target_file.erase(_target_file.find_last_of('/') + 1);
                    _auto_index = true;
                    return (0);
                }
                else
                {
                    _code = 403;
                    return (1);
                }
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
        // if(_request.getMethod() == POST || _request.getMethod() == DELETE)
        //     _target_file = combinePaths(_server.getRoot(), "upload" , _request.getPath());
        // else    
        _target_file = combinePaths(_server.getRoot(), _request.getPath(), "");
        if(isDirectory(_target_file))
        {
            if (_target_file.back() != '/')
            {
                _code = 301;
                _location = _request.getPath() + "/";
                // std::cout << "LOCATINO IS = " << _location << std::endl;
                return (1);
            }
            _target_file += _server.getIndex();
            if(!fileExists(_target_file))
            {
                /* Uncomment when autoindex is enabled outside location blocks
                if(_server.getAutoindex())
                {
                    _target_file.erase(_target_file.find_last_of('/') + 1);
                    _auto_index = true;
                    return (0);
                }
                */
                // std::cout << "FORBIDEN !!!!!!!!!!!!!!!\n";
                _code = 403;
                return (1);
            }
            if (isDirectory(_target_file))
            {
                _code = 301;
                _location = combinePaths(_request.getPath(), _server.getIndex(), "");
                if(_location.back() != '/')
                    _location.insert(_location.end(), '/');
                // std::cout << "Location =  " << _location << std::endl;;
                return (1);
            }
        }
    }
    return (0);
}

bool    Response::reqError()
{
    if(_request.errorCode())
    {
        _code = _request.errorCode();
        return (1);
    }        
    return (0);
}

void    Response::setServerDefaultErrorPages()
{
    _response_body = getErrorPage(_code);
    // _body.insert(_body.begin(), error_body.begin(), error_body.end());
    // _body_length = _body.size();
    // std::cout << "DEFAULLT STRING ERRORS \n";
}

void    Response::buildErrorBody()
{
        // if(_code == 301)
        //     return;
        // instead check here if error codes contains .css or just plain text. if it contains style then set _code to 302
        if( !_server.getErrorPages().count(_code) || _server.getErrorPages().at(_code).empty())
            setServerDefaultErrorPages();
        else
        {
            // std::cout << "NON_DEFAULT STRING ERRORS \n";

            if(_code >= 400 && _code < 500)
            {
                _location = _server.getErrorPages().at(_code);
                // std::cout << "Error Location is  " << _location << std::endl;
                _code = 302;
            }

            _target_file = _server.getRoot() +_server.getErrorPages().at(_code);
            // std::cout << "Non Default ErrorPath is " << _target_file << std::endl;
            short old_code = _code;
            if(readFile())
            {
                _code = old_code;
                _response_body = getErrorPage(_code);
                // _body.insert(_body.begin(), error_body.begin(), error_body.end());
                // _body_length = _body.size();
            }
        }

}
int    Response::constructCgiResp()
{
    // char buf[4096];
    // int  recvd = 0;
    // while(( recvd = read(_cgi_fd[0], buf, 4096)) > 0)
    // {
    //     _response_content.append(buf, recvd);
    //     memset(buf, 0, sizeof(buf));
    // }
    // close(_cgi_fd[0]);
    // // if(recvd == 0)
    // //     return 0;
    // if(recvd < 0)
    // {
    //     _code = 500;
    //     return 1;
    // }
    return (0);
        
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
	{
        if(!constructCgiResp())
            return;
        buildErrorBody();
    }
    else if(_auto_index)
    {
        if(buildHtmlIndex(_target_file, _body, _body_length))
        {
            _code = 500;
            buildErrorBody();
        }
        else
            _code = 200;
    }
    setStatusLine();
    setHeaders();
    _response_content.append(_response_body);
}

/* Returns the entire reponse ( Headers + Body )*/
std::string     Response::getRes(){

	// if(_cgi)
	// {
    // 	char *temp = new(std::nothrow) char[4001];
    //     if(!temp)
    //     {
    // 		std::cerr << "new Failed" << std::endl;
    //         return (NULL);
    //     }
	// 	_cgi_response_length = read(_cgi_fd[0], temp, 4001);
    //     close(_cgi_fd[0]);
	// 	return temp;
	// }
    // else
	// {
        return (_response_content);
	// }
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
    _response_content.append("HTTP/1.1 " + std::to_string(_code) + " ");
    _response_content.append(statusCodeString(_code));
    _response_content.append("\r\n");

}

int    Response::buildBody()
{
    if (handleTarget())
        return (1);
	if(_cgi || _auto_index)
		return (0);
    if(_code)
        return (0);
    // std::cout << _target_file << "ERROR " << std::endl;
    if(_request.getMethod() == GET)
    {
        if(readFile())
            return (1);
    }
    else if(_request.getMethod() == POST)
    {
        std::ofstream file(_target_file.c_str(), std::ios::binary);
        if (file.fail())
        {
            // std::cout << "FILE READ FAILED1, PATH is: " + _target_file << std::endl;
            _code = 404;
            return (1);
        }



        if (_request.getMultiformFlag())
        {
            std::string body = _request.getBody();
            body = removeBoundary(body, _request.getBoundary());
            file.write(body.c_str(), _request.getBody().length());
        }

        else
        {
            std::cout << "TARGET FILE IS :" << _target_file << " and file size is " << _request.getBody().length() << std::endl;
            file.write(_request.getBody().c_str(), _request.getBody().length());
        }



        // _target_file
    }
    else if (_request.getMethod() == DELETE)
    {
        if( remove( _target_file.c_str() ) != 0 )
        {
            perror( "Error deleting file" );
            _code = 500;
            return(1);
        }
        else
        {
            puts( "File successfully deleted" );
        }
        
    }
    _code = 200;
    return (0);
}

int     Response::readFile()
{
    std::ifstream file(_target_file.c_str());

    if (file.fail())
    {
        // std::cout << "FILE READ FAILED1, PATH is: " + _target_file << std::endl;
        _code = 404;
        return (1);
    }
    std::ostringstream ss;
	if(!(ss << file.rdbuf()))
    {
        // std::cout << "FILE READ FAILED2, PATH is: " + _target_file << std::endl;
        _code = 404;
        return (1);
    }
    // std::cout << "FILE READ Succeed, PATH is: " + _target_file << std::endl;

    _response_body = ss.str();
    // _body.insert(_body.begin(), temp_str.begin(), temp_str.end());
    // _body_length = _body.size();

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
void        Response::cutRes(size_t i)
{
    _response_content = _response_content.substr(i);
}
void   Response::clearResponse()
{
    _target_file.clear();
    _body.clear();
    _body_length = 0;
    _response_content.clear();
    _response_body.clear();
    _location.clear();
    _code = 0;
    // if(_res)
    // {
    //     delete [] _res;
    //     _res = NULL;
    // }
    _cgi = 0;
    _cgi_response_length = 0;
    _auto_index = 0;
}

int      Response::getCode() const
{
    return (_code);
}

bool    Response::isCgi()
{
    return (_cgi);
}

std::string Response::removeBoundary(std::string &body, std::string &boundary)
{
    std::string buffer;
    std::string new_body;
    std::string filename;
    // std::vector<std::string> for_filename;
    bool is_boundary = false;
    bool is_content = false;

    if (body.find("--" + boundary) != std::string::npos && body.find("--" + boundary + "--") != std::string::npos)
    {
        // std::cout << "BODY:" << body << std::endl;
        for (size_t i = 0; i < body.size(); i++)
        {
            buffer.clear();
            while(body[i] != '\n')
            {
                buffer += body[i];
                i++;
            }
            if (!buffer.compare(("--" + boundary + "--\r")))
            {
                is_content = true;
                is_boundary = false;
            }
            if (!buffer.compare(("--" + boundary + "\r")))
            {
                is_boundary = true;
            }
            if (is_boundary)
            {
                if (!buffer.compare(0, 31, "Content-Disposition: form-data;"))
                {
                    filename = "test";
                // _target_file написать имя файла 

                }
                else if(!buffer.compare(0, 1, "\r") && !filename.empty())
                {
                    is_boundary = false;
                    is_content = true;
                }
                
            }
            else if (is_content)
            {
                if (!buffer.compare(("--" + boundary + "\r")))
                {
                    is_boundary = true;
                }
                else if (!buffer.compare(("--" + boundary + "--\r")))
                {
                    // _target_file = "upload/"+ filename;
                    new_body.erase(new_body.end() - 1);
                    break;
                }
                else
                    new_body += (buffer + "\n");
            }

        }
    }
    std::cout << "NEW BODY:" << new_body << std::endl;
    return new_body;
    // else
    //    _error_code = 400; // download error
}