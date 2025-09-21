#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include "WebServer.hpp"
#include "utils/StringUtils.hpp"
#include "utils/Utils.hpp"
#include "log/Log.h"
#include <string.h>
#include <climits>

WebServer::WebServer(std::vector<Config> &configs) : configs(configs)
{
    serversCreator(configs);
}

WebServer::~WebServer()
{
    for (size_t x = 0; x < poll_fds.size(); x++)
    {
        close(poll_fds[x].fd);
    }
}

int WebServer::socketCreator(Server &server)
{

    server.setAddrLen(sizeof(server.getAddress()));
    server.getAddress().sin_family = AF_INET;
    server.getAddress().sin_port = htons(server.getPort());

    memset(&server.getFirst(), 0, sizeof(server.getFirst()));
    memset(&server.getFirst(), 0, sizeof(server.getFirst()));

    server.getFirst().ai_family = AF_INET;
    server.getFirst().ai_socktype = SOCK_STREAM;
    server.getFirst().ai_flags = AI_PASSIVE;

    int status = getaddrinfo(server.getHost().c_str(), StringUtils::from(server.getPort()).c_str(), &server.getFirst(), &server.res);
    if (status != 0)
    {
        freeaddrinfo(server.res);
        throw ServerExcp("getaddrinfo Error");
    }
    return (socket(server.res->ai_family, server.res->ai_socktype, server.res->ai_protocol));
}

// void WebServer::cgiHandle(Client &client)
// {
//     extern char **environ;
//     int fd_out[2];
//     int fd_in[2];
//     if (pipe(fd_out) == -1 || pipe(fd_in) == -1)
//         return;
//     int pid = fork();
//     if (pid < 0)
//     {
//         close(fd_out[0]);
//         close(fd_out[1]);
//         close(fd_in[0]);
//         close(fd_in[1]);
//         return;
//     }
//     int req = static_cast<int>(client.getResponse().getRequestType());
//     if (pid == 0)
//     {
//         const std::string &cgi_root = client.getResponse().getCgiRoot();

//         chdir(cgi_root.c_str());
//         if (req == HttpMethod::POST || req == HttpMethod::DELETE)
//         {
//             setenv("CONTENT_TYPE", client.getResponse().getContentType().c_str(), 1);
//             setenv("CONTENT_LENGTH", StringUtils::from(client.getResponse().getContentLength()).c_str(), 1);
//         }
//         setenv("REQUEST_METHOD", HttpMethod::names[INT_MAX - req].c_str(), 1);
//         close(fd_out[0]);
//         dup2(fd_out[1], 1);
//         close(fd_out[1]);

//         close(fd_in[1]);
//         dup2(fd_in[0], 0);
//         close(fd_in[0]);

//         std::string file = client.getResponse().getFile().substr(cgi_root.length() + 1);
//         std::string cgiPath = client.getResponse().getCgiPath();
//         const char *argv[] = {cgiPath.c_str(), "-Wignore", file.c_str(), NULL};
//         execve(argv[0], const_cast<char *const *>(argv), environ);
//         exit(1);
//     }

//     close(fd_out[1]);
//     close(fd_in[0]);

//     bool errorHandle = false;
//     if (req == HttpMethod::POST || req == HttpMethod::DELETE)
//     {
//         const std::string &data = client.getResponse().getFormData();
//         if (data.empty())
//         {
//             client.getResponse().setResponseCode(HttpStatus::BAD_REQUEST);
//             errorHandle = true;
//         }
//         else if (write(fd_in[1], data.c_str(), data.size()) < 0)
//         {
//             client.getResponse().setResponseCode(HttpStatus::INTERNAL_SERVER_ERROR);
//             errorHandle = true;
//         }
//     }

//     close(fd_in[1]);

