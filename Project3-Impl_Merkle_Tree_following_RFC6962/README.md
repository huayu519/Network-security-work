#  README

## Part1

代码说明

1. 本次实验采用 visual studio实现；使用openssl中的evp.h调用SM3摘要算法
2. 代码第一部分是实现MerkleTree中hash功能的函数： tDigest（）函数的实现代码如下图所示

```c++

void tDigest(char msg[], MerkleTree* mt)
{
	unsigned char sm3_value[EVP_MAX_MD_SIZE];   //保存输出的摘要值的数组
	unsigned int sm3_len, i;
	EVP_MD_CTX* sm3ctx;                         //EVP消息摘要结构体
	sm3ctx = EVP_MD_CTX_new();//调用函数初始化

	EVP_MD_CTX_init(sm3ctx);                    //初始化摘要结构体
	EVP_DigestInit_ex(sm3ctx, EVP_sm3(), NULL); //设置摘要算法和密码算法引擎，这里密码算法使用sm3，算法引擎使用OpenSSL默认引擎即软算法
	EVP_DigestUpdate(sm3ctx, msg, strlen(msg));//调用摘要UpDate计算msg的摘要
	EVP_DigestFinal_ex(sm3ctx, sm3_value, &sm3_len);//摘要结束，输出摘要值   
	EVP_MD_CTX_reset(sm3ctx);                       //释放内存
	char hash_1[65];
	sprintf_s(hash_1, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
		sm3_value[0], sm3_value[1], sm3_value[2], sm3_value[3], sm3_value[4], sm3_value[5], sm3_value[6], sm3_value[7],
		sm3_value[8], sm3_value[9], sm3_value[10], sm3_value[11], sm3_value[12], sm3_value[13], sm3_value[14], sm3_value[15],
		sm3_value[16], sm3_value[17], sm3_value[18], sm3_value[19], sm3_value[20], sm3_value[21], sm3_value[22], sm3_value[23],
		sm3_value[24], sm3_value[25], sm3_value[26], sm3_value[27], sm3_value[28], sm3_value[29], sm3_value[30], sm3_value[31]);
	for (int i = 0; i < sizeof(mt->hash); i++)
	{
		mt->hash[i] = hash_1[i];
	}
}
```

3. 第二部分是MerkleTree的整体结构的实现，主要是使用递归的方法从叶子节点开始逐一构建MerkleTree
4. 第三部分是更新MerkleTree的hash值，hash值的计算方法有两类，一类是叶子节点的hash值实现，另一类是内部节点的hash值实现。

* 叶子节点的hash值实现——将内容前面加上00，进行hash ——hash（0x00 || m）

```c++
// 计算根节点的hash值// hash（0x00||m)
void hash_int(int num, MerkleTree* mt) {
	char temp_1[2];
	sprintf_s(temp_1, "%x", num);
	char temp_2[sizeof(temp_1) + 2] = "0";
	strcpy_s(temp_2 + 1, sizeof(temp_1) + 1, temp_1);
	tDigest(temp_2, mt);
}
```

* 内部节点的hash值实现——将内容前面加上01，再将两个子节点的hash值进行拼接作为消息——hash（0x01 || hash1 || hash2）

```c++
// hash（0x01||h1||h2)
void hash_uint(MerkleTree* Left, MerkleTree* Right, MerkleTree* mt) {
	if (Right == NULL)
	{
		for (int i = 0; i < sizeof(mt->hash); i++)
		{
			mt->hash[i] = Left->hash[i];
		}
	}
	else
	{
		char* temp_1 = Left->hash;
		char* temp_2 = Right->hash;
		strcpy_s(temp_1 + strlen(temp_1), strlen(temp_2) + 1, temp_2);
		char temp_3[130] = "1";
		strcpy_s(temp_3 + 1, strlen(temp_1) + 1, temp_1);
		tDigest(temp_3, mt);

	}
}
```





## Part2

运行指导

1. 在visual studio  Debug x64模式下运行
2. 运行之前先配置openssl项目属性

![image-20220730111751368](https://github.com/huayu519/Network-security-work/blob/main/images/image-20220730111751368.png)



![image-20220730111819100](https://github.com/huayu519/Network-security-work/blob/main/images/image-20220730111819100.png)

## Part3

运行全过程截图：节点的hash值只取了前16bit展示

![image-20220730145846229](https://github.com/huayu519/Network-security-work/blob/main/images/image-20220730145846229.png)



## Part4

具体贡献：小组成员只有一人，单独完成

