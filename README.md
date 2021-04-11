# server-client

Cli was downloaded from https://github.com/daniele77/cli and added to path in /usr/local/include 

compile async server with command: g++  web_server.cpp -o serv -std=c++1y  -lboost_system -lpthread
and run: /serv

compile async server with command: g++  web_client.cpp -o client -std=c++1y  -lboost_system -lpthread
and run: /client