//     if (!errorHandle)
//     {
//         if (!Utils::wait(pid, 2))
//         {
//             client.getResponse().setResponseCode(HttpStatus::REQUEST_TIMEOUT);
//             errorHandle = true;
//         }
//     }
//     std::string output;
//     if (!errorHandle)
//     {
//         char buffer[10240];
//         int n;
//         while ((n = read(fd_out[0], buffer, sizeof(buffer))) > 0)
//             output.append(buffer, n);
//         if (n < 0)
//         {
//             client.getResponse().setResponseCode(HttpStatus::INTERNAL_SERVER_ERROR);
//             errorHandle = true;
//         }
//     }

//     close(fd_out[0]);

//     if (!errorHandle)
//     {
//         client.getResponse().setContent(output);
//     }
//     else
//     {
//         client.getResponse().setContent(Utils::resToErrorPages(client.getResponse(), client.getResponse().getResponseCode(), client));
//     }
//     client.setWriteBuffer(Utils::getResponseHeader(client));
// }

// ---- Yardımcılar ----

static bool make_pipes(int fd_out[2], int fd_in[2]) {
    if (::pipe(fd_out) == -1) return false;
    if (::pipe(fd_in)  == -1) {
        ::close(fd_out[0]); ::close(fd_out[1]);
        return false;
    }
    return true;
}

static void close_pair(int fds[2]) {
    ::close(fds[0]); ::close(fds[1]);
}


static void setup_child_stdio(int fd_out[2], int fd_in[2]) {
    ::close(fd_out[0]);
    ::dup2(fd_out[1], 1);
    ::close(fd_out[1]);

    ::close(fd_in[1]);
    ::dup2(fd_in[0], 0);
    ::close(fd_in[0]);
}

static void apply_cgi_env(const Client &client, int req) {
    // Not: Orijinal akıştaki env ayarları birebir korunur.
    if (req == HttpMethod::POST || req == HttpMethod::DELETE) {
        ::setenv("CONTENT_TYPE",  client.getResponse().getContentType().c_str(), 1);
        ::setenv("CONTENT_LENGTH", StringUtils::from(client.getResponse().getContentLength()).c_str(), 1);
    }
    ::setenv("REQUEST_METHOD",
             HttpMethod::names[INT_MAX - req].c_str(), 1);
}

static void exec_cgi_child(const Client &client, int req, int fd_out[2], int fd_in[2]) {
    extern char **environ;

    const std::string &cgi_root = client.getResponse().getCgiRoot();
    ::chdir(cgi_root.c_str());
    apply_cgi_env(client, req);
    setup_child_stdio(fd_out, fd_in);

    std::string file = client.getResponse().getFile().substr(cgi_root.length() + 1);
    std::string cgiPath = client.getResponse().getCgiPath();

    const char *argv[] = { cgiPath.c_str(), "-Wignore", file.c_str(), NULL };
    ::execve(argv[0], const_cast<char *const *>(argv), environ);
    ::exit(1);
}

static void handle_body_write_if_needed(Client &client, int req, int write_fd, bool &errorHandle) {
    if (req == HttpMethod::POST || req == HttpMethod::DELETE) {
        const std::string &data = client.getResponse().getFormData();
        if (data.empty()) {
            client.getResponse().setResponseCode(HttpStatus::BAD_REQUEST);
            errorHandle = true;
        } else if (::write(write_fd, data.c_str(), data.size()) < 0) {
            client.getResponse().setResponseCode(HttpStatus::INTERNAL_SERVER_ERROR);
            errorHandle = true;
        }
    }
}

static void wait_and_collect(pid_t pid, int out_fd, Client &client, bool &errorHandle, std::string &output) {
    if (!errorHandle) {
        if (!Utils::wait(pid, 2)) {
            client.getResponse().setResponseCode(HttpStatus::REQUEST_TIMEOUT);
            errorHandle = true;
        }
    }

    if (!errorHandle) {
        char buffer[10240];
        int n = 0;
        while ((n = ::read(out_fd, buffer, sizeof(buffer))) > 0) {
            output.append(buffer, n);
        }
        if (n < 0) {
            client.getResponse().setResponseCode(HttpStatus::INTERNAL_SERVER_ERROR);
            errorHandle = true;
        }
    }
}

