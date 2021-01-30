all: serverUseThreadPool.cpp HandleClient.o server.cpp HandleServer.o client.cpp global.cpp HandleServerUseThreadPool.o serverV2.cpp HandleServerV2.o
	g++ -o serverUseThreadPool serverUseThreadPool.cpp HandleServerUseThreadPool.o global.cpp -lmysqlclient -lpthread -lhiredis
	g++ -o server server.cpp HandleServer.o global.cpp -lmysqlclient -lpthread -lhiredis
	g++ -o client client.cpp HandleClient.o -lpthread
	g++ -o serverV2 serverV2.cpp  HandleServerV2.o global.cpp -lmysqlclient -lpthread -lhiredis

serverUseThreadPool: serverUseThreadPool.cpp global.cpp HandleServerUseThreadPool.o
	g++ -o serverUseThreadPool serverUseThreadPool.cpp HandleServerUseThreadPool.o global.cpp -lmysqlclient -lpthread -lhiredis

server: server.cpp HandleServer.o global.cpp 
	g++ -o server server.cpp HandleServer.o global.cpp -lmysqlclient -lpthread -lhiredis

client: client.cpp HandleClient.o 
	g++ -o client client.cpp HandleClient.o -lpthread

serverV2: serverV2.cpp HandleServerV2.o global.cpp
	g++ -o serverV2 serverV2.cpp  HandleServerV2.o global.cpp -lmysqlclient -lpthread -lhiredis

HandleServerV2.o:HandleServerV2.cpp
	g++ -c HandleServerV2.cpp -lmysqlclient -lpthread -lhiredis

HandleServerUseThreadPool.o: HandleServerUseThreadPool.cpp
	g++ -c HandleServerUseThreadPool.cpp -lmysqlclient -lpthread -lhiredis

HandleServer.o: HandleServer.cpp
	g++ -c HandleServer.cpp -lmysqlclient -lpthread -lhiredis

HandleClient.o:	HandleClient.cpp
	g++ -c HandleClient.cpp

clean:
	rm server
	rm client
	rm serverV2
	rm serverUseThreadPool
	rm *.o
