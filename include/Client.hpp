#pragma once

#include "Response.hpp"
#include "conf/Config.hpp"

class Client
{
private:
    int fd;
    std::string form_data;
    RequestType events;
    Response response;
    unsigned long max_body_size;
    Server server;
    Config config;
    std::string write_buffer;
    size_t write_offset;
    std::string server_name;

public:
    Client(int fd, Server &server, Config &config);
    ~Client();

    int getFd() const;
    std::string& getFormData();
    RequestType getEvents() const;
    Response& getResponse();
    const Response& getResponse() const;
    unsigned long getMaxBodySize() const;
    Server& getServer();
    Config& getConfig();
    const std::string& getWriteBuffer() const;
    size_t getWriteOffset() const;
    const std::string& getServerName() const;

    void setFd(int fd);
    void setFormData(const std::string& formData);
    void setEvents(RequestType events);
    void setResponse(const Response& response);
    void setMaxBodySize(unsigned long maxBodySize);
    void setServer(const Server& server);
    void setConfig(const Config& config);
    void setWriteBuffer(const std::string& writeBuffer);
    void setWriteOffset(size_t writeOffset);
    void setServerName(const std::string& serverName);

    void clear(void);
};
