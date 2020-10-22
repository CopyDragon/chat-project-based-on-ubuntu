target:
	g++ -o server server.cpp HandleServer.cpp HandleClient.cpp global.cpp  -lmysqlclient -lpthread
	g++ -o client client.cpp HandleClient.cpp -lpthread
clean:
	rm server
	rm client
