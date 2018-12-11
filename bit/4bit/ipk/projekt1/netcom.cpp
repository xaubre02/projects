/**
 * Client-Server app using BSD sockets
 * @author Tomas Aubrecht (xaubre02)
 * @version 1.0 2017-03-01
 */

#include "netcom.h"

// array of http codes
const char *StateCode[3] =
{
	"200 OK",
	"400 Bad Request",
	"404 Not Found"
};

// array of error messages
const char* OperationErr[8] =
{
	"Not a directory.",
	"Directory not found.",
	"Directory not empty.",
	"Already exists.",
	"Not a file.",
	"File not found.",
	"Unknown error.",
	"User Account Not Found."
};

/*
 * ------------ NETCOM FUNCTIONS DEFINITIONS ------------
 */
NetCom::NetCom(void)
{
	// init values and zeroes buffers
	size = sizeof(sa_server);
	bzero(msg_buff, MSG_BUFF);
	bzero((char *)&sa_server, size);
}

NetCom::~NetCom(void)
{
	close(ClientSocket);
}

void NetCom::recvMsg(void)
{
	// zero buffer before receive
	bzero(msg_buff, MSG_BUFF);
	result = recv(ClientSocket, msg_buff, (MSG_BUFF - 1), 0);
	if (result < 0) exitError();
}

void NetCom::sendMsg(const char *msg)
{
	result = send(ClientSocket, msg, strlen(msg), 0);
	if (result < 0) exitError();
}

void NetCom::sendFile(const char *path, long length)
{
	// open file
	ifstream file;
	file.open(path, ios::binary);

	long remaining = length;
	while (remaining > 0)
	{
		if (remaining >= FILE_BUFF)
		{
			file.read(file_buff, FILE_BUFF);
			// send it
			result = send(ClientSocket, file_buff, FILE_BUFF, 0);
			if (result < 0) exitError();
		}
		else
		{
			bzero(file_buff, FILE_BUFF);
			file.read(file_buff, remaining);
			// send it
			result = send(ClientSocket, file_buff, remaining, 0);
			if (result < 0) exitError();
		}

		remaining -= result;
	}

	file.close();
}

void NetCom::recvFile(const char *path, long length)
{
	ofstream file;
	file.open(path, ios::binary);

	long remaining = length;
	while (remaining > 0)
	{
		// receive the file
		if (remaining >= FILE_BUFF)
		{	
			result = recv(ClientSocket, file_buff, FILE_BUFF, 0);
			if (result < 0) exitError();
			// write file and free memory
			file.write(file_buff, FILE_BUFF);
		}
		else
		{
			bzero(file_buff, FILE_BUFF);
			result = recv(ClientSocket, file_buff, remaining, 0);
			if (result < 0) exitError();
			// write file and free memory
			file.write(file_buff, remaining);
		}

		remaining -= result;
	}
	
	file.close();
}

long NetCom::getFileSize(const char *path)
{
	// open file
	ifstream mySource;
	mySource.open(path, ios_base::binary |  ios_base::ate);
	// open was successful
	if(mySource.good())
	{	
		if(checkIfIsDir(path))
			argsError("Not a file!", true);
	}
	else
		argsError("Opening the file!");	
	// save the size of file
	unsigned long size = mySource.tellg();
	mySource.close();

	return size;
}

void NetCom::convertPort(const char* arg)
{
	char * pEnd = NULL;
	port = strtol(arg, &pEnd, 10);

	if(*pEnd == '\0') return;
	// else -> invalid port number
	argsError("Invalid port number!");
}

bool NetCom::checkIfIsDir(string path)
{
	// if null string -> error
	if (path.empty())
		return false;

	struct stat sb;
	// check if is dir
	if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		return true;

	return false;
}

void NetCom::getRecvSize(string msg)
{
	// get the length from a message
	size_t pos = msg.find("Content-Length: ") + 16;
	size_t len = msg.length() - pos;
	length = strtol(msg.substr(pos, len).c_str(), NULL, 10);
}

string NetCom::getActualTime(void) const
{
	time_t t = time(NULL);
	struct tm *now = localtime(&t);

	char buffer[32];
	// save time in the same format as it is in the REST
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %T %Z", now);

	return buffer;
}

void NetCom::exitError(void) const
{
	// print msg from errno
	cerr << "Error: " << strerror(errno) << endl;
	exit(EXIT_FAILURE); 
}