static void finalize_cgi_response(Client &client, const std::string &output, bool errorHandle) {
    if (!errorHandle) {
        client.getResponse().setContent(output);
    } else {
        client.getResponse().setContent(
            Utils::resToErrorPages(client.getResponse(),
                                   client.getResponse().getResponseCode(),
                                   client)
        );
    }
    client.setWriteBuffer(Utils::getResponseHeader(client));
}

// ---- Asıl fonksiyon ----

void WebServer::cgiHandle(Client &client)
{
    int fd_out[2];
    int fd_in[2];

    if (!make_pipes(fd_out, fd_in))
        return;

    int pid = ::fork();
    if (pid < 0) {
        close_pair(fd_out);
        close_pair(fd_in);
        return;
    }

    int req = static_cast<int>(client.getResponse().getRequestType());

    if (pid == 0) {
        exec_cgi_child(client, req, fd_out, fd_in);
        // ulaşılmaz
    }

    // Ebeveyn
    ::close(fd_out[1]);
    ::close(fd_in[0]);

    bool errorHandle = false;

    handle_body_write_if_needed(client, req, fd_in[1], errorHandle);
    ::close(fd_in[1]);

    std::string output;
    wait_and_collect(pid, fd_out[0], client, errorHandle, output);
    ::close(fd_out[0]);

    finalize_cgi_response(client, output, errorHandle);
}


void WebServer::serversCreator(std::vector<Config> &configs)
{
    Log::info("Creating servers.");
    for (size_t i = 0; i < configs.size(); i++)
    {
        Server &server = configs[i].getDefault();
        server.setServerFd(socketCreator(configs[i].getDefault()));
        if (server.getServerFd() == 0)
        {
            freeaddrinfo(server.res);
            close(server.getServerFd());
            throw ServerExcp("Socket Error");
        }
        int opt = 1;
        if (setsockopt(server.getServerFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            close(server.getServerFd());
            freeaddrinfo(server.res);
            throw ServerExcp("Setsockopt Error");
        }
        if (bind(server.getServerFd(), server.res->ai_addr, server.res->ai_addrlen) < 0)
        {
            freeaddrinfo(server.res);
            close(server.getServerFd());
            throw ServerExcp("Bind Error");
        }
        freeaddrinfo(server.res);
        if (listen(server.getServerFd(), SOMAXCONN) < 0)
        {
            close(server.getServerFd());
            throw ServerExcp("Listen Error");
        }
        setNonBlocking(server.getServerFd());
        addClient(server.getServerFd(), POLLIN, i);
    }
    this->server_size = poll_fds.size();
}

void WebServer::setNonBlocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_RDWR | O_APPEND | O_NONBLOCK) == -1)
        throw ServerExcp("Fcntl Error");
}

bool WebServer::checkResponse(Client &client, std::string &headers)
{
    if (headers.find("Expect: 100-continue") != std::string::npos)
    {
        std::string continueResponse = "HTTP/1.1 100 Continue\r\n\r\n";
        if (send(client.getFd(), continueResponse.c_str(), continueResponse.size(), 0) <= 0)
        {
            client.setEvents(EXITED);
            return true;
        }
    }
    Utils::parseRsp(headers, client);
    if (client.getResponse().getIsCgi())
    {
        if (client.getEvents() == WAIT_FORM)
            return true;
        else
            return cgiHandle(client), true;
    }
    return false;
}

