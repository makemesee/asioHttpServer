#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "server.hpp"

struct ProgramArgs
{
	std::string ipAddr;         	// параметр -h
	std::string port;           	// параметр -p
	std::string serverDirectory;	// параметр -d
} args;

static const char *optString = "h:p:d:";

static const size_t numberOfThreads = 8;

int main(int argc, char* argv[])
{
    /// Демонизируемся
    pid_t pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    ///	Инициализация ProgramArgs дефолтными значениями
    args.ipAddr = "127.0.0.1";
    args.port = "80";
    args.serverDirectory = "/home/box/final/";

    // Процедура обработки входных параметров
    int optStatus = getopt(argc, argv, optString);
    while(optStatus != -1)
    {
        switch(optStatus)
        {
            case 'h':
                args.ipAddr = std::string(optarg); /* true */
                break;
            case 'p':
                args.port = std::string(optarg);
                break;
            case 'd':
                args.serverDirectory = std::string(optarg);
                break;
            default:
                // Сюда на самом деле попасть невозможно
                break;
        }

        optStatus = getopt( argc, argv, optString );
    }

    try
    {
        // Инициализация объекта-сервера
        HTTPServer server(args.ipAddr, args.port, args.serverDirectory, numberOfThreads);
        // Запуск сервера.
        server.Run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Исключительная ситуация: " << e.what() << "\n";
    }

    return 0;
}
