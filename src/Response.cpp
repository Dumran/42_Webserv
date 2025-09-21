#include <limits>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include "Response.hpp"
#include "Client.hpp"
#include "utils/StringUtils.hpp"
#include "utils/Utils.hpp"

Response::Response() : form_data(""),
                       file(""),
                       request_type(NONE),
                       response_code(HttpStatus::NONE),
                       content(""),
                       is_cgi(false),
                       cgi_path(""),
                       cgi_extension(""),
                       content_length(0),
                       content_type(""),
                       is_chunked(false),
                       response_code_str(""),
                       method_not_allowed(false),
                       add_header("")
{
}

Response::~Response() {}

std::string Response::getFormData() const
{
    return form_data;
}

std::string Response::getFile() const
{
    return file;
}

std::string Response::getPureLink() const
{
    return pure_link;
}

RequestType Response::getRequestType() const
{
    return request_type;
}

int Response::getResponseCode() const
{
    return response_code;
}

std::string Response::getContent() const
{
    return content;
}

bool Response::getIsCgi() const
{
    return is_cgi;
}

std::string Response::getCgiPath() const
{
    return cgi_path;
}

std::string Response::getCgiExtension() const
{
    return cgi_extension;
}

size_t Response::getContentLength() const
{
    return content_length;
}

std::string Response::getContentType() const
{
    return content_type;
}

bool Response::getIsChunked() const
{
    return is_chunked;
}

std::string Response::getResponseCodeStr() const
{
    return HttpStatus::to_string(response_code);
}

bool Response::getMethodNotAllowed() const
{
    return method_not_allowed;
}

bool Response::getAutoIndex() const
{
    return auto_index;
}

std::string Response::getAddHeader() const
{
    return add_header;
}

std::vector<std::string> Response::getMethods() const
{
    return methods;
}

std::string Response::getRedirect() const
{
    return redirect;
}

std::string Response::getCgiRoot() const
{
    return cgi_root;
}

void Response::setFormData(const std::string &formData)
{
    form_data = formData;
}

void Response::setFile(const std::string &file)
{
    this->file = file;
}

void Response::setPureLink(const std::string &pureLink)
{
    pure_link = pureLink;
}

void Response::setRequestType(RequestType requestType)
{
    request_type = requestType;
}

void Response::setResponseCode(int responseCode)
{
    response_code = responseCode;
}

void Response::setContent(const std::string &content)
{
    this->content = content;
}

void Response::setIsCgi(bool isCgi)
{
    is_cgi = isCgi;
}

void Response::setCgiPath(const std::string &cgiPath)
{
    cgi_path = cgiPath;
}

void Response::setCgiExtension(const std::string &cgiExtension)
{
    cgi_extension = cgiExtension;
}

void Response::setContentLength(size_t contentLength)
{
    content_length = contentLength;
}

void Response::setContentType(const std::string &contentType)
{
    content_type = contentType;
}

void Response::setIsChunked(bool isChunked)
{
    is_chunked = isChunked;
}

void Response::setResponseCodeStr(const std::string &responseCodeStr)
{
    response_code_str = responseCodeStr;
}

void Response::setMethodNotAllowed(bool methodNotAllowed)
{
    method_not_allowed = methodNotAllowed;
}

void Response::setAutoIndex(bool autoIndex)
{
    auto_index = autoIndex;
}

void Response::setAddHeader(const std::string &addHeader)
{
    add_header = addHeader;
}

void Response::setMethods(const std::vector<std::string> &methods)
{
    this->methods = methods;
}

void Response::setRedirect(const std::string &redirect)
{
    this->redirect = redirect;
}

void Response::setCgiRoot(const std::string &cgiRoot)
{
    cgi_root = cgiRoot;
}

// void Response::configureFile(std::string file, Server &server)
// {
//     pure_link = file;
//     // Kök dizin isteği için dosya yolunu doğrudan ayarla
//     if (file.empty() || file == "/") {
//         this->file = "./www/html/index.html";
//         this->setResponseCode(HttpStatus::OK);
//         return;
//     }
//     if (file.find('/') == std::string::npos)
//     {
//         this->file = file;
//         return;
//     }
//     this->file = ".";
//     std::vector<std::string> parts = StringUtils::split(file, '/');
//     std::vector<std::string> united_parts;
//     std::string united;

//     for (std::vector<std::string>::size_type i = 0; i < parts.size(); ++i)
//     {
//         united += "/" + parts[i];
//         united_parts.push_back(united);
//     }
//     if (parts.empty())
//         united_parts.push_back("/");

