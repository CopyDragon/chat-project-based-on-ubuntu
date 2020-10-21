server:
	g++ -o server global.cpp HandleClient.h server.cpp HandleServer.cpp HandleServer.h HandleClient.cpp   -lmysqlclient -lpthread
	g++ -o client client.cpp HandleClient.cpp -lpthread
clean:
	rm server
	rm client
