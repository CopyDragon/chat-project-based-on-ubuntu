server:
	g++ -o server server.cpp
	g++ -o client client.cpp
clean:
	rm server
	rm client