//     int matchedIndex = -1, locationIndex = -1;
//     for (int i = (int)united_parts.size() - 1; i >= 0 && locationIndex == -1; --i)
//     {
//         for (size_t j = 0; j < server.getLocations().size(); ++j)
//         {
//             if (server.getLocations()[j].getPath() == united_parts[i])
//             {
//                 if (cgi_path.empty() && !server.getLocations()[j].getCgiPath().empty())
//                 {
//                     cgi_path = server.getLocations()[j].getCgiPath();
//                     cgi_root = "." + server.getLocations()[j].getRoot();
//                 }
//                 if (cgi_extension.empty() && !server.getLocations()[j].getCgiExtension().empty())
//                     cgi_extension = server.getLocations()[j].getCgiExtension();
//                 locationIndex = j;
//                 matchedIndex = i;
//                 break;
//             }
//         }
//     }

//     if (locationIndex == -1 && server.getRootLocation() == -1 && server.getServerInRoot().empty())
//     {
//         response_code = HttpStatus::INTERNAL_SERVER_ERROR;
//         return;
//     }
//     else if (locationIndex == -1 && server.getRootLocation() == -1 && !server.getServerInRoot().empty())
//     {
//         this->file += server.getServerInRoot();
//         this->methods.push_back("GET");
//         this->methods.push_back("POST");
//         this->methods.push_back("DELETE");
//     }
//     else if (locationIndex != -1 || server.getRootLocation() != -1)
//     {
//         const Location &loc = (locationIndex == -1) ? server.getLocations()[server.getRootLocation()] : server.getLocations()[locationIndex];

//         if (!loc.getRedirect().empty())
//         {
//             redirect = loc.getRedirect();
//             this->setResponseCode(HttpStatus::FOUND);
//             return;
//         }
//         if (!loc.getAddHeader().empty())
//             add_header = loc.getAddHeader();

//         if (!loc.getRoot().empty())
//             this->file += loc.getRoot();
//         else if (!server.getServerInRoot().empty())
//             this->file += server.getServerInRoot();
//         else
//         {
//             this->setResponseCode(HttpStatus::NOT_FOUND);
//             return;
//         }

//         this->setAutoIndex(loc.getAutoIndex());
//         this->setMethods(loc.getMethods());
//     }
//     if (request_type != NONE)
//     {
//         std::string methodName = HttpMethod::names[std::numeric_limits<int>::max() - static_cast<int>(request_type)];
//         if (std::find(methods.begin(), methods.end(), methodName) != methods.end())
//             this->setResponseCode(HttpStatus::OK);
//         else
//             this->setResponseCode(HttpStatus::METHOD_NOT_ALLOWED);
//     }
//     else
//         this->setResponseCode(HttpStatus::METHOD_NOT_ALLOWED);
//     for (size_t i = matchedIndex + 1; i < parts.size(); ++i)
//         this->file += ((this->file[this->file.length() - 1] == '/') ? "" : "/") + parts[i];
// }

// Yardımcılar bu .cpp içinde "private" gibi davranması için static tutuldu.
static bool is_root_request(const std::string &f) {
    return f.empty() || f == "/";
}

static bool has_no_slash(const std::string &f) {
    return f.find('/') == std::string::npos;
}

static void accumulate_parts(const std::vector<std::string> &parts,
                             std::vector<std::string> &united_parts) {
    std::string acc;
    for (std::vector<std::string>::size_type i = 0; i < parts.size(); ++i) {
        acc += "/" + parts[i];
        united_parts.push_back(acc);
    }
    if (parts.empty())
        united_parts.push_back("/");
}

static void maybe_inherit_cgi_from_loc(const Location &loc,
                                       std::string &cgi_path,
                                       std::string &cgi_root,
                                       std::string &cgi_extension) {
    if (cgi_path.empty() && !loc.getCgiPath().empty()) {
        cgi_path  = loc.getCgiPath();
        cgi_root  = "." + loc.getRoot();
    }
    if (cgi_extension.empty() && !loc.getCgiExtension().empty()) {
        cgi_extension = loc.getCgiExtension();
    }
}

static void find_location_match(const std::vector<std::string> &united_parts,
                                const Server &server,
                                int &matchedIndex,
                                int &locationIndex,
                                std::string &cgi_path,
                                std::string &cgi_root,
                                std::string &cgi_extension) {
    matchedIndex  = -1;
    locationIndex = -1;

    for (int i = (int)united_parts.size() - 1; i >= 0 && locationIndex == -1; --i) {
        for (size_t j = 0; j < server.getLocations().size(); ++j) {
            const Location &loc = server.getLocations()[j];
            if (loc.getPath() == united_parts[i]) {
                maybe_inherit_cgi_from_loc(loc, cgi_path, cgi_root, cgi_extension);
                locationIndex = (int)j;
                matchedIndex  = i;
                break;
            }
        }
    }
}

