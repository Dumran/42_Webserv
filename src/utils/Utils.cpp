#include "utils/Utils.hpp"
#include <sstream>
#include "Client.hpp"

// Eksik chunked fonksiyonları
std::string Utils::chunkedGetLine(std::istringstream &src, int size) {
    std::string line;
    if (size == 0)
        std::getline(src, line);
    else {
        char *buf = new char[size + 1];
        src.read(buf, size);
        buf[size] = '\0';
        line = std::string(buf);
        delete[] buf;
    }
    return line;
}

void chunkedCompleted(Client &client, const std::string &aggregate) {
    (void)client;
    (void)aggregate;
    // Chunked işlemi tamamlandığında yapılacaklar buraya eklenebilir
}
// Eksik fonksiyonların basit gövdeleri
#include <fstream>
#include <sstream>
#include "Client.hpp"
bool readFileToString(const std::string &path, std::string &out) {
    std::ifstream file(path.c_str());
    if (!file.is_open()) return false;
    std::ostringstream ss;
    ss << file.rdbuf();
    out = ss.str();
    return true;
}

std::string generateAutoIndex(const std::string &dirPath, const std::string &requestPath, const Client &client) {
    (void)requestPath;
    (void)client;
    return "<html><body><h1>AutoIndex for " + dirPath + "</h1></body></html>";
}
#include <string>
#include "Client.hpp"

// Fonksiyon prototipleri
bool readFileToString(const std::string &path, std::string &out);
std::string generateAutoIndex(const std::string &dirPath, const std::string &requestPath, const Client &client);
#include "utils/Utils.hpp"
#include <iostream>
#include <cstdio>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <ctime>
#include <signal.h>
#include <string>
#include "Response.hpp"
#include "Client.hpp"
#include "utils/ResponseUtils.hpp"

bool Utils::wait(pid_t pid, int timeout_seconds)
{
    time_t start = std::time(NULL);

    while (true)
    {
        pid_t result = waitpid(pid, NULL, WNOHANG);
        if (result == pid)
            return true;
        else if (result == -1)
            return false;
        if (std::time(NULL) - start >= timeout_seconds)
            break;
    }

    kill(pid, SIGKILL);
    waitpid(pid, NULL, 0);
    return false;
}

std::string Utils::getTime()
{
    std::time_t t = std::time(NULL);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return (std::string(buffer));
}

// std::string Utils::readFile(const std::string &fileName, Response &response, Client &client, int code)
// {
//     int respCode = response.getResponseCode();
//     if (respCode != 0 && respCode != 200 && respCode != -1)
//         return Utils::resToErrorPages(response, respCode, client);

//     struct stat st;
//     if (stat(fileName.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
//     {
//         const std::string &pureLink = response.getPureLink();
//         if (pureLink.empty() || pureLink[pureLink.size() - 1] != '/')
//             return Utils::resToErrorPages(response, HttpStatus::MOVED_PERMANENTLY, client);

//         std::string indexPath = fileName + "/index.html";
//         if (access(indexPath.c_str(), R_OK) == 0)
//         {
//             std::string body;
//             if (readFileToString(indexPath, body))
//             {
//                 response.setResponseCode(code);
//                 return body;
//             }
//         }
//         if (response.getAutoIndex())
//         {
//             return generateAutoIndex(fileName, client.getResponse().getFile(), client);
//         }
//         return Utils::resToErrorPages(response, HttpStatus::FORBIDDEN, client);
//     }
//     const Server &srv = client.getServer();

//     if (!client.getResponse().getCgiPath().empty() && !srv.getCgiExtensionInServer().empty())
//     {
//         if (access(srv.getCgiPathInServer().c_str(), X_OK) != 0 || access(fileName.c_str(), F_OK) != 0 || access(fileName.c_str(), R_OK) != 0)
//         {
//             if (access(fileName.c_str(), R_OK) != 0)
//                 return Utils::resToErrorPages(response, HttpStatus::INTERNAL_SERVER_ERROR, client);
//             return Utils::resToErrorPages(
//                 response,
//                 access(srv.getCgiPathInServer().c_str(), X_OK) != 0
//                     ? HttpStatus::INTERNAL_SERVER_ERROR
//                     : HttpStatus::NOT_FOUND,
//                 client);
//         }
//         if (access(fileName.c_str(), X_OK) != 0)
//             return Utils::resToErrorPages(response, HttpStatus::INTERNAL_SERVER_ERROR, client);

