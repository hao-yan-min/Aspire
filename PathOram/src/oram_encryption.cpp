#include"oram_encryption.h"

int aes_encrypt(unsigned char *plaintext, int plaintext_len,
                unsigned char *key, unsigned char *iv,
                unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;

    int len = 0;

    int ciphertext_len;

    /* Create and initialise the context */
    ctx = EVP_CIPHER_CTX_new();

    /* Initialise the encryption operation. */
    EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv);

    /* Encrypt the message */
    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    ciphertext_len = len;

    /* Finalise the encryption */
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int aes_decrypt(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *key, unsigned char *iv,
                unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;

    int len = 0;

    int plaintext_len;

    /* Create and initialise the context */
    ctx = EVP_CIPHER_CTX_new();

    /* Initialise the decryption operation. */
    EVP_DecryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv);

    /* decrypt the message */
    EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    plaintext_len = len;

    /* Finalise the encryption */
    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}


int block_encrypt(const int *plainblock, unsigned char *key, int *cipherblock)
{
    if(plainblock == nullptr || cipherblock == nullptr || key == nullptr)
    {
        printf("Invalid input to block_encrypt\n");
        return -1; 
    }
    unsigned char iv[AES_KEY_LENGTH];
    RAND_bytes(iv, AES_KEY_LENGTH);
    memcpy(reinterpret_cast<unsigned char*>(cipherblock),iv,AES_KEY_LENGTH);
    unsigned char* plaintext = new unsigned char[Block::BLOCK_SIZE * sizeof(int)-16];
    memcpy(plaintext, plainblock, Block::BLOCK_SIZE * sizeof(int)-16);  
    aes_encrypt(plaintext, Block::BLOCK_SIZE * sizeof(int)-16, key, iv, reinterpret_cast<unsigned char*>(cipherblock)+16);
    delete[] plaintext;
    //aes_encrypt(reinterpret_cast<unsigned char*>(plainblock), Block::BLOCK_SIZE * sizeof(int)-16, key, iv, reinterpret_cast<unsigned char*>(cipherblock)+16);
    
    return 0;
}
int block_decrypt(const int *cipherblock, unsigned char *key,  int *plainblock)
{
    if(plainblock == nullptr || cipherblock == nullptr || key == nullptr)
    {
        printf("Invalid input to block_decrypt\n");
        return -1;
    }
    unsigned char iv[AES_KEY_LENGTH];
    memcpy(iv, reinterpret_cast<const unsigned char*>(cipherblock), AES_KEY_LENGTH);
    unsigned char* ciphertext = new unsigned char[Block::BLOCK_SIZE * sizeof(int)-16];
    memcpy(ciphertext, reinterpret_cast<const unsigned char*>(cipherblock)+16, Block::BLOCK_SIZE * sizeof(int)-16);
    aes_decrypt(ciphertext, Block::BLOCK_SIZE * sizeof(int)-16, key, iv, reinterpret_cast<unsigned char*>(plainblock));
    delete[] ciphertext;
    return 0;
}