void NetCom::argsError(const char *msg, bool def) const
{
	// print my own error msg
	if (!def) cerr << "Error: ";

	cerr << msg << endl;
	exit(EXIT_FAILURE);
}

/*
 * ------------ CLIENT FUNCTIONS DEFINITIONS ------------
 */
Client::Client(int argc, char* argv[])
{
	// initialize values
	transfer = false;
	http.length = 0;
	http.file = ("");
	// process arguments
	parseArguments(argc, argv);
}

Client::~Client(void)
{

}

void Client::parseArguments(int argc, char* argv[])
{
	if(argc == 3 || argc == 4)
	{
		// command is 1. argument
		http.cmd = argv[1];
		// the http line
		string line = makeSpaces(argv[2]);
		// if local path is specified
		if (argc == 4) http.file = makeSpaces(argv[3]);

		// check the prefix of http
		if(line.substr(0,7).compare("http://"))
			argsError("Invalid http!");

		// parse http line
		size_t s_pos = 7;
		size_t e_pos = line.find(":", s_pos);
		// if : is not found -> port number is not specified, use default one (6677)
		if (e_pos == string::npos) 
		{	
			port = 6677;
			// find next slash
			e_pos = line.find("/", s_pos);
			if (e_pos == string::npos) 
				argsError("User Account Not Found.", true);
			// save server name
			http.server = line.substr(s_pos, e_pos - s_pos);
		}
		else
		{
			// save server name
			http.server = line.substr(s_pos, e_pos - s_pos);

			s_pos = e_pos + 1;
			e_pos = line.find("/", s_pos);
			if (e_pos == string::npos) 
				argsError("User Account Not Found.", true);
			// convert string to number and save it to port
			convertPort(line.substr(s_pos, e_pos - s_pos).c_str());
		}

		s_pos = e_pos + 1;
		e_pos = line.find("/", s_pos);
		// check whether REMOTE-PATH is specified
		if (e_pos == string::npos) 
			argsError("No REMOTE-PATH specified.");

		s_pos--;
		// save the path
		http.path = line.substr(s_pos, line.length() - s_pos);

// **************** COMPARE inserted command with supported commands ****************
		if		(!http.cmd.compare("del") && argc == 3) 
		{
			http.cmd = "DELETE";
			http.type = "file";
			return;
		}
		else if (!http.cmd.compare("rmd") && argc == 3)
		{
			http.cmd = "DELETE";
			http.type = "folder";
			return;
		}
		else if (!http.cmd.compare("lst") && argc == 3)
		{
			http.cmd = "GET";
			http.type = "folder";
			return;
		}
		else if (!http.cmd.compare("mkd") && argc == 3)
		{
			http.cmd = "PUT";
			http.type = "folder";
			return;
		}
		else if (!http.cmd.compare("get"))
		{
			// gather the file name
			size_t file_pos1;
			size_t file_pos2 = 0;
			while (true)
			{
				file_pos1 = file_pos2;
				file_pos2 = http.path.find("/", file_pos1 + 1);
				if(file_pos2 == string::npos)
				{
					// file name is after the last slash
					http.fileName = http.path.substr(file_pos1 + 1, http.path.length() - file_pos1 + 1);
					break;
				}
			}

			if (argc == 4)
			{
				// if local path is specified
				http.fileName = http.file;
			}

			if( checkFile(http.fileName.c_str()))
			{
				http.cmd = "GET";
				http.type = "file";
				return;	
			}
			argsError("Already exists.", true);
		}
		else if (!http.cmd.compare("put"))
		{
			if(argc == 4) 
			{
				http.cmd = "PUT";
				http.type = "file";
				http.length = getFileSize(http.file.c_str()); // get file size
				return;
			}
			argsError("Local path is not specified!"); // put must have local path
		}
		if (argc == 3)
			argsError("Invalid command!");
	}
	argsError("Bad arguments!");
}

bool Client::checkFile(const char *path)
{
	// check whether it is a directory, if yes -> error
	if (checkIfIsDir(path)) return false;

	fstream file;
	file.open(path, fstream::in | fstream::out);
	
	// if file does not exist -> OK
	if(file.fail()) return true;

	return false;
}

string Client::makeSpaces(string text) const
{
	// transform http spaces (%20) into spaces
	size_t pos = text.find("%20");
	// while there is a %20, transform it
	while (pos != string::npos)
	{
		text.replace(pos, 3, " ");
		pos = text.find("%20");
	}

	return text;
}

