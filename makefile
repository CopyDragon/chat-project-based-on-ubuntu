target: serverUseThreadPool.cpp HandleClient.o server.cpp HandleServer.o client.cpp global.cpp HandleServerUseThreadPool.o
	#g++ -o server server.cpp HandleServer.cpp HandleClient.o global.cpp  -lmysqlclient -lpthread
	g++ -g -o serverUseThreadPool serverUseThreadPool.cpp HandleServerUseThreadPool.o global.cpp -lmysqlclient -lpthread -lhiredis
	g++ -o server server.cpp HandleServer.o global.cpp -lmysqlclient -lpthread -lhiredis
	g++ -o client client.cpp HandleClient.o -lpthread

HandleServerUseThreadPool.o: HandleServerUseThreadPool.cpp HandleClient.o global.cpp
	g++ -c HandleServerUseThreadPool.cpp HandleClient.o global.cpp -lmysqlclient -lp -lhiredis

HandleServer.o: HandleServer.cpp HandleClient.o global.cpp
	g++ -c HandleServer.cpp HandleClient.o global.cpp -lmysqlclient -lpthread -lhiredis

HandleClient.o:	HandleClient.cpp
	g++ -c HandleClient.cpp

clean:
	rm server
	rm client
	rm *.o
