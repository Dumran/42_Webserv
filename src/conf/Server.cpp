#include "conf/Server.hpp"

Server::Server() : client_max_body_size(1024 * 1024), port(0)
{
}

Server::~Server()
{
}

unsigned long Server::getClientMaxBodySize() const
{
    return client_max_body_size;
}

int Server::getPort() const
{
    return port;
}

const std::string &Server::getHost() const
{
    return host;
}

const std::vector<std::string> &Server::getServerNames() const
{
    return server_names;
}

std::map<int, std::string> &Server::getErrorPages()
{
    return error_pages;
}

int Server::getRootLocation() const
{
    return root_location;
}

const std::string &Server::getServerInRoot() const
{
    return server_in_root;
}

const std::string &Server::getCgiExtensionInServer() const
{
    return cgi_extension_in_server;
}

const std::string &Server::getCgiPathInServer() const
{
    return cgi_path_in_server;
}

const std::vector<Location> &Server::getLocations() const
{
    return locations;
}

struct addrinfo &Server::getFirst() 
{
    return first;
}

struct addrinfo *Server::getRes()
{
    return res;
}

int Server::getAddrLen()
{
    return addr_len;
}

struct sockaddr_in &Server::getAddress()
{
    return address;
}

int Server::getServerFd() const
{
    return server_fd;
}

const std::vector<Server> &Server::getPossibleServers() const
{
    return possible_servers;
}

void Server::setClientMaxBodySize(unsigned long size)
{
    client_max_body_size = size;
}

void Server::setPort(int port)
{
    this->port = port;
}

void Server::setHost(const std::string &host)
{
    this->host = host;
}

void Server::setServerNames(const std::vector<std::string> &names)
{
    server_names = names;
}

void Server::insertServerNames(const std::vector<std::string> &names)
{
    server_names.insert(server_names.end(), names.begin(), names.end());
}

void Server::addServerName(const std::string &name)
{
    server_names.push_back(name);
}

void Server::setErrorPages(const std::map<int, std::string> &pages)
{
    error_pages = pages;
}

void Server::insertErrorPages(const std::map<int, std::string> &pages)
{
    error_pages.insert(pages.begin(), pages.end());
}

void Server::addErrorPage(int code, const std::string& page)
{
    error_pages[code] = page;
}

void Server::setRootLocation(int location)
{
    root_location = location;
}

void Server::setServerInRoot(const std::string &root)
{
    server_in_root = root;
}

void Server::setCgiExtensionInServer(const std::string &extension)
{
    cgi_extension_in_server = extension;
}

void Server::setCgiPathInServer(const std::string &path)
{
    cgi_path_in_server = path;
}

void Server::setLocations(const std::vector<Location> &locations)
{
    this->locations = locations;
}

void Server::insertLocations(const std::vector<Location> &locations)
{
    this->locations.insert(this->locations.end(), locations.begin(), locations.end());
}

void Server::addLocation(const Location &location)
{
    this->locations.push_back(location);
}

void Server::setFirst(const struct addrinfo &first)
{
    this->first = first;
}

void Server::setRes(struct addrinfo *res)
{
    this->res = res;
}

void Server::setAddrLen(int len)
{
    addr_len = len;
}

void Server::setAddress(const struct sockaddr_in &addr)
{
    address = addr;
}

void Server::setServerFd(int fd)
{
    server_fd = fd;
}

void Server::setPossibleServers(const std::vector<Server> &servers)
{
    possible_servers = servers;
}

void Server::addPossibleServer(const Server &server)
{
    possible_servers.push_back(server);
}
