/*======================================================
    > File Name: net_base.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月14日 星期一 15时54分34秒
 =======================================================*/

#include "net_base.h"

namespace lnet
{
    int setNonBlockFd(int fd)
    {
        int old_option = fcntl(fd,F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        fcntl(fd,F_SETFL,new_option);
        return old_option;
    }
}