static bool apply_location_config(Response *self,
                                  const Location &loc,
                                  const Server &server) {
    // redirect varsa anında dön
    if (!loc.getRedirect().empty()) {
        self->setRedirect(loc.getRedirect());
        self->setResponseCode(HttpStatus::FOUND);
        return true; // erken dönüş
    }

    if (!loc.getAddHeader().empty())
        self->setAddHeader(loc.getAddHeader());

    if (!loc.getRoot().empty()) {
        self->setFile(self->getFile() + loc.getRoot());
    } else if (!server.getServerInRoot().empty()) {
        self->setFile(self->getFile() + server.getServerInRoot());
    } else {
        self->setResponseCode(HttpStatus::NOT_FOUND);
        return true; // kök belirlenemedi, 404
    }

    self->setAutoIndex(loc.getAutoIndex());
    self->setMethods(loc.getMethods());
    return false; // normal akış
}

static void allow_default_methods(Response *self) {
    std::vector<std::string> defaultMethods = self->getMethods();
    defaultMethods.push_back("GET");
    defaultMethods.push_back("POST");
    defaultMethods.push_back("DELETE");
    self->setMethods(defaultMethods);
}

static void decide_status_by_method(Response *self) {
    if (self->getRequestType() != NONE) {
        std::string methodName =
            HttpMethod::names[std::numeric_limits<int>::max() - static_cast<int>(self->getRequestType())];
        std::vector<std::string> methods = self->getMethods();
        if (std::find(methods.begin(), methods.end(), methodName) != methods.end())
            self->setResponseCode(HttpStatus::OK);
        else
            self->setResponseCode(HttpStatus::METHOD_NOT_ALLOWED);
    } else {
        self->setResponseCode(HttpStatus::METHOD_NOT_ALLOWED);
    }
}

static void append_remaining_segments(Response *self,
                                      const std::vector<std::string> &parts,
                                      int matchedIndex) {
    std::string currentFile = self->getFile();
    for (size_t i = (size_t)(matchedIndex + 1); i < parts.size(); ++i) {
        currentFile += ((currentFile[currentFile.length() - 1] == '/') ? "" : "/") + parts[i];
    }
    self->setFile(currentFile);
}

// -------------------------------------------------------------

void Response::configureFile(std::string file, Server &server)
{
    pure_link = file;

    // 1) Kök istek
    if (is_root_request(file)) {
        this->file = "./www/html/index.html";
        this->setResponseCode(HttpStatus::OK);
        return;
    }

    // 2) Tek segment (slash yoksa) doğrudan dosya
    if (has_no_slash(file)) {
        this->file = file;
        return;
    }

    // 3) Parçaları hazırla
    this->file = ".";
    std::vector<std::string> parts = StringUtils::split(file, '/');
    std::vector<std::string> united_parts;
    accumulate_parts(parts, united_parts);

    // 4) En iyi eşleşen location'ı bul
    int matchedIndex, locationIndex;
    find_location_match(united_parts, server, matchedIndex, locationIndex,
                        cgi_path, cgi_root, cgi_extension);

    // 5) Location bulunamazsa ve server kökü de yoksa -> 500
    if (locationIndex == -1 && server.getRootLocation() == -1 && server.getServerInRoot().empty()) {
        response_code = HttpStatus::INTERNAL_SERVER_ERROR;
        return;
    }
    // 6) Location yok fakat server root varsa onu kullan, default metodları aç
    else if (locationIndex == -1 && server.getRootLocation() == -1 && !server.getServerInRoot().empty()) {
        this->file += server.getServerInRoot();
        allow_default_methods(this);
    }
    // 7) Location bulundu veya rootLocation tanımlı
    else if (locationIndex != -1 || server.getRootLocation() != -1) {
        const Location &loc = (locationIndex == -1)
                                ? server.getLocations()[server.getRootLocation()]
                                : server.getLocations()[locationIndex];

        // Location ayarlarını uygula (redirect varsa erken dön)
        if (apply_location_config(this, loc, server))
            return;
    }

    // 8) Metoda göre durum kodu belirle
    decide_status_by_method(this);

    // 9) Kalan path segmentlerini ekle
    append_remaining_segments(this, parts, matchedIndex);
}


