#  README

## Part1

代码说明

1. 本次实验采用 visual studio实现；使用openssl中的evp.h调用SM3摘要算法
1. 代码中的第一部分就是使用openssl实现SM3摘要算法，具体过程分为两点，一点是初始化算法，另一点是调用摘要算法，具体代码如下

* 初始化算法

```c++
 	unsigned char sm3_value[EVP_MAX_MD_SIZE];   //保存输出的摘要值的数组
    unsigned int sm3_len;
    EVP_MD_CTX* sm3ctx;                         //EVP消息摘要结构体
    sm3ctx = EVP_MD_CTX_new();//调用函数初始化

    EVP_MD_CTX_init(sm3ctx);                    //初始化摘要结构体
    EVP_DigestInit_ex(sm3ctx, EVP_sm3(), NULL); //设置摘要算法和密码算法引擎，这里密码算法使用sm3，算法引擎使用OpenSSL默认引擎即软算法
```

* 调用摘要算法

```c++
	EVP_DigestUpdate(sm3ctx, c, strlen(c));//调用摘要UpDate计算msg1的摘要
    EVP_DigestFinal_ex(sm3ctx, sm3_value, &sm3_len);//摘要结束，输出摘要值  
```

3. 代码中的第二部分是寻找环，具体过程分为两点，一点是不断将hash结果再次进行hash，另一点是使用map容器进行匹配

![image-20220730115418031](https://github.com/huayu519/Network-security-work/blob/main/images/image-20220730115418031.png)

* 不断将hash结果再次进行hash

```c++
		string result;
        char new_sm3_value[8];
        sprintf_s(new_sm3_value, "%x%x", sm3_value[0], sm3_value[1]);
        result = new_sm3_value;//强制将char*转换乘string

        c = (char*)sm3_value; //将本次的hash结果作为下一次的消息
```



* 使用map容器匹配碰撞

```c++
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
```



## Part2

运行指导

1. 在visual studio  Debug x64模式下运行
2. 运行之前先配置openssl项目属性

![image-20220730111751368](https://github.com/huayu519/Network-security-work/blob/main/images/image-20220730111751368.png)



![image-20220730111819100](https://github.com/huayu519/Network-security-work/blob/main/images/image-20220730111819100.png)

3. 正常调试运行代码即可



## Part3

运行全过程截图

找到一个环

![image-20220730120028165](https://github.com/huayu519/Network-security-work/blob/main/images/image-20220730120028165.png)



## Part4

具体贡献：小组成员只有一人，单独完成



