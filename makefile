c:main.cpp
	g++ -Wall -g  netw_controller.cpp netw.cpp main.cpp -lcurl -lboost_system -lboost_filesystem -o c

clean:
	rm c
