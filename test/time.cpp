/*======================================================
    > File Name: time.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2017年05月23日 星期二 03时52分08秒
 =======================================================*/

#include <iostream>
#include "../net_base.h"
#include "../log.h"
#include "../singleton.h"

int main()
{
    lnet::Singleton<lnet::Log>::getInstance()->init(lnet::WARN);
    LOG_DEBUG("hello");
    LOG_INFO("world");
    LOG_WARN("aaa");
    LOG_ERROR("bbb");
    return 0;
}
