##########################################################
# File Name: cc.sh
# Author: lyh
# E-mail: 
# Created Time: 2017年05月24日 星期三 15时08分44秒
##########################################################
#!/bin/bash
g++ ../net_base.cpp ../net_socket.cpp  client.cpp -std=c++11 -o client