//         if (fileName.find(srv.getCgiExtensionInServer()) == std::string::npos)
//             return Utils::resToErrorPages(response, HttpStatus::INTERNAL_SERVER_ERROR, client);
//         response.setIsCgi(true);
//         return "";
//     }
//     std::string body;
//     if (readFileToString(fileName, body))
//     {
//         response.setResponseCode(code);
//         return body;
//     }
//     return Utils::resToErrorPages(response, HttpStatus::NOT_FOUND, client);
// }

// ---- Yardımcılar (aynı .cpp içinde static tutabilirsiniz) ----

static bool has_predetermined_response(const Response &resp) {
    const int c = resp.getResponseCode();
    return (c != 0 && c != 200 && c != -1);
}

static bool is_directory(const std::string &path, struct stat &st) {
    return (::stat(path.c_str(), &st) == 0) && S_ISDIR(st.st_mode);
}

// Dizine girildiğinde orijinal akış: trailing slash kontrolü, index.html, autoindex, aksi FORBIDDEN
static std::string serve_directory_case(const std::string &dirPath,
                                        Response &response,
                                        Client &client,
                                        int code)
{
    const std::string &pureLink = response.getPureLink();

    // trailing slash yoksa 301
    if (pureLink.empty() || pureLink[pureLink.size() - 1] != '/')
        return Utils::resToErrorPages(response, HttpStatus::MOVED_PERMANENTLY, client);

    // index.html varsa onu oku
    const std::string indexPath = dirPath + "/index.html";
    if (::access(indexPath.c_str(), R_OK) == 0)
    {
        std::string body;
        if (readFileToString(indexPath, body))
        {
            response.setResponseCode(code);
            return body;
        }
        // okuma başarısız ise, autoindex/forbidden akışına düşer
    }

    // autoindex açıksa liste üret
    if (response.getAutoIndex())
        return generateAutoIndex(dirPath, client.getResponse().getFile(), client);

    // aksi halde erişim yasak
    return Utils::resToErrorPages(response, HttpStatus::FORBIDDEN, client);
}

static bool should_try_cgi(const Response &resp, const Server &srv) {
    return (!resp.getCgiPath().empty() && !srv.getCgiExtensionInServer().empty());
}

// CGI çalıştırma önkoşulları ve karar mantığı (orijinalle birebir)
static std::string handle_cgi_case(Response &response,
                                   Client &client,
                                   const std::string &fileName,
                                   const Server &srv)
{
    // cgi binary çalıştırılabilir mi, dosya mevcut ve okunabilir mi?
    if (::access(srv.getCgiPathInServer().c_str(), X_OK) != 0 ||
        ::access(fileName.c_str(), F_OK) != 0 ||
        ::access(fileName.c_str(), R_OK) != 0)
    {
        if (::access(fileName.c_str(), R_OK) != 0)
            return Utils::resToErrorPages(response, HttpStatus::INTERNAL_SERVER_ERROR, client);

        return Utils::resToErrorPages(
            response,
            (::access(srv.getCgiPathInServer().c_str(), X_OK) != 0)
                ? HttpStatus::INTERNAL_SERVER_ERROR
                : HttpStatus::NOT_FOUND,
            client);
    }

    // script dosyasının da çalıştırılabilir olması bekleniyor
    if (::access(fileName.c_str(), X_OK) != 0)
        return Utils::resToErrorPages(response, HttpStatus::INTERNAL_SERVER_ERROR, client);

    // uzantı eşleşmesi (orijinal: substring arama)
    if (fileName.find(srv.getCgiExtensionInServer()) == std::string::npos)
        return Utils::resToErrorPages(response, HttpStatus::INTERNAL_SERVER_ERROR, client);

    // CGI akışına geçileceğini işaretle ve gövdeyi daha sonra üret
    response.setIsCgi(true);
    return std::string();
}

static std::string serve_static_file(const std::string &fileName,
                                     Response &response,
                                     Client &client,
                                     int code)
{
    std::string body;
    if (readFileToString(fileName, body))
    {
        response.setResponseCode(code);
        return body;
    }
    return Utils::resToErrorPages(response, HttpStatus::NOT_FOUND, client);
}

// ---- Asıl fonksiyon (koordinasyon) ----

