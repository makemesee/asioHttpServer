#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>

#include "mime.hpp"

RequestHandler::RequestHandler(const std::string &docDir) :
    m_ServerFilesDir(docDir)
{}

void RequestHandler::HandleIncomingRequest(const ClientRequest& req, ServerResponse& response)
{
    // Преобразование url -> path.
    std::string requestedPath;
    if (!DecodeURL(req.uri, requestedPath))
    {
        response = ServerResponse::stock_reply(ServerResponse::bad_request);
        return;
    }

    bool pathIsNotAbsolute = (requestedPath[0] != '/' || (requestedPath.find("..") != std::string::npos));

    if (requestedPath.empty() || pathIsNotAbsolute)
    {
        response = ServerResponse::stock_reply(ServerResponse::bad_request);
        return;
    }

    // Определение типа файла.
    std::size_t last_slash_pos = requestedPath.find_last_of("/");
    std::size_t last_dot_pos = requestedPath.find_last_of(".");
    std::string extension;
    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
    {
        extension = requestedPath.substr(last_dot_pos + 1);
    }

    // Открываем нужный файл.
    std::string fullPath = m_ServerFilesDir + requestedPath;
    std::ifstream is(fullPath.c_str(), std::ios::in | std::ios::binary);
    if (!is)
    {
        response = ServerResponse::stock_reply(ServerResponse::not_found);
        return;
    }

    // Формируем ответ клиенту.
    response.responseStatus = ServerResponse::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
    response.m_ResponseContent.append(buf, is.gcount());
    response.m_Headers.resize(2);
    response.m_Headers[0].name = "Content-Length";
    response.m_Headers[0].value = boost::lexical_cast<std::string>(response.m_ResponseContent.size());
    response.m_Headers[1].name = "Content-Type";
    response.m_Headers[1].value = ExtensionToMIME(extension);
}

bool RequestHandler::DecodeURL(const std::string &in, std::string &out)
{
    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i)
    {
        if (in[i] == '%')
        {
            if (i + 3 <= in.size())
            {
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value)
                {
                    out += static_cast<char>(value);
                    i += 2;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else if (in[i] == '+')
        {
            out += ' ';
        }
        else if (in[i] == '?')
        {
            break;
        }
        else
        {
            out += in[i];
        }
    }
    return true;
}