string Client::createMessage(void)
{
	// convert number to string
	stringstream number;
	number << http.length;
	// construct http header
	string msg(http.cmd);
	msg.append(" " + http.path + "?type=" + http.type + " HTTP/1.1");
	msg.append("\nHost: " + http.server);
	msg.append("\nDate: " + getActualTime());
	msg.append("\nAccept: text/plain");
	msg.append("\nAccept-Encoding: identity");
	msg.append("\nContent-Type: text/plain");
	msg.append("\nContent-Length: " + number.str());

	return msg;
}

void Client::decodeMsg(void)
{
	string msg(msg_buff);
	if(!msg.substr(9,3).compare("200"))
	{
		// everything is ok and ready to proceed
		transfer = true;
		getRecvSize(msg);
		if (!http.cmd.compare("GET") && !http.type.compare("folder"))
		{
			// find and print the content of a directory, if there is any
			size_t pos = msg.find("Content-Encoding: identity") + 27;
			if((pos - 1) == msg.length())
				return;
			cout << msg.substr(pos, msg.length() - pos) << endl;
		}	
		return;
	}

	else
	{
		// something went wrong, print error and exit with failure
		transfer = false;
		size_t pos = msg.find("Content-Encoding: identity") + 27;
		cerr << msg.substr(pos, msg.length() - pos) << endl;
		exit(EXIT_FAILURE);
	}
}

void Client::seekAndConnect(void)
{
	// structure, that keeps info about server address
	struct hostent *ServerName;
	// create socket
	ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSocket <= 0) 
		exitError();
	// get server address
	ServerName = gethostbyname(http.server.c_str());
	if(ServerName == NULL)
	{
		close(ClientSocket);
		cerr << "Error: No such host with this name!" << endl;
		exit(EXIT_FAILURE); 
	}
	// fill the server structure
	sa_server.sin_family = AF_INET;
	bcopy((char *)ServerName->h_addr, (char *)&sa_server.sin_addr.s_addr, ServerName->h_length);
	sa_server.sin_port = htons(port);
	// connect to the server
	result = connect(ClientSocket, (struct sockaddr *)&sa_server, size);
	if (result != 0)
	{	
		close(ClientSocket);
		exitError();
	}
}

void Client::sendRequest(void)
{	
	// send request to server
	sendMsg(createMessage().c_str());
	if (!http.cmd.compare("PUT") && !http.type.compare("file"))
	{	
		// get answer from server
		recvMsg();
		decodeMsg();
		// if the server is ready and ok to receive the file, transfer it
		if(transfer) sendFile(http.file.c_str(), http.length);
	}
	else if (!http.cmd.compare("GET") && !http.type.compare("file"))
	{
		// get answer from the server
		recvMsg();
		decodeMsg();
		// if the server is ready and ok to send the file, transfer it
		if(transfer) recvFile(http.fileName.c_str(), length);
	}
}

void Client::getAnswer(void)
{
	// in those cases, answer is processed elsewhere
	if (!http.cmd.compare("PUT") && !http.type.compare("file")) return;
	if (!http.cmd.compare("GET") && !http.type.compare("file")) return;

	recvMsg(); // get server response
	decodeMsg(); // analyze it
}

/*
 * ------------ SERVER FUNCTIONS DEFINITIONS ------------
 */
Server::Server(int argc, char* argv[])
{
	// initialize values
	dir = false;
	answer = true;
	port = 6677; // default port
	length = 0;
	root = ""; // default root
	parseArguments(argc, argv); // process arguments
	bzero((char *)&sa_client, size); // clear buffer
}

Server::~Server(void)
{
	close(ListenSocket);
}

