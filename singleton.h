/*======================================================
    > File Name: singleton.h
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2017年05月22日 星期一 22时20分31秒
 =======================================================*/

#ifndef SINGLETON_H
#define SINGLETON_H

#include <iostream>

namespace lnet
{
template<class T>
class Singleton
{
public:
    static T* getInstance()
    {
        static T _instance;
        return &_instance;
    }
private:
    Singleton(){}
};
}
#endif
