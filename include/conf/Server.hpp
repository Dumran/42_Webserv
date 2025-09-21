#pragma once

#include <string>
#include <vector>
#include <map>
#include <netdb.h>

#include "Location.hpp"

class Server
{

private:
    unsigned long client_max_body_size;
    int port;
    std::string host;
    std::vector<std::string> server_names;
    std::map<int, std::string> error_pages;
    int root_location;
    std::string server_in_root;
    std::string cgi_extension_in_server;
    std::string cgi_path_in_server;
    std::vector<Location> locations;
    struct addrinfo first;
    struct sockaddr_in address;
    int server_fd;
    std::vector<Server> possible_servers;
    
    public:
    Server();
    ~Server();
    struct addrinfo *res;
    
    int addr_len;
    
    unsigned long getClientMaxBodySize() const;
    int getPort() const;
    const std::string &getHost() const;
    const std::vector<std::string> &getServerNames() const;
    std::map<int, std::string> &getErrorPages();
    int getRootLocation() const;
    const std::string &getServerInRoot() const;
    const std::string &getCgiExtensionInServer() const;
    const std::string &getCgiPathInServer() const;
    const std::vector<Location> &getLocations() const;
    struct addrinfo &getFirst();
    struct addrinfo *getRes();
    int getAddrLen();
    struct sockaddr_in &getAddress();
    int getServerFd() const;
    const std::vector<Server> &getPossibleServers() const;

    void setClientMaxBodySize(unsigned long size);
    void setPort(int port);
    void setHost(const std::string &host);
    void setServerNames(const std::vector<std::string> &names);
    void insertServerNames(const std::vector<std::string> &names);
    void addServerName(const std::string &name);
    void setErrorPages(const std::map<int, std::string> &pages);
    void insertErrorPages(const std::map<int, std::string> &pages);
    void addErrorPage(int code, const std::string& page);
    void setRootLocation(int location);
    void setServerInRoot(const std::string &root);
    void setCgiExtensionInServer(const std::string &extension);
    void setCgiPathInServer(const std::string &path);
    void setLocations(const std::vector<Location> &locations);
    void insertLocations(const std::vector<Location> &locations);
    void addLocation(const Location &location);
    void setFirst(const struct addrinfo &first);
    void setRes(struct addrinfo *res);
    void setAddrLen(int len);
    void setAddress(const struct sockaddr_in &addr);
    void setServerFd(int fd);
    void setPossibleServers(const std::vector<Server> &servers);
    void addPossibleServer(const Server &server);

};
