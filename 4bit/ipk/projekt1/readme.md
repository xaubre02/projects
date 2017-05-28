.\" Process this file with
.\" groff -man -Tascii foo.1
.\"
.TH IPK 1 "MARCH 2017" Linux "Project Manual"

.SH NAME
ftrest \- a client application

.SH SYNOPSIS
.B ftrest 
.I command remote-path 
[
.I local-path
]

.SH DESCRIPTION
Ftrest is a client based application using BSD sockets and HTTP RESTful API for communication.

Its lifetime begins with parsing of arguments and their evaluation. If everything is all right, it tries to connect to provided server until the connection is established, otherwise ftrest exits with error. Then it sends a request to the server and waits for a response. If the request was successfully fulfilled, ftrest exits with return code 0, otherwise -1 is returned.

.B See source code for more details.

.SH OPTIONS
.IP command
Command to be executed. (see next section)

.IP remote-path
Path to file or directory on the server.

.IP local-path
Path in the local file system.

.SH COMMANDS
.B del
.RS
Delete file on remote server. File is determined by 
.I remote-path.
.RE

.B get
.RS
Copy file from 
.I remote-path
to local directory. Local directory could be specified with
.I local-path.
.RE

.B put
.RS
Copy file from
.I local-path
to
.I remote-path
on the server.
.RE

.B lst
.RS
List the content of the remote directory to standard output.
.RE

.B mkd
.RS
Create a directory specified by
.I remote-path
on the server.
.RE

.B rmd
.RS
Remove a directory specified by 
.I remote-path
from the server.

.SH DIAGNOSTICS
The program may end up with an error written to stderr:

.B Error: Bad arguments!
.RS
When an invalid argument is present or invalid number/combination of arguments.
.RE

.B Error: Invalid command!
.RS
When not supported command is specified.
.RE

.B Error: Local path is not specified!
.RS
When a command
.B put
is specified without
.I local-path.
.RE

.B Error: Invalid http!
.RS
When a second argument is an invalid.
.RE

.B Error: No REMOTE-PATH specified.
.RS
When a second argument does not contain
.I remote-path.
.RE

.B Error: No such host with this name!
.RS
When a client is unable to identify provided host.
.RE

.B Error: Invalid port number!
.RS
When the specified 
.I port
is invalid.
.RE

.RE
.B Not a directory.
.RS
When 
.I remote-path
is pointing on a file while using
.B lst
or
.B rmd.
.RE

.B Directory not found.
.RS
When 
.I remote-path
is not pointing on any existing object while using
.B lst
or
.B rmd.
.RE

.B Directory not empty.
.RS
When 
.I remote-path
is pointing on a directory, which is not empty, while using
.B rmd.
.RE

.B Already exists.
.RS
When 
.I remote-path
is pointing on a file/directory, which already exist, while using
.B mkd
or
.B put.
.RE

.B Not a file.
.RS
When 
.I remote-path
is pointing on a directory while using 
.B del
or
.B get.
.RE

.B File not found.
.RS
When 
.I remote-path
is not pointing on any existing object while using
.B del
or
.B get.
.RE

.B User Account Not Found.
.RS
When the user does not exist.
.RE

.B  Unknown error.
.RS
All other errors.
.RE

.SH AUTHOR
Tomas Aubrecht (xaubre02)

.SH NAME
ftrestd \- a server application

.SH SYNOPSIS
.B ftrestd
[
.B -r
.I root-folder
] [
.B -p 
.I port
]

.SH DESCRIPTION
Ftrestd is a server based application using BSD sockets and HTTP RESTful API for communication.

Its lifetime begins with parsing of arguments and their evaluation. If everything is all right, it creates a listen socket, which is waiting for incoming connections, otherwise it exists with failure and error message. Then it runs in a loop, where it accepts connections and fulfil their requests. Once a new connection is established, server checks whether the user, who is connecting, exists. If he doesn't server sends error message to client and disconnects him. Then server can wait for another connections. If the user exist, server decodes operation, which the user is demanding, and tries to perform it. If the operation was successfully completed, server sends message with confirmation and relevant code, otherwise with error. Now server can repeat the whole process.

.B See source code for more details.

.SH OPTIONS
.IP "-r root-folder"
Root directory where the files will be stored for each user. ( Default directory is current )
.IP "-p port"
Port on which the server will listen. ( Implicitly 
.B 6677
)

.SH DIAGNOSTICS
The program may end up with an error written to stderr:

.B Error: Bad arguments!
.RS
When an invalid argument is present or invalid number/combination of arguments.
.RE

.B Error: Invalid port number!
.RS
When the specified 
.I port
is invalid.
.RE

.B Error: Local path does not exists!
.RS
When
.I roor-folder
is an invalid directory.
.RE

.SH AUTHOR
Tomas Aubrecht (xaubre02)