std::string Utils::readFile(const std::string &fileName, Response &response, Client &client, int code)
{
    // Önceden belirlenmiş bir hata kodu varsa doğrudan onun sayfasını dön
    if (has_predetermined_response(response))
        return Utils::resToErrorPages(response, response.getResponseCode(), client);

    // Dizin mi?
    struct stat st;
    if (is_directory(fileName, st))
        return serve_directory_case(fileName, response, client, code);

    // CGI mi denenmeli?
    const Server &srv = client.getServer();
    if (should_try_cgi(client.getResponse(), srv))
    {
        std::string cgi_result = handle_cgi_case(response, client, fileName, srv);
        return cgi_result; // "" olabilir (CGI akışı için), ya da hata sayfası/gövde
    }

    // Düz dosya oku
    return serve_static_file(fileName, response, client, code);
}


bool Utils::readFileToString(const std::string &path, std::string &out)
{
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    if (!file)
        return false;
    file.seekg(0, std::ios::end);
    std::streamoff size = file.tellg();
    if (size > 0)
    {
        out.resize(static_cast<std::size_t>(size));
        file.seekg(0, std::ios::beg);
        file.read(&out[0], size);
    }
    else
    {
        out.clear();
    }
    return true;
}

void Utils::doubleSeparator(std::string key, std::string &buffer, Client &client)
{
    std::string target = "=";
    size_t firstPos = key.find(target);
    if (firstPos == std::string::npos)
        return;
    std::string separator = key.substr(firstPos + 1);
    if (countSeparator(buffer, separator) > 1)
    {
        size_t firstIndex = buffer.find(separator, buffer.find(separator) + 1);
        if (firstIndex != std::string::npos)
        {
            std::string temp = buffer.substr(firstIndex - 2);
            if (temp.length() == client.getResponse().getContentLength())
                client.getResponse().setFormData(temp);
            else
                client.getFormData().append(temp);
        }
    }
    if (client.getResponse().getFormData().length() != client.getResponse().getContentLength())
        client.setEvents(WAIT_FORM);
}

int Utils::countSeparator(const std::string &buffer, const std::string &target)
{
    int count = 0;
    size_t pos = buffer.find(target);

    while (pos != std::string::npos)
    {
        count++;
        pos = buffer.find(target, pos + target.length());
    }
    return count;
}

std::string Utils::getHostHeader(const std::string &request)
{
    size_t host_start = request.find("Host:");
    if (host_start == std::string::npos)
        return "";
    size_t host_end = request.find("\r\n", host_start);
    std::string host = request.substr(host_start + 5, host_end - host_start - 5);
    size_t port_pos = host.find(":");
    if (port_pos != std::string::npos)
    {
        host = host.substr(0, port_pos);
    }
    return Config::trim_space(host);
}

void Utils::getServerByHost(const std::string &host, Client &client)
{
    std::vector<Server> &possible = client.getConfig().getPossibleServers();

    for (size_t i = 0; i < possible.size(); i++)
    {
        Server &srv = possible[i];

        for (size_t j = 0; j < srv.getServerNames().size(); j++)
        {
            if (srv.getServerNames()[j] == host)
            {
                client.setServer(srv);
                client.setMaxBodySize(srv.getClientMaxBodySize());
                return;
            }
        }
    }
    client.setMaxBodySize(client.getConfig().getDefault().getClientMaxBodySize());
}

// void Utils::parseChunked(Client &client, std::string &body)
// {
//     std::istringstream tempBody(body);
//     std::string result;
//     std::string line;
//     int size = 0;
//     std::string temp;

//     line = chunkedGetLine(tempBody, 0);
//     while (!line.empty())
//     {
//         size = 0;
//         std::istringstream hexStream(line);
//         hexStream >> std::hex >> size;
//         if (size == 0)
//         {
//             size = -1;
//             break;
//         }
//         std::string temp;
//         while (temp.length() != (size_t)size)
//         {
//             line = chunkedGetLine(tempBody, size);
//             temp.append(line);
//         }
//         result.append(temp);
//         line = chunkedGetLine(tempBody, 2);
//         line = chunkedGetLine(tempBody, 0);
//     }

//     if (size == -1)
//     {
//         if (result.length() > client.getMaxBodySize())
//         {
//             client.getResponse().setContent(Utils::resToErrorPages(client.getResponse(), HttpStatus::PAYLOAD_TOO_LARGE, client));
//             return;
//         }
//         chunkedCompleted(client, result);
//     }
// }

