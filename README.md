# Parellel-Computing-Hash-Decrypter
Y Pushyarag | Vishal Babu Bhavani
140050047 | 140050049

## List of relevant files
`makefile` - makefile of the project
###Source Files
`server.cpp` - contains code of the server <br>
`user.cpp` - contains code of the user <br>
`worker.cpp` - contains code of the worker <br>
###Efficiency plots
`results.jpg` - required image graph of evaluation <br>

## Running instructions

```
$ make
```
The above generates the executables  `server`, `user` and `worker` 
for server, user and worker respectively.
```
$ make clean
```
The above cleans the directory and returns it back to the 
original state,

```
$ ./server <port-no>
```
The above starts a server which listens on the specified port number.

```
$ ./worker <server-ip> <port-no>
```
The above starts a worker and connects to a hash-breaker-server listening on specified ip and port number.

```
$ ./user <server ip/host-name> <server-port> <hash> <passwd-length> <binary-string>
```
The above starts a user with instructions as specified in the problem which requests the server to crack the hash.
statement.

Use `crypt("<string>", "<salt>")` to get the hash, <passwd-length> is the length of
the password string. <binary-string> is a 3 bit binary string, where the three bits 
indicate the use of lower case, upper case and numerical characters in the password. 
For example, 001 indicates the password is made up of only numbers; 111 indicates 
the password can contain lower, upper as well as numerical characters.
