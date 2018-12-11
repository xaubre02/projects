/**
 * Client-Server app using BSD sockets
 * @author Tomas Aubrecht (xaubre02)
 * @version 1.0 2017-03-01
 */

#ifndef _NETCOM_H_
#define _NETCOM_H_

#include <iostream>
#include <string.h>
#include <dirent.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <errno.h>
#include <vector>
#include <ctime>
#include <netdb.h>
#include <unistd.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MSG_BUFF 512
#define FILE_BUFF 16384
#define QUEUE_MAX 10

using namespace std;

/**
 * Struct 
 */
struct HTTP_t
{
	long length;
	string cmd;
	string server;
	string path;
	string type;
	string file;
	string fileName;
};

/********************************************
 * *** Parent class for Client and Server ***
 */
class NetCom
{
public:

	NetCom(void);
	~NetCom(void);

protected:

	// used for keeping port number
	int port;
	// used for checking if a an operation was successful
	int result;
	// socket for connecting to a server or accepting a client
	int ClientSocket;
	// used for holding length of the message
	long length;
	// holds the total size of the struct sockaddr_in
	socklen_t size;
	// struct for holding information about sever
	struct sockaddr_in sa_server;
	// buffer which keeps message to be sent
	char msg_buff[MSG_BUFF];
	// buffer which keeps message to be sent
	char file_buff[FILE_BUFF];
	/**
	 * Receive data and store them in a msg_buff.
	 */
	void recvMsg(void);

	/**
	 * Send data.
	 * @param msg Message to be sent
	 */
	void sendMsg(const char *msg);
	
	/**
	 * Send a file.
	 * @param path Path where the file is located
	 * @param length Total length of the file in bytes
	 */
	void sendFile(const char *path, long length);
	
	/**
	 * Receive a file.
	 * @param path Path where the file will be saved
	 * @param length Total length of the file in bytes
	 */
	void recvFile(const char *path, long length);

	/**
	 * Get size of a file
	 * @param path Path where the file is located
	 * @return Total length of the file in bytes 
	 */
	long getFileSize(const char *path);
	
	/**
	 * Convert string into integer
	 * If no error occurs, save the value to port
	 * @param arg Argument part to be converted
	 */
	void convertPort(const char* arg);
	
	/**
	 * Process the entry arguments and check if any error occurs
	 * @param argc Number of arguments
	 * @param argv Array of arguments
	 */
	void parseArguments(int argc, char* argv[]);
	
	/**
	 * Check if path is a directory.
	 * @param path Path to directory to be checked
	 * @return True if path is dir, otherwise false
	 */
	bool checkIfIsDir(string path);

	/**
	 * Get the content lenght
	 * @param msg Whole message where the length will be searched
	 */
	void getRecvSize(string msg);
	
	/**
	 * Get actual time
	 * @return String containing actual time
	 */
	string getActualTime(void) const;

	/**
	 * Print a message got by errno to stderr and exit program with error code -1
	 */
	void exitError(void) const;
	
	/**
	 * Print a message to stderr and exit program with error code -1
	 * @param msg Message to by written
	 * @param def Bool for choosing the style of message to be written
	 */
	void argsError(const char *msg, bool def = false) const;
};


/********************************************
 * ************** Client class **************
 */
class Client : NetCom
{
public:

	Client(int argc, char* argv[]);
	~Client(void);
	
	/**
	 * Find a server with provided address and port and connect to itn
	 */
	void seekAndConnect(void);
	
	/**
	 * Send initial message containing request for the server
	 */
	void sendRequest(void);
	
	/**
	 * Receive a massage containing information about requsted operation
	 */
	void getAnswer(void);

private:

	// if set, file is ready to be sent, otherwise an error has occured
	bool transfer;
	// struct holding information from parsed arguments
	struct HTTP_t http; 

	/**
	 * Check if a provided path is a file or not.
	 * @param path Path to be checked
	 * @return True if path is a file, otherwise false is returned
	 */
	bool checkFile(const char *path);

	/**
	 * Process the entry arguments and check if any error occurs
	 * @param argc Number of arguments
	 * @param argv Array of arguments
	 */
	void parseArguments(int argc, char* argv[]);

	/**
	 * Decode answer from the server.
	 */
	void decodeMsg(void);

	/**
	 * Convert %20 into spaces
	 * @param text Content where the spaces should be created
	 * @return String with spaces
	 */
	string makeSpaces(string text) const;

	/**
	 * Create a RESTful API message 
	 * @return Message ready to be sent
	 */
	string createMessage(void);
};

/********************************************
 * ************** Server class **************
 */
class Server : NetCom
{
public:

	Server(int argc, char* argv[]);
	~Server(void);

	/**
	 * Initialize and launch the server.
	 */
	void run(void);
	
	/**
	 * Accept a new client.
	 */
	void acceptNew(void);
	
	/**
	 * Accept a request from client and try to realize it
	 */
	void processIt(void);

private:

	// indicates whether it is folder or directory
	bool dir;
	// indicates whether a message should be sent or not
	bool answer;
	// contains root directory from argument
	string root;
	// additional information in message - error msg or directory content
	string content;
	// socket used for listening for new connections
	int ListenSocket;
	// structure containing information about client
	struct sockaddr_in sa_client;

	/**
	 * Process the entry arguments and check if any error occurs
	 * @param argc Number of arguments
	 * @param argv Array of arguments
	 */
	void parseArguments(int argc, char* argv[]);

	/**
	 * Decode request from the client.
	 * @return Error value, if OK, 0 is returned
	 */
	int decodeMsg(void);

	/**
	 * Check whether a file exists.
	 * @param path Path where the file is located
	 * @param get Differ wheter it is put or get operation
	 * @return True if everything is OK, otherwise false is returnded
	 */
	bool checkFile(const char *path, bool get = false);
	
	/**
	 * Check whether a user exists.
	 * @param path Name of the user
	 * @return True if user exists, otherwise false is returnded
	 */
	bool checkUser(const char *path);

	/**
	 * Create a RESTful API message 
	 * @param code Indicates which return code should be sent
	 * @return Message ready to be sent
	 */
	string createMessage(int code);

	/**
	 * Select which message should be sent when an error occurs
	 * @return Error message
	 */
	string processError(void) const;

	/**
	 * Show content of a directory.
	 * @param path Target path where the operation should be realized
	 * @return True if operation was successful, otherwise false is returned
	 */
	bool listDir(const char *path);
	
	/**
	 * Create a new directory.
	 * @param path Target path where the operation should be realized
	 * @return True if operation was successful, otherwise false is returned
	 */
	bool makeDir(const char *path);
	
	/**
	 * Remove a directory.
	 * @param path Target path where the operation should be realized
	 * @return True if operation was successful, otherwise false is returned
	 */
	bool removeDir(const char *path);
	
	/**
	 * Delete a file.
	 * @param path Target path where the operation should be realized
	 * @return True if operation was successful, otherwise false is returned
	 */
	bool deleteFile(const char *path);
};

#endif