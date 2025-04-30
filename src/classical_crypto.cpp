#include "classical_crypto.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstring>
#include <random>
#include <fstream>
#include <ctime>

namespace crypto {
namespace classical {

// Simple AES-like encryption (simplified for demo purposes, NOT real AES)
std::vector<unsigned char> aes_encrypt(const std::string& plaintext, const std::string& key, const std::string& iv) {
    std::vector<unsigned char> ciphertext;
    
    // Ensure key and IV are of appropriate length
    if (key.length() < 16 || iv.length() < 16) {
        return ciphertext;
    }
    
    // Simple implementation for demo purposes (not real AES)
    // VULNERABILITY: Using a simplified XOR-based encryption as replacement
    
    // Reserve space for cipher text (same as plain text for this simple implementation)
    ciphertext.resize(plaintext.size());
    
    // XOR-based encryption with key and IV
    for (size_t i = 0; i < plaintext.size(); ++i) {
        // Use key and IV in a simple way (NOT secure, just for demo)
        unsigned char k = key[i % key.length()];
        unsigned char v = iv[i % iv.length()];
        ciphertext[i] = plaintext[i] ^ k ^ v;
    }
    
    return ciphertext;
}

// Simple AES-like decryption (simplified for demo purposes, NOT real AES)
std::string aes_decrypt(const std::vector<unsigned char>& ciphertext, const std::string& key, const std::string& iv) {
    std::vector<unsigned char> plaintext;
    
    // Ensure key and IV are of appropriate length
    if (key.length() < 16 || iv.length() < 16) {
        return "";
    }
    
    // Reserve space for plain text
    plaintext.resize(ciphertext.size());
    
    // XOR-based decryption (same as encryption for XOR)
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        unsigned char k = key[i % key.length()];
        unsigned char v = iv[i % iv.length()];
        plaintext[i] = ciphertext[i] ^ k ^ v;
    }
    
    return std::string(plaintext.begin(), plaintext.end());
}

// Simplified RSA implementation (for demo only - NOT real RSA)
// This is a very simple and insecure simulation of RSA concepts

// Generate simple key pair with modular arithmetic (NOT real RSA)
bool generate_rsa_key_pair(const std::string& public_key_file, const std::string& private_key_file, int key_size) {
    // VULNERABILITY: Ignoring key_size parameter in this simplified version

    // Generate two "prime" numbers (not actually prime, just for demo)
    unsigned long p = 61; // Small prime for demo
    unsigned long q = 53; // Small prime for demo
    unsigned long n = p * q;                  // modulus
    unsigned long phi = (p - 1) * (q - 1);    // Euler's totient
    
    // Choose public exponent (commonly 65537 in real RSA)
    unsigned long e = 17;
    
    // Calculate private exponent (simplified)
    unsigned long d = 0;
    for (unsigned long i = 1; i < phi; i++) {
        if ((i * e) % phi == 1) {
            d = i;
            break;
        }
    }
    
    // Store keys in files
    std::ofstream public_key(public_key_file);
    if (!public_key.is_open()) {
        return false;
    }
    public_key << "RSA PUBLIC KEY SIMULATION\n";
    public_key << "n=" << n << "\n";
    public_key << "e=" << e << "\n";
    public_key.close();
    
    std::ofstream private_key(private_key_file);
    if (!private_key.is_open()) {
        return false;
    }
    private_key << "RSA PRIVATE KEY SIMULATION\n";
    private_key << "n=" << n << "\n";
    private_key << "d=" << d << "\n";
    private_key.close();
    
    return true;
}

// Modular exponentiation for simple RSA operations
unsigned long mod_pow(unsigned long base, unsigned long exponent, unsigned long modulus) {
    if (modulus == 1) return 0;
    unsigned long result = 1;
    base = base % modulus;
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = (result * base) % modulus;
        }
        exponent = exponent >> 1;
        base = (base * base) % modulus;
    }
    return result;
}

// Simple RSA encryption (very simplified, just for demo)
std::vector<unsigned char> rsa_encrypt(const std::string& plaintext, const std::string& public_key_file) {
    std::vector<unsigned char> ciphertext;
    
    // Read public key
    std::ifstream key_file(public_key_file);
    if (!key_file.is_open()) {
        return ciphertext;
    }
    
    std::string line;
    unsigned long n = 0, e = 0;
    
    // Skip header
    std::getline(key_file, line);
    
    // Read n
    std::getline(key_file, line);
    if (line.substr(0, 2) == "n=") {
        n = std::stoul(line.substr(2));
    }
    
    // Read e
    std::getline(key_file, line);
    if (line.substr(0, 2) == "e=") {
        e = std::stoul(line.substr(2));
    }
    
    key_file.close();
    
    if (n == 0 || e == 0) {
        return ciphertext;
    }
    
    // Encrypt each character separately (very simplified)
    for (char c : plaintext) {
        unsigned long m = static_cast<unsigned char>(c);
        unsigned long encrypted = mod_pow(m, e, n);
        
        // Store the encrypted value (4 bytes per char)
        ciphertext.push_back((encrypted >> 24) & 0xFF);
        ciphertext.push_back((encrypted >> 16) & 0xFF);
        ciphertext.push_back((encrypted >> 8) & 0xFF);
        ciphertext.push_back(encrypted & 0xFF);
    }
    
    return ciphertext;
}

// Simple RSA decryption (very simplified, just for demo)
std::string rsa_decrypt(const std::vector<unsigned char>& ciphertext, const std::string& private_key_file) {
    std::string plaintext;
    
    // Read private key
    std::ifstream key_file(private_key_file);
    if (!key_file.is_open()) {
        return plaintext;
    }
    
    std::string line;
    unsigned long n = 0, d = 0;
    
    // Skip header
    std::getline(key_file, line);
    
    // Read n
    std::getline(key_file, line);
    if (line.substr(0, 2) == "n=") {
        n = std::stoul(line.substr(2));
    }
    
    // Read d
    std::getline(key_file, line);
    if (line.substr(0, 2) == "d=") {
        d = std::stoul(line.substr(2));
    }
    
    key_file.close();
    
    if (n == 0 || d == 0) {
        return plaintext;
    }
    
    // Process ciphertext in blocks of 4 bytes
    for (size_t i = 0; i < ciphertext.size(); i += 4) {
        if (i + 3 >= ciphertext.size()) break;
        
        unsigned long encrypted = 0;
        encrypted |= static_cast<unsigned long>(ciphertext[i]) << 24;
        encrypted |= static_cast<unsigned long>(ciphertext[i+1]) << 16;
        encrypted |= static_cast<unsigned long>(ciphertext[i+2]) << 8;
        encrypted |= static_cast<unsigned long>(ciphertext[i+3]);
        
        unsigned long decrypted = mod_pow(encrypted, d, n);
        plaintext.push_back(static_cast<char>(decrypted));
    }
    
    return plaintext;
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