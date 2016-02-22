#include "server.hpp"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

HTTPServer::HTTPServer(const std::string& address,
		const std::string& port,
		const std::string& fileDirectory,
		std::size_t threadPoolSize) :
		m_ThreadGroupSize(threadPoolSize),
		m_MasterAcceptor(m_io_service),
		m_NextConnection(),
		request_handler_(fileDirectory)
{
	ip::tcp::resolver resolver(m_io_service);
	ip::tcp::resolver::query query(address, port);
	ip::tcp::endpoint endpoint = *resolver.resolve(query);
	m_MasterAcceptor.open(endpoint.protocol());
	m_MasterAcceptor.set_option(ip::tcp::acceptor::reuse_address(true));
	m_MasterAcceptor.bind(endpoint);
	m_MasterAcceptor.listen();

	StartAccept();
}

void HTTPServer::Run()
{
    boost::thread_group threads;
    for (std::size_t i = 0; i < m_ThreadGroupSize; ++i)
    {
        threads.create_thread(boost::bind(&boost::asio::io_service::run, &m_io_service));
    }
    threads.join_all();
}

void HTTPServer::StartAccept()
{
    m_NextConnection.reset(new Connection(m_io_service, request_handler_));

    m_MasterAcceptor.async_accept(
            m_NextConnection->GetSocket(),
            boost::bind(
                    &HTTPServer::AcceptHandler,
                    this,
                    boost::asio::placeholders::error
            )
    );
}

void HTTPServer::AcceptHandler(const boost::system::error_code& e)
{
    if (!e)
    {
        m_NextConnection->Start();
    }

    // Чтобы не прерывать работу io_service.run()
    StartAccept();
}

void HTTPServer::StopHandler()
{
    m_io_service.stop();
}
