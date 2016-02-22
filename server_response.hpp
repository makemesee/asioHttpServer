#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "header.hpp"

/// Форма ответа для клиента
struct ServerResponse
{
    enum status_type
    {
        ok = 200,
        created = 201,
        accepted = 202,
        no_content = 204,
        multiple_choices = 300,
        moved_permanently = 301,
        moved_temporarily = 302,
        not_modified = 304,
        bad_request = 400,
        unauthorized = 401,
        forbidden = 403,
        not_found = 404,
        internal_server_error = 500,
        not_implemented = 501,
        bad_gateway = 502,
        service_unavailable = 503
    } responseStatus;

    /// Заголовки, включаемые в ответ.
    std::vector<HTTPheader> m_Headers;

    /// Содержимое ответа.
    std::string m_ResponseContent;

    /// Преобразовать ответ сервера в вектор буферов.
    /// "The buffers do not own the underlying memory blocks
    /// therefore the reply object must remain valid and
    /// not be changed until the write operation has completed".
    std::vector<boost::asio::const_buffer> ToBuffers();

    static ServerResponse stock_reply(status_type);
};