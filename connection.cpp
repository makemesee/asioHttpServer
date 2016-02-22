#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>

Connection::Connection(boost::asio::io_service& io_service, RequestHandler &handler) :
    m_Socket(io_service),
    m_RequestHandler(handler)
{}

boost::asio::ip::tcp::socket const & Connection::GetSocket() const
{
  return m_Socket;
}

boost::asio::ip::tcp::socket & Connection::GetSocket()
{
  return m_Socket;
}

void Connection::Start()
{
    m_Socket.async_read_some(
          boost::asio::buffer(m_Data),
                  boost::bind(
                          &Connection::ReadHandler,
                          shared_from_this(),
                          boost::asio::placeholders::error,
                          boost::asio::placeholders::bytes_transferred
                  )
    );
}

void Connection::ReadHandler(const boost::system::error_code &error, std::size_t bytesTransmitted)
{
    if (!error)
    {
        boost::tribool statusOK;
        boost::tie(statusOK, boost::tuples::ignore) = m_RequestParser.parse(m_Request, m_Data.data(), m_Data.data() + bytesTransmitted);

        if(statusOK)
        {
            m_RequestHandler.HandleIncomingRequest(m_Request, m_ServerResponse);

            boost::asio::async_write(
                  m_Socket,
                  m_ServerResponse.ToBuffers(),
                  boost::bind(
                          &Connection::WriteHandler,
                          shared_from_this(),
                          boost::asio::placeholders::error
                  )
            );
        }
        else if(!statusOK)
        {
            m_ServerResponse = ServerResponse::stock_reply(ServerResponse::bad_request);

            boost::asio::async_write(
                    m_Socket,
                    m_ServerResponse.ToBuffers(),
                    boost::bind(
                            &Connection::WriteHandler,
                            shared_from_this(),
                            boost::asio::placeholders::error
                    )
            );
        }
        else
        {
            m_Socket.async_read_some(
                    boost::asio::buffer(m_Data),
                    boost::bind(
                            &Connection::ReadHandler,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred
                            )
            );
        }
    }
}

void Connection::WriteHandler(const boost::system::error_code &error)
{
    if (!error)
    {
        m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    }
    // Не инициируем новых асинхронных операций => память освобождается и вызывается деструктор Connection
}
