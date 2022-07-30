#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <process.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <assert.h>




//SM2公钥和私钥
std::string sm2PriKeyStr = "MIICSwIBADCB7AYHKoZIzj0CATCB4AIBATAsBgcqhkjOPQEBAiEA/v8AAAAA//8wRAQg/v8AAAAA//wEICjp+p6dn140TVqeS89lCafzl4n1FauPkt28vUFNlA6TBEEEMsSuLB8ZgRlfmQRGajnJlI/jC7/yZgvhcVpFiTNMdMe8Nzai9PZ3nFm9zuNraSFT0KmHfMYqR0AC3zLlITnwoAIhAP7///9yA99rIcYFK1O79Ak51UEjAgEBBIIBVTCCAVECAQEEINXHVaHjZdZjM3Ja9CyYR/VT4ZXqX2JCG1w59I+G2DoToIHjMIHgAgEBMCwGByqGSM49AQECIQD+/wAAAAD//zBEBCD+/wAAAAD//AQgKOn6np2fXjRNWp5Lz2UJp/OXifUVq4+S3by9QU2UDpMEQQQyxK4sHxmBGV+ZBEZqOcmUj+MLv/JmC+FxWkWJM0x0x7w3NqL09necWb3O42tpIVPQqYd8xipHQALfMuUhOfCgAiEA/v///3ID32shxgUrU7v0CTnVQSMCAQGhRANCAAQ8nhkap78DwzgwGnIBNfgXNIyoqPzfT+rnmXvRJ7NY8fjONiql9wQYxliyuAil6lNu2ax2MULoG43kdKE8a2JU";
std::string sm2PubKeyStr = "MIIBMzCB7AYHKoZIzj0CATCB4AIBATAsBgcqhkjOPQEBAiEA/v8AAAAA//8wRAQg/v8AAAAA//wEICjp+p6dn140TVqeS89lCafzl4n1FauPkt28vUFNlA6TBEEEMsSuLB8ZgRlfmQRGajnJlI/jC7/yZgvhcVpFiTNMdMe8Nzai9PZ3nFm9zuNraSFT0KmHfMYqR0AC3zLlITnwoAIhAP7///9yA99rIcYFK1O79Ak51UEjAgEBA0IABDyeGRqnvwPDODAacgE1+Bc0jKio/N9P6ueZe9Ens1jx+M42KqX3BBjGWLK4CKXqU27ZrHYxQugbjeR0oTxrYlQ=";

//SM2加密算法
int my_sm2encrpt(std::string keystr, unsigned char* sourStr, int cStrlen, unsigned char* enStr)
{
    BIO* bp = NULL;
    EVP_PKEY* pkey = NULL;
    EVP_PKEY_CTX* ectx = NULL;
    size_t cEnStrlen;
    char* cEnStr = NULL;
    char* chPublicKey = const_cast<char*>(keystr.c_str());
    if ((bp = BIO_new_mem_buf(chPublicKey, -1)) == NULL)
    {
        printf("BIO_new_mem_buf failed!\n");
        return NULL;
    }
    pkey = PEM_read_bio_PUBKEY(bp, NULL, NULL, NULL);
    BIO_free_all(bp);
    if ((EVP_PKEY_set_alias_type(pkey, EVP_PKEY_SM2)) != 1)
    {
        goto clean_up;
    }

    if (!(ectx = EVP_PKEY_CTX_new(pkey, NULL)))
    {
        goto clean_up;
    }

    if ((EVP_PKEY_encrypt_init(ectx)) != 1)
    {
        goto clean_up;
    }

    if ((EVP_PKEY_encrypt(ectx, NULL, &cEnStrlen, reinterpret_cast<unsigned char*>(sourStr), (size_t)(cStrlen))) != 1)
    {
        goto clean_up;
    }
    if (!(cEnStr = (char*)malloc(cEnStrlen)))
    {
        goto clean_up;
    }

    if ((EVP_PKEY_encrypt(ectx, reinterpret_cast<unsigned char*>(cEnStr), &cEnStrlen, reinterpret_cast<unsigned char*>(sourStr), cStrlen)) != 1)
    {
        goto clean_up;
    }
    printf("enStrLen2:\n %d\n", cEnStrlen);
    memcpy(enStr, cEnStr, cEnStrlen);
    return (int)(cEnStrlen);

clean_up:

    if (pkey)
    {
        EVP_PKEY_free(pkey);
    }
    if (ectx)
    {
        EVP_PKEY_CTX_free(ectx);
    }

    if (sourStr)
    {
        free(sourStr);
    }

    if (cEnStr)
    {
        free(cEnStr);
    }
}

