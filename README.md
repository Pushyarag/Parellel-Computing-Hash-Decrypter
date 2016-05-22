# Parellel-Computing-Hash-Decrypter
Y Pushyarag | Vishal Babu Bhavani <br>
140050047 | 140050049

## List of relevant files
`makefile` - makefile of the project
###Source Files
`server.cpp` - contains code of the server <br>
`user.cpp` - contains code of the user <br>
`worker.cpp` - contains code of the worker <br>
###Efficiency plots
`results.jpg` - required image graph of evaluation <br>
##Efficiency
Time taken to crack the hash depends depends on :- <br>

 * number of workers cracking the password
 * length of the original password
 * type of password, like numerical, or alpha-numeric etc.

More the workers, faster is the decryption. It is faster for numerical than for alphabetical than for alpha-numeric.<br>
The server divides the work into small parts and allots each worker with one part, it allots next remaining part to a worker after the worker completes the previous part.The server ensures always optimal way of dividing the work among the workers, considering new workers enter and leave(leave only after completing its alloted part). 
The variation of time taken with number of workers for different lengths of passwords is shown in results.jpg

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

The above starts a server which listens on the specified port number.<br>
The server accepts requests from user and then returns them with decrypted password and time taken to crack it. 

```
$ ./worker <server-ip> <port-no>
```

The above starts a worker and connects to a hash-breaker-server listening on specified ip and port number.

```
$ ./user <server ip/host-name> <server-port> <hash> <passwd-length> <binary-string>
```

The above starts a user with instructions as specified in the problem which requests the server to crack the hash.


Use `crypt("<string>", "<salt>")` to get the hash, <passwd-length> is the length of
the password string. <binary-string> is a 3 bit binary string, where the three bits 
indicate the use of lower case, upper case and numerical characters in the password. 
For example, 001 indicates the password is made up of only numbers; 111 indicates 
the password can contain lower, upper as well as numerical characters.