void Server::parseArguments(int argc, char* argv[])
{
	if (argc == 1) return; // OK
	// root folder or port number must be entered, otherwise exit with error
	else if (argc == 3)
	{	
		if (!strcmp(argv[1], "-r"))
		{
			// check if valid root is entered
			if(checkIfIsDir(argv[2]))
			{	
				root = argv[2];
				return;
			}
			argsError("Local path does not exists!");
		}
		else if (!strcmp(argv[1], "-p"))
		{
			// convert string to integer
			convertPort(argv[2]);
			return;
		}
	}
	else if (argc == 5)
	{
		if (!strcmp(argv[1], "-r"))
		{
			if (!strcmp(argv[3], "-p"))
			{
				// check if valid root is entered
				if(checkIfIsDir(argv[2]))
				{	
					root = argv[2];
					// convert string to integer
					convertPort(argv[4]);
					return;
				}	
				argsError("Local path does not exists!");
			}
		}
		else if (!strcmp(argv[1], "-p"))
		{
			if (!strcmp(argv[3], "-r"))
			{
				// check if valid root is entered
				if(checkIfIsDir(argv[4]))
				{	
					root = argv[4];
					// convert string to integer
					convertPort(argv[2]);
					return;
				}
				argsError("Local path does not exists!");
			}
		}
	}

	argsError("Bad arguments!");
}

int Server::decodeMsg(void)
{
	string msg = msg_buff;
	// find type of data -> FILE/FOLDER
	size_t pos = msg.find("?type=") + 6;
	size_t len = msg.find(" HTTP/1.1") - pos;
	size_t space = msg.find(" ") + 2;
	// save that type
	string type = msg.substr(pos, len);

	pos = msg.find("/", space);
	len = msg.find("?type=") - pos;
	// save user
	root.append("/");
	string user = root;
	user.append(msg.substr(space, pos - space));
	// check if user does exist
	if (!checkUser(user.c_str())) return 99;
	// save path
	string path = root;
	path.append(msg.substr(space, pos - space + len));
	space -= 2;

	// user can not manipulate with parent directory
	if (path.find("..") != string::npos) return 42;
	// ********** COMPARING command with supported commands *********
	if (!msg.substr(0, space).compare("GET")) 
	{	
		if (!type.compare("file"))
		{
			//get
			dir = false;
			if (checkFile(path.c_str(), true))
			{
				answer = false;
				// get the size of file
				length = getFileSize(path.c_str());
				// send msg with file size
				sendMsg(createMessage(0).c_str());
				// send file
				sendFile(path.c_str(), length);
				return 0;
			}
			return errno;
		}
		else if (!type.compare("folder"))
		{
			// lst
			dir = true;
			if(listDir(path.c_str()))
			{
				length = content.length();
				return 0;
			}

			else return errno;
		}
		else return 42;
	}
	else if (!msg.substr(0, space).compare("PUT"))
	{
		// put
		if (!type.compare("file"))
		{
			dir = true;
			size_t tmp = path.find("/");
			size_t pos;
			// check if the directory where the file will be placed exists
			while (tmp != string::npos)
			{
				pos = tmp;
				tmp = path.find("/", tmp + 1);
			}
			if(checkIfIsDir(path.substr(0, pos).c_str()))
			{
				dir = false;
				if (checkFile(path.c_str()))
				{
					// server will not send another answer
					answer = false;
					// send msg indicating that server is ready to receive file
					sendMsg(createMessage(0).c_str());
					getRecvSize(msg);
					recvFile(path.c_str(), length);
					return 0;
				}
				errno = 17;
			}
			return errno;
		}
		// mkdir
		else if (!type.compare("folder"))
		{
			dir = true;
			if(makeDir(path.c_str()))
				return 0;

			else return errno;
		}
		else return 42;
	}
	else if (!msg.substr(0, space).compare("DELETE")) 
	{
		// del
		if (!type.compare("file"))
		{
			dir = false;
			if(deleteFile(path.c_str()))
				return 0;

			else return errno;
		}
		// rmd
		else if (!type.compare("folder"))
		{
			// user can not remove his own folder or actual, which is same
			if (!user.compare(path) ||
				!user.compare(path.substr(0, path.length() - 1))) return 42;

			dir = true;
			if(removeDir(path.c_str()))
				return 0;

			else return errno;
		}
		else return 42;
	}
	return 0;
}

bool Server::checkFile(const char *path, bool get)
{
	if (checkIfIsDir(path))
	{
		// it is a directory, not a file
		if(get)
		{
			errno = 21;
			return false;
		}

		return false;
	}

	// try to open the file
	fstream file;
	file.open(path, fstream::in | fstream::out);
	
	// if this fails, we can't open the file (e.g. file does not exist or permission denied)
	if(file.fail())
	{
		// if it does not exist, we can not get it -> error
		if (get)
		{
			errno = 2;
			return false;
		}
		// it does not exist -> we can create new
		errno = '\0';
		return true;
	}

	if (get) return true;  // file is OK, we can send it
	else     return false; // file is no OK, we can not receive it
}

