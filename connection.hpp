#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "request_handler.hpp"
#include "request_parser.hpp"


class Connection :
        public boost::enable_shared_from_this<Connection>,
        private boost::noncopyable
{
public:
    typedef boost::shared_ptr<Connection> connection_ptr;

    explicit Connection(boost::asio::io_service &, RequestHandler &);

    boost::asio::ip::tcp::socket const & GetSocket() const;
    boost::asio::ip::tcp::socket & GetSocket();

    /// Инициировать первую асинхронную операцию в созданной сессии
    void Start();

private:
    typedef boost::system::error_code error_code;

    void ReadHandler(const error_code &, std::size_t bytesTransmitted);

    void WriteHandler(const error_code& e);

    boost::asio::ip::tcp::socket m_Socket;

    /// Входящий клиентский запрос.
    ClientRequest m_Request;

    /// Ссылка на обработчик входящих соединений
    RequestHandler& m_RequestHandler;

    /// Парсер запросов.
    request_parser m_RequestParser;

    /// Объект-конструктор исходящих ответов.
    ServerResponse m_ServerResponse;

    /// Буфер для входных данных
    boost::array<char, 8192> m_Data;


};
