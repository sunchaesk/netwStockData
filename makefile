c:main.cpp
	g++ -Wall -g main.cpp netw_controller.cpp netw.cpp -lcurl -lboost_system -lboost_filesystem -o c

clean:
	rm c
