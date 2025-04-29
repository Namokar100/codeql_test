#include "classical_crypto.h"
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstring>

namespace crypto {
namespace classical {

// AES encryption with vulnerabilities
std::vector<unsigned char> aes_encrypt(const std::string& plaintext, const std::string& key, const std::string& iv) {
    std::vector<unsigned char> ciphertext;
    
    // Create context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return ciphertext;
    }
    
    // Initialize encryption operation
    // VULNERABILITY: Using AES-128-CBC instead of AES-256-GCM (weaker encryption)
    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, 
                          reinterpret_cast<const unsigned char*>(key.c_str()), 
                          reinterpret_cast<const unsigned char*>(iv.c_str())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return ciphertext;
    }
    
    // Allocate memory for ciphertext
    ciphertext.resize(plaintext.size() + EVP_CIPHER_CTX_block_size(ctx));
    int len = 0;
    int ciphertext_len = 0;
    
    // Encrypt
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, 
                         reinterpret_cast<const unsigned char*>(plaintext.c_str()), 
                         static_cast<int>(plaintext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<unsigned char>();
    }
    ciphertext_len = len;
    
    // Finalize encryption
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::vector<unsigned char>();
    }
    ciphertext_len += len;
    
    // Resize to actual ciphertext length
    ciphertext.resize(ciphertext_len);
    
    // VULNERABILITY: Not checking OpenSSL error stack
    
    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    
    return ciphertext;
}

// AES decryption
std::string aes_decrypt(const std::vector<unsigned char>& ciphertext, const std::string& key, const std::string& iv) {
    std::vector<unsigned char> plaintext;
    
    // Create context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return "";
    }
    
    // Initialize decryption operation
    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, 
                          reinterpret_cast<const unsigned char*>(key.c_str()), 
                          reinterpret_cast<const unsigned char*>(iv.c_str())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    
    // Allocate memory for plaintext
    plaintext.resize(ciphertext.size() + EVP_CIPHER_CTX_block_size(ctx));
    int len = 0;
    int plaintext_len = 0;
    
    // Decrypt
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, 
                         ciphertext.data(), 
                         static_cast<int>(ciphertext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len = len;
    
    // Finalize decryption
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len += len;
    
    // Resize to actual plaintext length
    plaintext.resize(plaintext_len);
    
    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    
    return std::string(plaintext.begin(), plaintext.end());
}

// RSA encryption
std::vector<unsigned char> rsa_encrypt(const std::string& plaintext, const std::string& public_key_file) {
    std::vector<unsigned char> ciphertext;
    
    FILE* fp = fopen(public_key_file.c_str(), "r");
    if (!fp) {
        return ciphertext;
    }
    
    RSA* rsa = PEM_read_RSA_PUBKEY(fp, nullptr, nullptr, nullptr);
    fclose(fp);
    
    if (!rsa) {
        return ciphertext;
    }
    
    // Allocate memory for ciphertext
    int rsa_size = RSA_size(rsa);
    ciphertext.resize(rsa_size);
    
    // VULNERABILITY: Using RSA_PKCS1_PADDING instead of RSA_PKCS1_OAEP_PADDING (less secure)
    int result = RSA_public_encrypt(
        static_cast<int>(plaintext.size()),
        reinterpret_cast<const unsigned char*>(plaintext.c_str()),
        ciphertext.data(),
        rsa,
        RSA_PKCS1_PADDING
    );
    
    RSA_free(rsa);
    
    if (result == -1) {
        return std::vector<unsigned char>();
    }
    
    ciphertext.resize(result);
    return ciphertext;
}

// RSA decryption
std::string rsa_decrypt(const std::vector<unsigned char>& ciphertext, const std::string& private_key_file) {
    std::vector<unsigned char> plaintext;
    
    FILE* fp = fopen(private_key_file.c_str(), "r");
    if (!fp) {
        return "";
    }
    
    RSA* rsa = PEM_read_RSAPrivateKey(fp, nullptr, nullptr, nullptr);
    fclose(fp);
    
    if (!rsa) {
        return "";
    }
    
    // Allocate memory for plaintext
    int rsa_size = RSA_size(rsa);
    plaintext.resize(rsa_size);
    
    int result = RSA_private_decrypt(
        static_cast<int>(ciphertext.size()),
        ciphertext.data(),
        plaintext.data(),
        rsa,
        RSA_PKCS1_PADDING
    );
    
    RSA_free(rsa);
    
    if (result == -1) {
        return "";
    }
    
    plaintext.resize(result);
    return std::string(plaintext.begin(), plaintext.end());
}

// Generate RSA key pair
bool generate_rsa_key_pair(const std::string& public_key_file, const std::string& private_key_file, int key_size) {
    // VULNERABILITY: Key size can be too small if caller specifies it
    RSA* rsa = RSA_new();
    BIGNUM* bn = BN_new();
    
    // Set public exponent to 65537
    if (BN_set_word(bn, RSA_F4) != 1) {
        BN_free(bn);
        RSA_free(rsa);
        return false;
    }
    
    // Generate key pair
    if (RSA_generate_key_ex(rsa, key_size, bn, nullptr) != 1) {
        BN_free(bn);
        RSA_free(rsa);
        return false;
    }
    
    // Save public key
    FILE* public_fp = fopen(public_key_file.c_str(), "w");
    if (!public_fp) {
        BN_free(bn);
        RSA_free(rsa);
        return false;
    }
    
    if (PEM_write_RSA_PUBKEY(public_fp, rsa) != 1) {
        fclose(public_fp);
        BN_free(bn);
        RSA_free(rsa);
        return false;
    }
    
    fclose(public_fp);
    
    // Save private key
    // VULNERABILITY: Not using encryption for the private key file
    FILE* private_fp = fopen(private_key_file.c_str(), "w");
    if (!private_fp) {
        BN_free(bn);
        RSA_free(rsa);
        return false;
    }
    
    if (PEM_write_RSAPrivateKey(private_fp, rsa, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
        fclose(private_fp);
        BN_free(bn);
        RSA_free(rsa);
        return false;
    }
    
    fclose(private_fp);
    BN_free(bn);
    RSA_free(rsa);
    
    return true;
}

// Utility function to convert bytes to hex
std::string bytes_to_hex(const std::vector<unsigned char>& data) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (const auto& byte : data) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

// Utility function to convert hex to bytes
std::vector<unsigned char> hex_to_bytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }
    
    return bytes;
}

} // namespace classical
} // namespace crypto 