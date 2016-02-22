#pragma once

#include <string>
#include <vector>
#include <string>
#include <boost/noncopyable.hpp>

#include "header.hpp"
#include "server_response.hpp"


/// Структура запроса клиента
struct ClientRequest
{
    std::string method;
    std::string uri;
    int httpVersionMajor;
    int httpVersionMinor;
    std::vector<HTTPheader> headers;
};


class RequestHandler : private boost::noncopyable
{
public:
    explicit RequestHandler(const std::string& docDir);

  /// Handle a request and produce a reply.
    void HandleIncomingRequest(const ClientRequest &, ServerResponse &);

private:
    std::string m_ServerFilesDir;

    static bool DecodeURL(const std::string &in, std::string &out);
};
