/*======================================================
    > File Name: net_base.h
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月14日 星期一 15时48分58秒
 =======================================================*/

#ifndef NET_BASE_H
#define NET_BASE_H

#include <iostream>
#include <string>

namespace lnet
{
int setNonBlockFd(int fd);
std::string nowTimeToString();
}

#endif //NET_BASE_H