// ---- Yardımcılar (aynı .cpp içinde static tutabilirsiniz) ----


static int parse_hex_chunk_size(const std::string &line) {
    int sz = 0;
    std::istringstream hexStream(line);
    hexStream >> std::hex >> sz;
    return sz; // 0 olabilir (son)
}

static void read_exact_chunk(std::istringstream &src, int size, std::string &chunk_out) {
    chunk_out.clear();
    std::string piece;
    while (chunk_out.length() != (std::size_t)size) {
        piece = Utils::chunkedGetLine(src, size);
        chunk_out.append(piece);
    }
}


static bool finalize_chunking(Client &client, const std::string &aggregate) {
    if (aggregate.length() > client.getMaxBodySize()) {
        client.getResponse().setContent(
            Utils::resToErrorPages(client.getResponse(), HttpStatus::PAYLOAD_TOO_LARGE, client)
        );
        return false;
    }
    chunkedCompleted(client, aggregate);
    return true;
}

// ---- Asıl fonksiyon (koordine eder) ----

void Utils::parseChunked(Client &client, std::string &body)
{
    std::istringstream tempBody(body);
    std::string result;
    std::string line;

    // İlk boyut satırını al
    line = chunkedGetLine(tempBody, 0);

    int size_flag = 0; // -1 => son chunk görüldü
    while (!line.empty())
    {
        int size = parse_hex_chunk_size(line);
        if (size == 0) { size_flag = -1; break; }

        std::string chunk;
        read_exact_chunk(tempBody, size, chunk);
        result.append(chunk);

        // CRLF tüket ve yeni boyut satırına geç
        line = chunkedGetLine(tempBody, 2);
        line = chunkedGetLine(tempBody, 0);
    }

    if (size_flag == -1)
        (void)finalize_chunking(client, result);
}


// std::string Utils::chunkedGetLine(std::istringstream &stream, int type)
// {
//     std::string temp;

//     char *buffer = new char[100240];
//     if (type == 0)
//     {
//         while (temp.find("\r\n") == std::string::npos && !stream.eof())
//         {
//             stream.read(buffer, 1);
//             temp.append(buffer, 1);
//         }
//     }
//     else
//     {
//         stream.read(buffer, type);
//         temp.append(buffer, type);
//     }
//     delete[] buffer;
//     return (temp);
// }

// void Utils::chunkedCompleted(Client &client, std::string &result)
// {
//     std::string key = client.getResponse().getContentType();
//     size_t firstPos = key.find("=");

//     client.setFormData(result);
//     client.getResponse().setContentLength(result.length());
//     client.getResponse().setFormData(result);
//     client.getResponse().setIsChunked(false);
//     if (firstPos == std::string::npos)
//         return;

//     std::string separator = key.substr(firstPos + 1);
//     if (countSeparator(result, separator) > 1)
//     {
//         size_t firstIndex = result.find(separator, result.find(separator) + 1);
//         if (firstIndex != std::string::npos)
//         {
//             std::string temp = result.substr(firstIndex - 2);
//             if (temp.length() == client.getResponse().getContentLength())
//                 client.getResponse().setFormData(temp);
//             else
//                 client.getFormData().append(temp);
//         }
//     }
// }

// std::string Utils::resToErrorPages(Response &response, int error_type, Client &client)
// {
//     Server &server = client.getServer();
//     std::string file = "";
//     std::map<int, std::string>::iterator it = server.getErrorPages().find(error_type);

//     response.setResponseCode(error_type);
//     if (it != server.getErrorPages().end())
//         file = "." + it->second;

//     std::ifstream nf(file.c_str());
//     std::stringstream buffer;
//     if (nf)
//     {
//         buffer << nf.rdbuf();
//         return buffer.str();
//     }
//     return response.getResponseCodeStr();
// }

// ---- Yardımcılar (aynı .cpp içinde static tutabilirsiniz) ----

static std::string resolve_error_file(Server &server, int error_type)
{
    std::string file;
    std::map<int, std::string>::iterator it = server.getErrorPages().find(error_type);
    if (it != server.getErrorPages().end())
        file = "." + it->second;
    return file;
}

static bool read_whole_file_to_string(const std::string &path, std::string &out)
{
    std::ifstream nf(path.c_str());
    if (!nf) return false;
    std::stringstream buffer;
    buffer << nf.rdbuf();
    out = buffer.str();
    return true;
}

// ------------------------------------------------------------

