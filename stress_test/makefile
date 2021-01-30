target: HandleClient.o client.cpp global.cpp serverV2.cpp HandleServerV2.o AddAccount.cpp
	g++ -o client client.cpp HandleClient.o -lpthread
	g++ -o serverV2 serverV2.cpp  HandleServerV2.o global.cpp -lmysqlclient -lpthread -lhiredis
	g++ -o addaccount AddAccount.cpp -lmysqlclient 

HandleServerV2.o:HandleServerV2.cpp 
	g++ -c HandleServerV2.cpp -lmysqlclient -lpthread -lhiredis

HandleClient.o:	HandleClient.cpp
	g++ -c HandleClient.cpp

clean:
	rm client
	rm serverV2
	rm *.o
