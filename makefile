server:
	g++ -o server server.cpp HandleServer.cpp HandleServer.h -lmysqlclient -lpthread
	g++ -o client client.cpp
clean:
	rm server
	rm client
