server:
	g++ -o server server.cpp HandleServer.cpp -l mysqlclient
	g++ -o client client.cpp
clean:
	rm server
	rm client