bool Server::checkUser(const char *path)
{
	if (checkIfIsDir(path)) return true;

	return false;
}

string Server::createMessage(int code)
{
	if (code == 42) errno = 42; // unknown error
	if (code == 99) errno = 99; // user does not exist
	if (code != 0 && code != 2) code = 1; // setting http code

	// if any error occured
	string error("");
	if (errno != 0 && code != 0)
	{
		error = processError();
		length = error.length();
	}

	// convert integer to string
	stringstream number;
	number << length;

	// construct http header
	string msg ("HTTP/1.1 ");
	msg.append(StateCode[code]);
	msg.append("\nDate: " + getActualTime());
	msg.append("\nContent-Type: text/plain");
	msg.append("\nContent-Length: " + number.str());
	msg.append("\nContent-Encoding: identity");
	msg.append(error);
	// if there is additional content, append it
	if (!content.empty()) msg.append("\n" + content);

	return msg;
}

string Server::processError(void) const
{
	// error will be printed on a new line
	string error("\n");
	switch(errno)
	{
		case  2:
			if(dir) // it is a directory
				error.append(OperationErr[1]); // "Directory not found."
			else // it is a file
				error.append(OperationErr[5]); // "File not found."
			break;

		case 17: error.append(OperationErr[3]); break; // "Already exists."
		case 20: error.append(OperationErr[0]); break; // "Not a directory."
		case 21: error.append(OperationErr[4]); break; // "Not a file."
		case 39: error.append(OperationErr[2]); break; // "Directory not empty."
		case 99: error.append(OperationErr[7]); break; // "User Account Not Found."
		default: error.append(OperationErr[6]);        // "Unknown error."
	}

	return error;
}

void Server::run(void)
{
	// fill server structure
	sa_server.sin_family = AF_INET;
	sa_server.sin_addr.s_addr = INADDR_ANY;
	sa_server.sin_port = htons(port);
	// create socket
	ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket <= 0) 
		exitError();
	// bind socket
	result = bind(ListenSocket, (const struct sockaddr *)&sa_server, size);
	if (result < 0) 
	{
		close(ListenSocket);
		exitError();
	}
	// listen
	result = listen(ListenSocket, QUEUE_MAX);
	if (result < 0) 
	{	
		close(ListenSocket);
		exitError();
	}
}

void Server::acceptNew(void)
{
	//reset values
	errno = '\0';
	answer = true;
	content.clear();
	// accept new client
	ClientSocket = accept(ListenSocket, (struct sockaddr *)&sa_client, &size);
	if (ClientSocket <= 0)
	{	
		close(ListenSocket);
		exitError();
	}
}

void Server::processIt(void)
{
	recvMsg();
	int code = decodeMsg();
	if (answer)
	sendMsg(createMessage(code).c_str());
}

bool Server::listDir(const char *path)
{
	if (checkIfIsDir(path))
	{
		DIR *dir;
		struct dirent *s_dir;
		// try to open directory
		if ((dir = opendir (path)) != NULL)
		{
			vector<string> list;
    		list.clear();
    		// if there is something to read, read it
			while ((s_dir = readdir (dir)) != NULL)
			{
				// save everything except parent and actual directory
				if (!strcmp(s_dir->d_name, ".") || !strcmp(s_dir->d_name, "..") || !strcmp(s_dir->d_name, ".DS_Store"))
					continue;
				// add to list
				list.push_back(s_dir->d_name);
			}
			closedir (dir);
			// sort items in a list
			sort(list.begin(), list.end());
			//save it to string
			for(unsigned int C = 0; C < list.size(); C++)
				content.append(list[C] + " ");

		}
		return true;
	}
	else
	{
		// it's not directory, but file
		if(errno == '\0') errno = 20;
	}

	return false;
}

bool Server::makeDir(const char *path)
{
	// if function fails, errno is set
	int result = mkdir(path, S_IRWXU | S_IRGRP |  S_IXGRP | S_IROTH | S_IXOTH);
	if(result == -1) 
		return false;

	return true;
}

bool Server::removeDir(const char *path)
{
	// if function fails, errno is set
	int result = rmdir(path);
	if(result == -1) 
		return false;

	return true;
}

bool Server::deleteFile(const char *path)
{
	// if function fails, errno is set
	int result = unlink(path);
	if(result == -1) 
		return false;

	return true;
}