void WebServer::serverResponse(Client &client)
{
    std::string headers;
    char buffer[10240] = {0};
    int bytesRead;

    if (client.getEvents() == REQUEST)
    {
        while (true)
        {
            bytesRead = recv(client.getFd(), buffer, sizeof(buffer) - 1, 0);
            if (bytesRead > 0)
            {
                headers.append(buffer, bytesRead);
                if (headers.find("\r\n\r\n") != std::string::npos)
                    break;
            }
            else
            {
                if (bytesRead == -1)
                    return;
                break;
            }
        }
        client.setServerName(Utils::getHostHeader(headers));
        Utils::getServerByHost(client.getServerName(), client);
    }
    if (checkResponse(client, headers))
        return;
    client.setWriteBuffer(Utils::getResponseHeader(client));
}

void WebServer::addClient(int fd, short events, size_t i)
{
    pollfd clientPollFd = {fd, events, 0};
    Client newClient(fd, configs[i].getDefault(), configs[i]);
    poll_fds.push_back(clientPollFd);
    clients.push_back(newClient);
}

void WebServer::closeClient(int index)
{
    int fd = poll_fds[index].fd;
    if (fd < 0)
        return;
    close(fd);
    poll_fds[index].fd = -1;
    clients.erase(clients.begin() + index);
    poll_fds.erase(poll_fds.begin() + index);
}

void WebServer::readFormData(int i)
{
    bool tempChunk = clients[i].getResponse().getIsChunked();
    while (true)
    {
        char buffer[10240];
        int bytesRead = recv(clients[i].getFd(), buffer, sizeof(buffer), 0);
        if (bytesRead > 0)
        {
            clients[i].getFormData().append(buffer, bytesRead);
            if (tempChunk && clients[i].getFormData().find("0\r\n\r\n") != std::string::npos)
                Utils::parseChunked(clients[i], clients[i].getFormData());
        }
        else
            break;
    }
    if (clients[i].getResponse().getResponseCode() == HttpStatus::PAYLOAD_TOO_LARGE)
    {
        clients[i].setWriteBuffer(Utils::getResponseHeader(clients[i]));
        clients[i].setEvents(REQUEST);
        poll_fds[i].events |= POLLOUT;
    }
    else if (clients[i].getResponse().getContentLength() == clients[i].getFormData().size() || tempChunk != clients[i].getResponse().getIsChunked())
    {
        clients[i].getResponse().setFormData(clients[i].getFormData());
        clients[i].setEvents(REQUEST);
        if (clients[i].getResponse().getIsCgi())
            cgiHandle(clients[i]);
    }
}

int WebServer::newConnection(size_t i)
{
    Server &server = configs[i].getDefault();
    int clientFd = accept(server.getServerFd(), (sockaddr *)&server.getAddress(), (socklen_t *)&server.addr_len);
    if (clientFd < 0)
    {
        return clientFd;
    }

    setNonBlocking(clientFd);
    addClient(clientFd, POLLIN, i);
    return 1;
}

void WebServer::clientSend(int i)
{
    Client &c = clients[i];
    if (c.getWriteOffset() >= c.getWriteBuffer().size())
    {
        poll_fds[i].events &= ~POLLOUT;
        return;
    }

    size_t toSend = c.getWriteBuffer().size() - c.getWriteOffset();
    ssize_t n = send(c.getFd(),
                     c.getWriteBuffer().data() + c.getWriteOffset(),
                     toSend,
                     0);
    if (n > 0)
    {
        c.setWriteOffset(c.getWriteOffset() + n);
        if (c.getWriteOffset() >= c.getWriteBuffer().size())
        {
            Log::response(c);
            c.clear();
            poll_fds[i].events &= ~POLLOUT;
        }
    }
    else if (n == 0)
    {
        closeClient(i);
    }
    else
    {
        return;
    }
}

// void WebServer::start()
// {
//     Log::info("Server is running.");
//     while (true)
//     {
//         int events = poll(poll_fds.data(), poll_fds.size(), -1);
//         if (events < 0)
//             throw ServerExcp("Poll Error");

//         for (int i = poll_fds.size() - 1; i >= 0; i--)
//         {
//             short re = poll_fds[i].revents;
//             Client &client = clients[i];