//SM2解密算法
int dencryptStr(std::string keystr, unsigned char* cEnStr, int cEnstrlen, unsigned char* deStr)

{
    BIO* priBp = NULL;
    EVP_PKEY* mSm2PriKey;
    EVP_PKEY_CTX* ectx = NULL;
    size_t cDeStrlen = 0;
    char* cDeStr = NULL;
    //create pri key
    char* chPrilicKey = const_cast<char*>(keystr.c_str());
    if ((priBp = BIO_new_mem_buf(chPrilicKey, -1)) == NULL)
    {
        printf("BIO_new_mem_buf failed!\n");
    }
    mSm2PriKey = PEM_read_bio_PrivateKey(priBp, NULL, NULL, NULL);
    BIO_free_all(priBp);
    if (NULL == mSm2PriKey)
    {
        ERR_load_crypto_strings();
        char errBuf[512];
        ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
        printf("load sm2 private key failed[%s]\n", errBuf);
    }
    //解密
    if ((EVP_PKEY_set_alias_type(mSm2PriKey, EVP_PKEY_SM2)) != 1)
    {
        printf("EVP_PKEY_set_alias_type failed!\n");
    }

    if (!(ectx = EVP_PKEY_CTX_new(mSm2PriKey, NULL)))
    {
        printf("EVP_PKEY_CTX_new failed!\n");
    }
    if ((EVP_PKEY_decrypt_init(ectx)) != 1)
    {
        printf("EVP_PKEY_decrypt_init failed!\n");
    }
    if ((EVP_PKEY_decrypt(ectx, NULL, &cDeStrlen, reinterpret_cast<unsigned char*>(cEnStr), cEnstrlen)) != 1)
    {
        printf("EVP_PKEY_decrypt failed!\n");
        ERR_load_crypto_strings();
        char errBuf[512];
        ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
        printf("EVP_PKEY_decrypt[%s]\n", errBuf);
    }
    if (!(cDeStr = (char*)malloc(cDeStrlen)))
    {
        printf(" (unsigned char *)malloc(cDeStrlen)) failed!\n");
    }
    if ((EVP_PKEY_decrypt(ectx, reinterpret_cast<unsigned char*>(cDeStr), &cDeStrlen, reinterpret_cast<unsigned char*>(cEnStr), cEnstrlen)) != 1)
    {
        printf(" EVP_PKEY_decrypt failed!\n");
    }
    printf("cDeStrlen:%d\n", cDeStrlen);
    memcpy(deStr, cDeStr, cDeStrlen);
    EVP_PKEY_CTX_free(ectx);
    free(cDeStr);
    return cDeStrlen;
}

//AES密钥和初始向量
#define AESKEY "df98b715d5c6ed2b25817b6f255411a1"	//HEX密钥
#define AESIV "2841ae97419c2973296a0d4bdfe19a4f"	//HEX初始向量

//将文本形式的HEX串进行转换
unsigned char* str2hex(char* str)
{
    unsigned char* ret = NULL;
    int str_len = strlen(str);
    int i = 0;
    assert((str_len % 2) == 0);
    ret = (unsigned char*)malloc(str_len / 2);
    for (i = 0; i < str_len; i = i + 2)
    {
        sscanf_s(str + i, "%2hhx", &ret[i / 2]);
    }
    return ret;
}



