#include <iostream>
#include <string>
#include <string.h>
#include <openssl/evp.h>
#include <bitset>
#include <algorithm>
#include <map>
#include<windows.h> 
using namespace std;

#include <stdio.h>


int main()
{
    double time = 0;   //计时工具
    LARGE_INTEGER nFreq;
    LARGE_INTEGER nBeginTime;
    LARGE_INTEGER nEndTime;
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&nBeginTime);//开始计时  

    map<string, int> hashMap; //构建map检测碰撞
    OpenSSL_add_all_algorithms();//初始化OpenSSL库
    unsigned int i = 0;  //遍历用数

    unsigned char sm3_value[EVP_MAX_MD_SIZE];   //保存输出的摘要值的数组
    unsigned int sm3_len;
    EVP_MD_CTX* sm3ctx;                         //EVP消息摘要结构体
    sm3ctx = EVP_MD_CTX_new();//调用函数初始化

    EVP_MD_CTX_init(sm3ctx);                    //初始化摘要结构体
    EVP_DigestInit_ex(sm3ctx, EVP_sm3(), NULL); //设置摘要算法和密码算法引擎，这里密码算法使用sm3，算法引擎使用OpenSSL默认引擎即软算法

    bitset<16> a(i); //存储16进制消息
    string b = a.to_string();//将消息转换成string
    const char* c = b.data();//将string转换成const char*
    while (1)
    {
        EVP_DigestUpdate(sm3ctx, c, strlen(c));//调用摘要UpDate计算msg1的摘要
        EVP_DigestFinal_ex(sm3ctx, sm3_value, &sm3_len);//摘要结束，输出摘要值  
       

        string result;
        char new_sm3_value[8];
        sprintf_s(new_sm3_value, "%x%x", sm3_value[0], sm3_value[1]);
        result = new_sm3_value;//强制将char*转换乘string

        c = (char*)sm3_value; //将本次的hash结果作为下一次的消息

        map<string, int>::iterator iter = hashMap.find(result);
        if (hashMap.end() == iter) {
            hashMap[result] = i;
        }
        else {
            QueryPerformanceCounter(&nEndTime);//停止计时  
            cout << "出现环，找到一组16-bit碰撞";
            EVP_MD_CTX_reset(sm3ctx);                       //释放内存
            break; //跳出循环
        }
        i++;
    }
    time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
    cout << "Time：" << time << "s" << endl;
    return 0;
}
