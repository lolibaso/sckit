// modified from https://github.com/kokke/tiny-AES-c.git

#ifndef _sc_aes256_h_
#define _sc_aes256_h_

// AES encryption in CBC-mode of operation.

#include <stdint.h>

namespace sc {

#define AES_BLOCKLEN 16 //Block length in bytes AES is 128b block only
#define AES_KEYLEN 32
#define AES_keyExpSize 240

struct AES_ctx
{
  uint8_t RoundKey[AES_keyExpSize];
  uint8_t Iv[AES_BLOCKLEN];
};

void AES256_init_ctx(struct AES_ctx* ctx, const uint8_t* key);
void AES256_init_ctx_iv(struct AES_ctx* ctx, const uint8_t* key, const uint8_t* iv);
void AES256_ctx_set_iv(struct AES_ctx* ctx, const uint8_t* iv);

// buffer size MUST be mutile of AES_BLOCKLEN;
// Suggest https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme
// NOTES: you need to set IV in ctx via AES_init_ctx_iv() or AES_ctx_set_iv()
//        no IV should ever be reused with the same key 
void AES256_CBC_encrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);
void AES256_CBC_decrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);

}

#endif // _sc_aes_h_
