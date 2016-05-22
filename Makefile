.PHONY: all clean


all : user server worker 
user:
	@echo "Creating user executable ......."
	@g++ src/user.cpp -o user
server:
	@echo "Creating server executable ......."
	@g++ src/server.cpp -o server
worker:
	@echo "Creating worker executable ......."
	@g++ src/worker.cpp -o worker -std=c++11 -lpthread -lcrypt
clean:
	@echo "cleaning up ......."
	@rm -f user
	@rm -f server
	@rm -f worker
