#include "picosha2.h"
#include "lightMD5.h"
#include "DPK.h"

#include <cstdint>
#include <cstring> 
#include <vector> 
#include <string> 

// Shared secret used for key derivation - must be the same on both sender and receiver
const uint8_t SHARED_SECRET[] = "IOT2026"; 

// Converts a single hexadecimal character to its numeric value (0-15)
// Returns: Numeric value 0-15, or 0 if invalid character
uint8_t hexToNibble(char c)
{
    if (c >= '0' && c <= '9') return (uint8_t)(c - '0');
    if (c >= 'a' && c <= 'f') return (uint8_t)(10 + (c - 'a'));
    if (c >= 'A' && c <= 'F') return (uint8_t)(10 + (c - 'A'));
    return 0;
}

// Converts a hexadecimal string into raw byte array
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
    // Temporary buffers for key derivation steps
    uint8_t secretHash[32];  // SHA-256 hash of the shared secret
    uint8_t K1[16];          // First 16 bytes of secretHash
    uint8_t K12[16];         // Last 16 bytes of secretHash
    uint8_t K2[16];          // MD5 hash of the salt
    uint8_t K3[16];          // XOR result of K1 and K2

    // Step 1: Hash the shared secret using SHA-256
    std::vector<unsigned char> shaVec; 
    picosha2::hash256(
        SHARED_SECRET,
        SHARED_SECRET + sizeof(SHARED_SECRET) - 1, 
        shaVec
    ); 

    for (int i = 0; i < 32; i++)
        secretHash[i] = shaVec[i]; 

    // Step 2: Split the 32-byte secret hash into two 16-byte parts
    memcpy(K1, secretHash, 16);        // First half: K1
    memcpy(K12, secretHash + 16, 16);  // Second half: K12

    // Step 3: Hash the salt using MD5 to get a 16-byte value
    // The salt makes each key unique even with the same shared secret
    std::string md5Hex = md5_process((uint8_t*)salt, saltLen); 
    hexToBytes(md5Hex, K2, 16);  // Convert hex string to bytes

    // Step 4: XOR K1 with K2 to produce K3
    for (int i = 0; i < 16; i++)
        K3[i] = (uint8_t)(K1[i] ^ K2[i]); 

    // Step 5: Concatenate K3 || K12 to form the final 32-byte AES-256 key
    // Final DPK = K3 (16 bytes) || K12 (16 bytes) = 32 bytes total
    memcpy(key, K3, 16);           // First 16 bytes: K3
    memcpy(key + 16, K12, 16);     // Last 16 bytes: K12

}

