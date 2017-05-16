##########################################################
# File Name: gg.sh
# Author: lyh
# E-mail: 
# Created Time: 2017年05月16日 星期二 14时41分20秒
##########################################################
#!/bin/bash

g++ -g ../buffer.cpp ../net_socket.cpp ../net_base.cpp ../event_loop.cpp ../woker.cpp ../tcpconnection.cpp ../tcpserver.cpp ../servant.cpp ./server.cpp -std=c++11 -lpthread -o server