//             if (re & (POLLHUP | POLLERR))
//             {
//                 closeClient(i);
//                 continue;
//             }

//             if (i < server_size && (re & POLLIN))
//             {
//                 newConnection(i);
//                 continue;
//             }

//             if (i > 0 && (re & POLLIN))
//             {
//                 if (client.getEvents() == WAIT_FORM)
//                 {
//                     readFormData(i);
//                     if (client.getResponse().getResponseCode() == HttpStatus::PAYLOAD_TOO_LARGE)
//                         continue;
//                 }
//                 else
//                 {
//                     serverResponse(client);
//                     if (client.getEvents() == EXITED)
//                         continue;
//                 }
//                 if (!client.getWriteBuffer().empty() && client.getEvents() != WAIT_FORM)
//                     poll_fds[i].events |= POLLOUT;
//             }

//             if (i > 0 && (re & POLLOUT) && client.getEvents() != WAIT_FORM && !client.getWriteBuffer().empty())
//                 clientSend(i);
//         }
//     }
// }

// --- WebServer yardımcıları ---

int WebServer::wait_for_events()
{
    if (poll_fds.empty()) return 0;
    int n = ::poll(&poll_fds[0], (int)poll_fds.size(), -1);
    if (n < 0) throw ServerExcp("Poll Error");
    return n;
}

bool WebServer::is_server_index(int i) const { return i < (int)server_size; }
bool WebServer::is_client_index(int i) const { return i > 0; }

static bool has_error(short re) { return (re & (POLLHUP | POLLERR)) != 0; }
static bool can_read(short re)  { return (re & POLLIN)  != 0; }
static bool can_write(short re) { return (re & POLLOUT) != 0; }

void WebServer::handle_error_or_hup(int i)
{
    closeClient(i);
}

void WebServer::handle_server_readable(int i)
{
    newConnection(i);
}

void WebServer::arm_write_if_needed(int i, const Client &client)
{
    if (!client.getWriteBuffer().empty() && client.getEvents() != WAIT_FORM)
        poll_fds[i].events |= POLLOUT;
}

void WebServer::handle_client_readable(int i, Client &client)
{
    if (client.getEvents() == WAIT_FORM)
    {
        readFormData(i);
        if (client.getResponse().getResponseCode() == HttpStatus::PAYLOAD_TOO_LARGE)
            return;
    }
    else
    {
        serverResponse(client);
        if (client.getEvents() == EXITED)
            return;
    }
    arm_write_if_needed(i, client);
}

void WebServer::handle_client_writable(int i, Client &client, short re)
{
    if (can_write(re) && client.getEvents() != WAIT_FORM && !client.getWriteBuffer().empty())
        clientSend(i);
}

void WebServer::process_fd(int i)
{
    short re = poll_fds[i].revents;
    Client &client = clients[i];

    if (has_error(re))
    {
        handle_error_or_hup(i);
        return;
    }

    if (is_server_index(i) && can_read(re))
    {
        handle_server_readable(i);
        return;
    }

    if (is_client_index(i) && can_read(re))
    {
        handle_client_readable(i, client);
        // burada yazılabilirlik aynı poll döngüsünde re'ye göre kontrol edilecek
    }

    if (is_client_index(i))
        handle_client_writable(i, client, re);
}

// --- Parçalanmış start() ---

void WebServer::start()
{
    Log::info("Server is running.");

    for (;;)
    {
        (void)wait_for_events();

        for (int i = (int)poll_fds.size() - 1; i >= 0; --i)
            process_fd(i);
    }
}


const char *WebServer::ServerExcp::what() const throw()
{
    return excp.c_str();
}

WebServer::ServerExcp::ServerExcp(const std::string &err) : excp("\033[31m[ERROR] " + err + ": Couldn't create the server!" + "\033[0m") {}

WebServer::ServerExcp::~ServerExcp() throw() {}