std::string Utils::resToErrorPages(Response &response, int error_type, Client &client)
{
    Server &server = client.getServer();
    response.setResponseCode(error_type);

    const std::string file = resolve_error_file(server, error_type);

    std::string content;
    if (!file.empty() && read_whole_file_to_string(file, content))
        return content;

    return response.getResponseCodeStr();
}


std::string Utils::getResponseHeader(Client &client)
{
    int responseCode = client.getResponse().getResponseCode();
    std::ostringstream statusLine;
    statusLine << "HTTP/1.1 " << responseCode << " " << HttpStatus::to_string(responseCode) << "\r\n";
    std::string header = statusLine.str();
    // Yanıtın ilk 32 byte'ını hex olarak logla
    // Her zaman Content-Type: text/html ekle
    header += "Content-Type: text/html\r\n";
    if (responseCode == HttpStatus::METHOD_NOT_ALLOWED)
    {
        header += "Allow: ";
        std::vector<std::string> methods = client.getResponse().getMethods();
        for (size_t i = 0; i < methods.size(); i++)
        {
            header += methods[i];
            if (i != methods.size() - 1)
                header += ", ";
        }
        header += "\r\n";
    }
    if (responseCode == HttpStatus::MOVED_PERMANENTLY)
        header += "Location: " + client.getResponse().getPureLink() + "/\r\n";
    else if (responseCode == HttpStatus::FOUND)
        header += "Location: " + client.getResponse().getRedirect() + "\r\n";
    if (!client.getResponse().getAddHeader().empty() && responseCode == HttpStatus::OK)
    {
        const std::string &add_header = client.getResponse().getAddHeader();
        if (add_header.find("attachment") != std::string::npos)
        {
            std::string file_name = client.getResponse().getFile();
            size_t i = 0;
            size_t j = file_name.find("/");
            while (j != std::string::npos)
            {
                i = j;
                j = file_name.find("/", j + 1);
            }
            header += add_header + "; filename=\"" + file_name.substr(i + 1) + "\" \r\n";
        }
        else
            header += add_header + "\r\n";
    }
    header += "Connection: keep-alive\r\n";
    std::ostringstream oss;
    std::string body = client.getResponse().getContent();
    oss << body.length();
    header += "Content-Length: " + oss.str() + "\r\n";
    header += "\r\n";
    if (body.empty())
        header += "<html><body><h1>Sunucu yanıtı boş!</h1></body></html>";
    else
        header += body;
    return header;
}


void Utils::parseRsp(std::string &buffer, Client &client)
{
    std::string request(buffer);
    Response &response = client.getResponse();
    if (client.getEvents() == REQUEST && client.getResponse().getRequestType() == NONE)
    {
        if (request.find("DELETE ") == 0)
            response.setRequestType(static_cast<RequestType>(HttpMethod::DELETE));
        else if (request.find("POST ") == 0)
            response.setRequestType(static_cast<RequestType>(HttpMethod::POST));
        else if (request.find("GET ") == 0)
            response.setRequestType(static_cast<RequestType>(HttpMethod::GET));
        else
            response.setRequestType(NONE);
        response.configureFile(ResponseUtils::extractFileName(request), client.getServer());
        response.setContentType(ResponseUtils::getContentType(request));
        response.setContentLength(ResponseUtils::utilGetContentLength(request, response));
        if (response.getContentLength() > client.getMaxBodySize())
        {
            response.setContent(Utils::resToErrorPages(response, HttpStatus::PAYLOAD_TOO_LARGE, client));
            return;
        }
        response.setContent(Utils::readFile(response.getFile(), response, client, HttpStatus::OK));
        if ((static_cast<int>(response.getRequestType()) == HttpMethod::POST || static_cast<int>(response.getRequestType()) == HttpMethod::DELETE))
            getBufferFormData(buffer, client);
    }
}

