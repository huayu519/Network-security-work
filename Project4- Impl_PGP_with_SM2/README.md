#  README

## Part1

代码说明

1. 本次实验采用 visual studio实现；使用openssl中的evp.h调用SM2加解密算法，同时使用AES加密算法
2. 实验代码的第一部分就是实现SM2的加解密工作
3. 实验代码的第二部分是调用AES加密算法
4. 代码的具体流程就是：明文——AES加密——SM2使用公钥对AES密钥加密——SM2使用私钥对AES密钥解密——AES解密——明文

## Part2

运行指导

1. 在visual studio  Debug x64模式下运行
2. 运行之前先配置openssl项目属性

![image-20220730111751368](https://github.com/huayu519/Network-security-work/blob/main/images/image-20220730111751368.png)



![image-20220730111819100](https://github.com/huayu519/Network-security-work/blob/main/images/image-20220730111819100.png)

3. 正常调试运行代码即可

## Part3

运行全过程截图

![image-20220730233517381](https://github.com/huayu519/Network-security-work/blob/main/images/image-20220730233517381.png)



## Part4

具体贡献：小组成员只有一人，单独完成

