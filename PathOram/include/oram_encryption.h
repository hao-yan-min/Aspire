#pragma once
#include <stdio.h>
#include<iostream>
#include"Block.h"
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <algorithm>

#define AES_KEY_LENGTH 16

int aes_encrypt(unsigned char *plaintext, int plaintext_len,
                unsigned char *key, unsigned char *iv,
                unsigned char *ciphertext);
int aes_decrypt(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *key, unsigned char *iv,
                unsigned char *plaintext);
int block_encrypt(const int *plainblock, unsigned char *key, int *cipherblock);
int block_decrypt(const int *cipherblock, unsigned char *key, int *plainblock);