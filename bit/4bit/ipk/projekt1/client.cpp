/**
 * Client app using BSD sockets
 * @author Tomas Aubrecht (xaubre02)
 * @version 1.0 2017-03-01
 */

#include "netcom.h"
 
/**
 * Main function controlling operation of the program.
 */
int main(int argc, char* argv[])
{
	Client client(argc, argv);
	client.seekAndConnect();

	client.sendRequest();
	client.getAnswer();
	
	return EXIT_SUCCESS;
}