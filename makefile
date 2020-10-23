target: HandleClient.o server.cpp HandleServer.o client.cpp global.cpp
	#g++ -o server server.cpp HandleServer.cpp HandleClient.o global.cpp  -lmysqlclient -lpthread
	g++ -o server server.cpp HandleServer.o global.cpp -lmysqlclient -lpthread
	g++ -o client client.cpp HandleClient.o -lpthread

HandleServer.o: HandleServer.cpp HandleClient.o global.cpp
	g++ -c HandleServer.cpp HandleClient.o global.cpp -lmysqlclient -lpthread


HandleClient.o:	HandleClient.cpp
	g++ -c HandleClient.cpp

clean:
	rm server
	rm client
	rm *.o
