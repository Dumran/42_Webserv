#include "Client.hpp"

Client::Client(int fd, Server &server, Config &config) : fd(fd),
                                                         form_data(""),
                                                         events(REQUEST),
                                                         server(server),
                                                         config(config),
                                                         write_buffer(""),
                                                         write_offset(0)
{
}

Client::~Client()
{
}

int Client::getFd() const {
    return fd;
}

std::string& Client::getFormData() {
    return form_data;
}

RequestType Client::getEvents() const {
    return events;
}

Response& Client::getResponse() {
    return response;
}

const Response& Client::getResponse() const {
    return response;
}

unsigned long Client::getMaxBodySize() const {
    return max_body_size;
}

Server& Client::getServer() {
    return server;
}

Config& Client::getConfig() {
    return config;
}

const std::string& Client::getWriteBuffer() const {
    return write_buffer;
}

size_t Client::getWriteOffset() const {
    return write_offset;
}

const std::string& Client::getServerName() const {
    return server_name;
}

void Client::setFd(int fd) {
    this->fd = fd;
}

void Client::setFormData(const std::string& formData) {
    this->form_data = formData;
}

void Client::setEvents(RequestType events) {
    this->events = events;
}

void Client::setResponse(const Response& response) {
    this->response = response;
}

void Client::setMaxBodySize(unsigned long maxBodySize) {
    this->max_body_size = maxBodySize;
}

void Client::setServer(const Server& server) {
    this->server = server;
}

void Client::setConfig(const Config& config) {
    this->config = config;
}

void Client::setWriteBuffer(const std::string& writeBuffer) {
    this->write_buffer = writeBuffer;
}

void Client::setWriteOffset(size_t writeOffset) {
    this->write_offset = writeOffset;
}

void Client::setServerName(const std::string& serverName) {
    this->server_name = serverName;
}

void Client::clear(void)
{
    this->response = Response();
    this->form_data = "";
    this->events = REQUEST;
    this->write_buffer.clear();
    this->write_offset = 0;
}