//PGP实现
int main(int argc, char* argv[])
{
    //SM2私钥转换
    unsigned char sm2_en[512], sm2_de[512];
    int sm2enStrLen, sm2deStrLen;
    for (int i = 64; i < sm2PriKeyStr.size(); i += 64)
    {
        if (sm2PriKeyStr[i] != '\n')
        {
            sm2PriKeyStr.insert(i, "\n");
        }
        ++i;
    }
    sm2PriKeyStr.insert(0, "-----BEGIN EC PARAMETERS-----\nBggqgRzPVQGCLQ==\n-----END EC PARAMETERS-----\n-----BEGIN EC PRIVATE KEY-----\n");
    sm2PriKeyStr.append("\n-----END EC PRIVATE KEY-----\n");

    //SM2公钥转换
    for (int i = 64; i < sm2PubKeyStr.size(); i += 64)
    {
        if (sm2PubKeyStr[i] != '\n')
        {
            sm2PubKeyStr.insert(i, "\n");
        }
        ++i;
    }
    sm2PubKeyStr.insert(0, "-----BEGIN PUBLIC KEY-----\n");
    sm2PubKeyStr.append("\n-----END PUBLIC KEY-----\n");


    //对AES加密算法密钥的处理
    AES_KEY encryptkey;
    AES_KEY decryptkey;

    unsigned char* key;
    unsigned char* stdiv;

    key = str2hex((char*)AESKEY);
    stdiv = str2hex((char*)AESIV);
    AES_set_encrypt_key(key, 128, &encryptkey);
    AES_set_decrypt_key(key, 128, &decryptkey);


    //对明文的处理

    unsigned char plain_text[32];

    memcpy(plain_text, "Implement a PGP scheme with SM2", 31);
    memset(plain_text + 31, 0, 1);
    //需要将加密区块长度填充为16字节整数倍，此处使用zero-padding，即末尾全用0填充
    printf("plain_text: ");
    for (int i = 0; i < 32; i++)
    {
        printf(" % 02X ", plain_text[i]);
    }
    printf("\n");


    //加密过程
    //AES 将明文加密
    unsigned char encrypted_text[32];

    //输出加密密文
    printf("AES_en: \n");
    for (int i = 0; i < 32; i++)
    {
        printf("0x%02x ", smencrypted_text2_en[i]);
    }
    printf("\n");

    memset(encrypted_text, 0, 32);
    unsigned char tmpiv[16];
    memcpy(tmpiv, stdiv, 16);
    AES_cbc_encrypt(plain_text, encrypted_text, 32, &encryptkey, tmpiv, AES_ENCRYPT);
    //初始向量这个参数每次使用都会将其改变，有兴趣的话可以把调用前后这个地址下的内容打印出来。所以如果要多次加密且每次使用固定的初始向量，可以先用tmpiv接收

    //使用SM2将AES密钥加密


 

    //加密并输出
    sm2enStrLen = my_sm2encrpt(sm2PubKeyStr, encryptkey, strlen(encryptkey), sm2_en);
    printf("sm2enStrLen :%d \n", sm2enStrLen);
    printf("sm2_en: \n");
    for (int i = 0; i < sm2enStrLen; i++)
    {
        printf("0x%02x ", sm2_en[i]);
    }
    printf("\n");




    //解密
    //使用SM2算法解密AES密钥
    sm2deStrLen = dencryptStr(sm2PriKeyStr, sm2_en, sm2enStrLen, sm2_de);
    printf("sm2deStrLen :%d \n", sm2deStrLen);
    printf("sm2_de: ");
    for (int i = 0; i < sm2deStrLen; i++)
    {
        printf("0x%x ", sm2_de[i]);
    }
    printf("\n");

    //使用AES算法解密消息
    unsigned char decrypted_text[32];

    memset(decrypted_text, 0, 32);
    memcpy(tmpiv, stdiv, 16);
    AES_cbc_encrypt(encrypted_text, decrypted_text, 32, &decryptkey, tmpiv, AES_DECRYPT);

}
