#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "connection.hpp"
#include "request_handler.hpp"
#include <boost/thread/thread.hpp>

using namespace boost::asio;

class HTTPServer : private boost::noncopyable
{
public:
    explicit HTTPServer(const std::string& address,
          const std::string& port,
          const std::string& fileDirectory,
          std::size_t threadPoolSize);

    /// Запуск цикла io_service
    void Run();

private:
  void StartAccept();

  void AcceptHandler(const boost::system::error_code&);

  void StopHandler();

  /// Количество потоков в пуле, которые будут производить асинхронную обработку
  /// ( вызывать io_service.run() )
  size_t m_ThreadGroupSize;

  /// Главный объект библиотеки boost::asio, реализующий взаимодействие с ОС и асинхронную обработку событий
  io_service m_io_service;

  /// Acceptor для приёма входных соединений
  ip::tcp::acceptor m_MasterAcceptor;

  Connection::connection_ptr m_NextConnection;

  /// Обработчик входящих запросов.
  RequestHandler request_handler_;
};


