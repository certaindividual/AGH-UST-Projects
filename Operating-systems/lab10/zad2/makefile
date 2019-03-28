CC = gcc -Wall -std=c11 -ggdb


compile:
	$(CC) -o server server.c -lpthread -lm
	$(CC) -o client client.c -lpthread -lm
	
	
clean:

	rm -f server client  *.o *.out	

serv:
	./server 8000 "serv"

clients:
	./client clientMarek "WEB" "127.0.0.1:8000" &
	./client clientWojtek "WEB" "127.0.0.1:8000" &
	./client clientBrajanek "LOCAL" "serv" &
	./client clientJanusz "LOCAL" "serv" &


all: clean compile test



