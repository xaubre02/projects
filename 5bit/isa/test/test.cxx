#include <cstdlib>
#include <thread>
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SOCKET_ERROR    (-1)
#define DEST_ADDR       "127.0.0.1"
#define DEST_PORT       "5555"

typedef int SOCKET;

SOCKET conn;

bool connectTo(const char *ip_addr, const char *port)
{
    struct addrinfo *result = NULL;

    if (getaddrinfo(ip_addr, port, NULL, &result))
        return false;

    for (result = result; result != NULL; result = result->ai_next)
    {
        conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (conn == SOCKET_ERROR)
            return false;

        if (connect(conn, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
        {
            close(conn);
            continue;
        }

        freeaddrinfo(result);
        return true;
    }

    freeaddrinfo(result);
    return false;
}

bool sendData(const char *data)
{
    if (send(conn, data, int(strlen(data)), 0) == SOCKET_ERROR)
        return false;

    return true;
}

int sendPayload(int number)
{
    for(int c = 0; c < number; c++)
    {
        if (connectTo(DEST_ADDR, DEST_PORT))
        {
            std::cout << "Connected!\n";
            if(!sendData("Just testing..."))
            {
                close(conn);
                std::cerr << "Something went wrong!\n";
                return EXIT_FAILURE;
            }
        }
        else
        {
            close(conn);
            std::cerr << "Can't connect to server...\n";
            return EXIT_FAILURE;
        }
    }
    
    close(conn);
    return EXIT_SUCCESS;
}

void printFlag(bool *flag)
{
    for (int c = 0; c < 20; c++)
    {
        std::cout << std::boolalpha << *flag << std::endl;
        sleep(1);
    }
}

int main(void)
{
    /*
    bool flag = false;
    std::thread t(printFlag, &flag);
    
    for (int c = 0; c < 5; c++)
    {
        flag = !flag;
        sleep(1);
    }
    t.join();
    return EXIT_SUCCESS;
    */
    
    return sendPayload(100);
}