void Utils::getBufferFormData(std::string &buffer, Client &client)
{
    std::string contentType = client.getResponse().getContentType().substr(0, client.getResponse().getContentType().find(";"));
    if (client.getResponse().getIsChunked())
        parseChunkedFt(client, buffer, 0);
    else if (!contentType.find("multipart/form-data"))
        Utils::doubleSeparator(client.getResponse().getContentType() , buffer, client);
    else if (!contentType.find("application/x-www-form-urlencoded"))
    {
        if (buffer.find("\r\n\r\n") != std::string::npos)
            client.getResponse().setFormData(buffer.substr(buffer.find("\r\n\r\n") + 4));
    }
    else if (contentType.find("text/plain") != std::string::npos) {
        size_t pos = buffer.find("\r\n\r\n");
        if (pos != std::string::npos)
            client.getResponse().setFormData(buffer.substr(pos + 4));
    }
    else {
        client.getResponse().setResponseCode(HttpStatus::UNSUPPORTED_MEDIA_TYPE);
        client.getResponse().setContent(Utils::resToErrorPages(client.getResponse(), HttpStatus::UNSUPPORTED_MEDIA_TYPE, client));
    }
}

void Utils::parseChunkedFt(Client &client, std::string &body, int type)
{
    if (!type) {
        size_t headerEnd = body.find("\r\n\r\n");
        if (headerEnd != std::string::npos)
            body = body.substr(headerEnd + 4);
    }
    if (body.find("0\r\n\r\n") == std::string::npos) {
        client.getFormData().append(body);
        client.setEvents(WAIT_FORM);
    }
    else
    {
        client.getResponse().setFormData(body);
        client.getResponse().setIsChunked(false);
        client.getResponse().setContentLength(body.length());
    }
}

// std::string Utils::generateAutoIndex(const std::string &path, const std::string &requestPath, Client &client)
// {
//     DIR *dir;
//     struct dirent *entry;
//     struct stat info;
//     std::ostringstream html;

//     html << "<html><head><title>Index of " << requestPath << "</title></head>\n";
//     html << "<body><h1>Index of " << requestPath << "</h1><ul>\n";

//     dir = opendir(path.c_str());
//     if (!dir)
//     {
//         html << "<p>Failed to open directory</p></body></html>";
//         return html.str();
//     }

//     while ((entry = readdir(dir)) != NULL)
//     {
//         std::string name = entry->d_name;

//         if (name == "." || name == "..")
//             continue;

//         std::string fullPath = path + "/" + name;

//         if (stat(fullPath.c_str(), &info) == 0 && S_ISDIR(info.st_mode))
//         {
//             name += "/";
//         }

//         html << "<li><a href=\"" << client.getResponse().getPureLink() + name << "\">" << name << "</a></li>\n";
//     }

//     closedir(dir);

//     html << "</ul></body></html>";
//     return html.str();
// }

std::string Utils::generateAutoIndex(const std::string &path,
                                     const std::string &requestPath,
                                     Client &client)
{
    // Çıktıyı parça parça biriktireceğiz
    std::string html;
    html.reserve(1024);

    // Başlık ve açılışlar (orijinal akışa sadık: önce yaz, sonra opendir dene)
    html.append("<html><head><title>Index of ");
    html.append(requestPath);
    html.append("</title></head>\n<body><h1>Index of ");
    html.append(requestPath);
    html.append("</h1><ul>\n");

    // Dizin açma
    DIR *d = opendir(path.c_str());
    if (!d)
    {
        // Orijinal davranış: <ul> zaten yazılmışken burada hata basıp kapat
        html.append("<p>Failed to open directory</p></body></html>");
        return html;
    }

    // Taban linki bir kere al
    const std::string base = client.getResponse().getPureLink();

    // readdir döngüsü (C++98)
    for (;;)
    {
        struct dirent *ent = readdir(d);
        if (!ent) break;

        const char *raw = ent->d_name;
        if (!raw) continue;

        // "." veya ".." atla (string oluşturmadan hızlı kontrol)
        if (raw[0] == '.')
        {
            // "." veya ".." kontrolü
            if (raw[1] == '\0') continue;
            if (raw[1] == '.' && raw[2] == '\0') continue;
        }

        // İsim string'i
        std::string entryName(raw);

        // Tam yol: path + "/" + name
        std::string absolute;
        absolute.reserve(path.size() + 1 + entryName.size());
        absolute.append(path);
        absolute.append("/");
        absolute.append(entryName);

        // Dizinse sonuna "/" ekle (hem görünen adda hem href'te)
        struct stat st;
        if (::stat(absolute.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
        {
            entryName.append("/");
        }

        // <li><a href="...">...</a></li>
        html.append("<li><a href=\"");
        html.append(base);
        html.append(entryName);
        html.append("\">");
        html.append(entryName);
        html.append("</a></li>\n");
    }

    closedir(d);

    html.append("</ul></body></html>");
    return html;
}