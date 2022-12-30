# Webserv

Develop a server in C++.

<img src="https://github.com/Anastasiia-Ni/webserv/blob/main/for_RM/HTTP_protocol.jpg" width=75% height=75%>

## About the project: 

An HTTP server is a software application that listens for incoming requests from clients and responds to them over the Hypertext Transfer Protocol (HTTP). HTTP servers are commonly used to serve web pages and other types of content to clients, such as web browsers.

When a client, such as a web browser, sends an HTTP request to a server, the server processes the request and sends an HTTP response back to the client. The response typically includes a status code indicating the result of the request, as well as any data requested by the client, such as a web page or image file.

HTTP servers can be used to serve a wide range of content, including static files (such as HTML, CSS, and JavaScript files) and dynamic web applications.

In the context of an HTTP server, a listening socket is used to listen for incoming HTTP requests from clients. When a client sends an HTTP request to the server, the server's listening socket accepts the connection and creates a new socket for the client to communicate with the server. The server then processes the request and sends an HTTP response back to the client via the newly created socket.

Listening sockets are commonly used in server-client communication to allow the server to accept incoming connections from multiple clients simultaneously. This allows the server to scale to handle a large number of concurrent connections efficiently.

In summary, an HTTP server uses a listening socket to listen for incoming HTTP requests from clients, and to create new sockets for communication with the clients. The HTTP server then processes the requests and sends responses back to the clients via the new sockets.

This project was code for MacOS and for Google Chrome browser.

The program is working with a configuration file default or pass by the argument.
You can setup the port, the host, the allowed methods and some other basic paramaters for a webserver.

The program capable of running an HTTP server, handling requests, and parsing configuration files. 

The server handles the following HTTP features: 
- the basics of HTTP protocol;
- support GET, POST and DELETE methods;
- HTTP response status codes;
- error pages for the appropriate status code (by default or pass in the configuration file);
- handle multiply CGI (extensions *.py and *.sh);
- cookies and  session management.

#### Configuration file

We were inspired by the "server" part of NGINX configuration file according to the subject.

The format of our configuration file is [here](https://github.com/Anastasiia-Ni/.../format_config_file.conf)

#### CGI (Common Gateway Interface) 
In computing, Common Gateway Interface (CGI) is an interface specification that enables web servers to execute an external program, typically to process user requests.
It written in a scripting language (CGI scripts), but they may include compiled programs.

To run CGI using fork().

A model showing how CGI works:

<img src="https://github.com/Anastasiia-Ni/webserv/blob/main/for_RM/CGI.jpg" width=55% height=55%>

## Index:
- [inc](https://github.com/Anastasiia-Ni/webserv/tree/main/inc) contains headers
- [src](https://github.com/Anastasiia-Ni/webserv/tree/main/src) contains the source code of the program
- [cgi-bin](https://github.com/Anastasiia-Ni/webserv/tree/main/cgi-bin) CGI scripts
- [fusion_web](https://github.com/Anastasiia-Ni/webserv/tree/main/fusion_web) Website 

## Installation:
```
$ git clone git@github.com:Anastasiia-Ni/
$ cd webserv
$ make
```

## Usage:
- `./webserv`    - take default basic configuration files

- `./webserv configs/name.conf`    - take path to a configuration file

<img src="https://github.com/Anastasiia-Ni/webserv/blob/main/for_RM/screen.png" width=65% height=65%>

## Resources:

Webserv on C 

- https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa

HTTP

- https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
- RFC 2616 https://www.rfc-editor.org/rfc/rfc2616.html#section-4
- https://developer.mozilla.org/en-US/docs/Web/HTTP
- RFC 9112 https://www.rfc-editor.org/rfc/rfc9112.html

Nginx
- https://nginx.org/en/
- https://github.com/nginx/nginx
- Interviews with Igor Sysoev

Sockaddr

- [https://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html](https://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html)
- [https://stackoverflow.com/questions/21099041/why-do-we-cast-sockaddr-in-to-sockaddr-when-calling-bind](https://stackoverflow.com/questions/21099041/why-do-we-cast-sockaddr-in-to-sockaddr-when-calling-bind)

CGI
- RFC 3875 https://datatracker.ietf.org/doc/html/rfc3875
- https://www.youtube.com/watch?v=_j5spdsJdV8&t=562s
- https://www.tutorialspoint.com/python/python_cgi_programming.htm
- https://www.tutorialspoint.com/cplusplus/cpp_web_programming.htm

Boundary

- https://www.youtube.com/watch?v=_j5spdsJdV8&t=562s

Cookies & Session management

- [RFC 6265](https://www.rfc-editor.org/rfc/rfc6265)
- [https://www.valentinog.com/blog/cookies/#what-are-cookies-in-web-development](https://www.valentinog.com/blog/cookies/#what-are-cookies-in-web-development)
- [https://pymotw.com/3/http.cookies/](https://pymotw.com/3/http.cookies/)
- [https://code.activestate.com/recipes/325484/](https://code.activestate.com/recipes/325484/)
- [https://stackoverflow.com/questions/33318927/python-cgi-persistence-with-session-keys](https://stackoverflow.com/questions/33318927/python-cgi-persistence-with-session-keys)
