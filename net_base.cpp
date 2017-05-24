/*======================================================
    > File Name: net_base.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月14日 星期一 15时54分34秒
 =======================================================*/

#include <unistd.h>
#include <fcntl.h>

#include "net_base.h"
#include "string.h"

using namespace lnet;

int lnet::setNonBlockFd(int fd)
{
    int old_option = fcntl(fd,F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}

std::string lnet::nowTimeToString()
{
    time_t     t = time(NULL);
    struct tm  tmp;
    char       t_str[20];
    localtime_r(&t,&tmp);
    snprintf(t_str,20,"%04d/%02d/%02d %02d:%02d:%02d",
            tmp.tm_year+1900,tmp.tm_mon+1,tmp.tm_mday,
            tmp.tm_hour,tmp.tm_min,tmp.tm_sec);
    return std::string(t_str);
}
