#include "picosha2.h"
#include "lightMD5.h"
#include "DPK.h"

#include <cstdint>
#include <cstring> 
#include <vector> 
#include <string> 

const uint8_t SHARED_SECRET[] = "IOT2026"; 

// convert hex char to value 0-15

uint8_t hexToNibble(char c)
{
    if (c >= '0' && c <= '9') return (uint8_t)(c - '0');
    if (c >= 'a' && c <= 'f') return (uint8_t)(10 + (c - 'a'));
    if (c >= 'A' && c <= 'F') return (uint8_t)(10 + (c - 'A'));
    return 0;
}

// convert hex string into raw bytes 

void hexToBytes(const std::string &hex, uint8_t *output, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        uint8_t high = hexToNibble(hex[2 * i]);
        uint8_t low  = hexToNibble(hex[2 * i + 1]);
        output[i] = (uint8_t)((high << 4) | low);
    }
}


void makeKey(const uint8_t *salt, size_t saltLen, uint8_t key[32])
{
    uint8_t secretHash[32];
    uint8_t K1[16]; 
    uint8_t K12[16]; 
    uint8_t K2[16]; 
    uint8_t K3[16]; 


    // Using the SHA256 (secret) with the PicoSHA2 library 

    std::vector<unsigned char> shaVec; 
    picosha2::hash256(
        SHARED_SECRET,
        SHARED_SECRET + sizeof(SHARED_SECRET) - 1, 
        shaVec
    ); 

    for (int i = 0; i < 32; i++)
        secretHash[i] = shaVec[i]; 

    memcpy(K1, secretHash, 16); 
    memcpy(K12, secretHash + 16, 16); 


    // MD5 (salt) using LightMD5 library 

    std::string md5Hex = md5_process((uint8_t*)salt, saltLen); 
    hexToBytes(md5Hex, K2, 16); 

    // XOR K1 AND K2 

    for (int i = 0; i < 16; i++)
        K3[i] = (uint8_t)(K1[i] ^ K2[i]); 

    // Final DPK = K3 || K12 256 bits = 32 byte key 
    memcpy(key, K3, 16); 
    memcpy(key + 16, K12, 16); 

}

