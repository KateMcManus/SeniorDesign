LIBS=-lpthread -lncurses
CXX := g++

all: emcserv

#%.o: %.cpp
#	$(CXX) -c $< -std=c++11 -o $@

#%.o: %.hpp
#	$(CXX) -c $< -std=c++11 -o $@

emcserv:
	@echo "** Building EMC Server"
	$(CXX) $(DEBUG) -std=c++11 -o emcserv main.cpp PCSBSocket.lin.cpp PCSBSocketUDP.lin.cpp EMCMessageDefinitions.hpp sp2Udp.h sp2Udp.cpp $(LIBS)

debug: DEBUG = -DDEBUG -g

debug: emcserv

install:
	@echo '** Installing...'
	cp emcserv ../

