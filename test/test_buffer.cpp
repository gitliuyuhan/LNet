/*======================================================
    > File Name: test_buffer.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2017年05月13日 星期六 16时54分53秒
 =======================================================*/

#include <iostream>
#include "../buffer.h"
#include "../net_socket.h"

int main()
{
    lnet::Buffer buf(12);
    char str[6] = "12345";
    char out[20];
    int a = 10;
    
    /*
    buf.append(str,5);
    buf.writeBuffer(str,5);
    buf.readBuffer(out,1);
    std::cout << out << std::endl;
    std::cout << buf.writeableSize() << " " <<buf.readableSize() << std::endl;
    buf.printBuffer();
    std::cout << std::endl;
    buf.writeBuffer(str,5);
    buf.printBuffer();
    std::cout << std::endl;
    std::cout << buf.writeableSize() << std::endl;
    */
    buf.writeBuffer((const char*)&a,sizeof(int));
    int b;
    //buf.readBuffer<int>(&b,sizeof(int));
    buf.copySomeData<int>(&b,sizeof(int));
    std::cout << b << std::endl;
    return 0;
}
