#include "test_comm.h"
#include "aes128.h"

using namespace sc;

uint8_t key[16] = { '1', '2', '3', '1', '2', '3', '1', '2', '3', '1', '2', '3', '1', '2', '3','0' }; 
uint8_t iv[16] = { '1', '1', '2', '3', '1', '2', '3', '1', '2', '3', '1', '2', '3', '1', '2', '3' };

uint8_t in[128];

int main(int argc, char* argv[])
{
    struct AES_ctx  ctx;

    AES128_init_ctx_iv(&ctx, key, iv); 

    BZERO(in, sizeof(in));

    memcpy(in,  argv[1], strlen(argv[1]));

    hexdump(stdout, in, sizeof(in));

    printf("\n");

    AES128_CBC_encrypt_buffer(&ctx, in, sizeof(in)); 

    hexdump(stdout, in, sizeof(in));
    printf("\n");

    memset(&ctx, 0, sizeof(ctx));
    AES128_init_ctx_iv(&ctx, key, iv); 
    
    AES128_CBC_decrypt_buffer(&ctx, in, sizeof(in));

    hexdump(stdout, in, sizeof(in));
    printf("\n");

    return 0;
}
