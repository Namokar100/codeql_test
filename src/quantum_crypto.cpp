#include "quantum_crypto.h"
#include "classical_crypto.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <cstring>

namespace crypto {
namespace quantum {

// Simulate Quantum Key Distribution (BB84 Protocol)
std::vector<unsigned char> generate_qkd_key(size_t key_length, double error_rate) {
    std::vector<unsigned char> key;
    key.resize(key_length);
    
    // Initialize random generators
    // VULNERABILITY: Using a deterministic seed
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<int> dist(0, 255);
    
    // Generate random bits
    for (size_t i = 0; i < key_length; ++i) {
        key[i] = static_cast<unsigned char>(dist(rng));
    }
    
    // Simulate quantum errors (if specified)
    if (error_rate > 0.0) {
        std::bernoulli_distribution error_dist(error_rate);
        for (size_t i = 0; i < key_length; ++i) {
            if (error_dist(rng)) {
                // Flip a random bit in the byte
                key[i] ^= (1 << (dist(rng) % 8));
            }
        }
    }
    
    return key;
}

// Simulate lattice-based encryption (for demonstration, not real lattice-based)
std::vector<unsigned char> lattice_encrypt(const std::string& plaintext, const std::vector<unsigned char>& public_key) {
    std::vector<unsigned char> ciphertext;
    
    // For simulation, we'll use the public key as a seed for some randomness
    std::mt19937 rng(0); // VULNERABILITY: Using a fixed seed
    for (const auto& byte : public_key) {
        rng.discard(byte);
    }
    
    // Add a header to indicate this is lattice-encrypted
    ciphertext.push_back('L');
    ciphertext.push_back('A');
    ciphertext.push_back('T');
    ciphertext.push_back('T');
    
    // For each byte in the plaintext, apply a simulated lattice operation
    for (const auto& byte : plaintext) {
        // Simple XOR with public key (this is a weak simulation, not real lattice crypto)
        unsigned char encrypted = byte;
        for (size_t i = 0; i < std::min(size_t(8), public_key.size()); ++i) {
            encrypted ^= public_key[i % public_key.size()];
            encrypted = (encrypted << 1) | (encrypted >> 7); // Rotate bits
        }
        ciphertext.push_back(encrypted);
    }
    
    // Add some "noise" to simulate lattice properties
    std::uniform_int_distribution<int> dist(0, 255);
    for (size_t i = 0; i < 16; ++i) {
        ciphertext.push_back(static_cast<unsigned char>(dist(rng)));
    }
    
    return ciphertext;
}

// Simulate lattice-based decryption
std::string lattice_decrypt(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& private_key) {
    std::string plaintext;
    
    // Check if this is a valid lattice-encrypted message
    if (ciphertext.size() < 20 || ciphertext[0] != 'L' || ciphertext[1] != 'A' || 
        ciphertext[2] != 'T' || ciphertext[3] != 'T') {
        return "Invalid lattice encryption format";
    }
    
    // Set up the same random process using the private key
    std::mt19937 rng(0); // VULNERABILITY: Using a fixed seed
    for (const auto& byte : private_key) {
        rng.discard(byte);
    }
    
    // Decrypt each byte (excluding header and trailing noise)
    for (size_t i = 4; i < ciphertext.size() - 16; ++i) {
        unsigned char decrypted = ciphertext[i];
        // Apply the inverse of the encryption operation
        for (size_t j = 0; j < std::min(size_t(8), private_key.size()); ++j) {
            // Apply operations in reverse order
            decrypted = (decrypted >> 1) | (decrypted << 7); // Rotate bits in opposite direction
            decrypted ^= private_key[j % private_key.size()];
        }
        plaintext.push_back(decrypted);
    }
    
    return plaintext;
}

// Generate a simulated lattice key pair
bool generate_lattice_key_pair(std::vector<unsigned char>& public_key, std::vector<unsigned char>& private_key, int security_level) {
    // security_level is nominal: 1=weak, 2=medium, 3=strong
    
    // VULNERABILITY: Security level is inadequate for real-world use
    const size_t key_size = 16 * security_level; // Too small for real lattice security
    
    // Clear and resize keys
    public_key.clear();
    private_key.clear();
    public_key.resize(key_size);
    private_key.resize(key_size);
    
    // Use random generator with time-based seed
    // VULNERABILITY: Weak random number generation
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<int> dist(0, 255);
    
    // Generate private key with random bytes
    for (size_t i = 0; i < key_size; ++i) {
        private_key[i] = static_cast<unsigned char>(dist(rng));
    }
    
    // Generate public key by applying a transformation to the private key
    // In a real lattice scheme, this would involve complex mathematical operations
    for (size_t i = 0; i < key_size; ++i) {
        // Simple transformation for demonstration
        public_key[i] = private_key[i] ^ static_cast<unsigned char>((i * 7) % 256);
    }
    
    return true;
}

// Hybrid encryption (using both classical RSA and simulated quantum-resistant lattice)
std::vector<unsigned char> hybrid_encrypt(const std::string& plaintext, 
                                      const std::string& rsa_public_key_file,
                                      const std::vector<unsigned char>& lattice_public_key) {
    std::vector<unsigned char> result;
    
    // First, generate a random session key
    std::vector<unsigned char> session_key = generate_qkd_key(32, 0.0);
    
    // Encrypt session key with RSA
    std::string session_key_str(session_key.begin(), session_key.end());
    std::vector<unsigned char> encrypted_session_key = crypto::classical::rsa_encrypt(
        session_key_str, rsa_public_key_file);
    
    // Also encrypt session key with lattice-based encryption for quantum resistance
    std::vector<unsigned char> lattice_encrypted_key = lattice_encrypt(
        session_key_str, lattice_public_key);
    
    // Store the sizes of each component
    uint32_t rsa_size = static_cast<uint32_t>(encrypted_session_key.size());
    uint32_t lattice_size = static_cast<uint32_t>(lattice_encrypted_key.size());
    
    // Write a header for the hybrid format
    result.push_back('H');
    result.push_back('Y');
    result.push_back('B');
    result.push_back('R');
    
    // Add the sizes
    result.insert(result.end(), 
        reinterpret_cast<unsigned char*>(&rsa_size),
        reinterpret_cast<unsigned char*>(&rsa_size) + sizeof(rsa_size));
    
    result.insert(result.end(), 
        reinterpret_cast<unsigned char*>(&lattice_size),
        reinterpret_cast<unsigned char*>(&lattice_size) + sizeof(lattice_size));
    
    // Add the encrypted keys
    result.insert(result.end(), encrypted_session_key.begin(), encrypted_session_key.end());
    result.insert(result.end(), lattice_encrypted_key.begin(), lattice_encrypted_key.end());
    
    // Use session key to encrypt the actual message
    // VULNERABILITY: Using a hardcoded IV
    std::string iv = "0123456789012345"; // 16 bytes for AES
    std::vector<unsigned char> encrypted_message = crypto::classical::aes_encrypt(
        plaintext, std::string(session_key.begin(), session_key.end()), iv);
    
    // Add the encrypted message
    result.insert(result.end(), encrypted_message.begin(), encrypted_message.end());
    
    return result;
}

// Hybrid decryption
std::string hybrid_decrypt(const std::vector<unsigned char>& ciphertext,
                       const std::string& rsa_private_key_file,
                       const std::vector<unsigned char>& lattice_private_key) {
    // Check if this is a valid hybrid encrypted message
    if (ciphertext.size() < 12 || ciphertext[0] != 'H' || ciphertext[1] != 'Y' || 
        ciphertext[2] != 'B' || ciphertext[3] != 'R') {
        return "Invalid hybrid encryption format";
    }
    
    // Extract the sizes
    uint32_t rsa_size, lattice_size;
    std::memcpy(&rsa_size, &ciphertext[4], sizeof(rsa_size));
    std::memcpy(&lattice_size, &ciphertext[8], sizeof(lattice_size));
    
    size_t header_size = 12; // 'HYBR' + two uint32_t values
    
    // Ensure there's enough data
    if (ciphertext.size() < header_size + rsa_size + lattice_size) {
        return "Corrupted data: insufficient size";
    }
    
    // Extract the encrypted session keys
    std::vector<unsigned char> encrypted_rsa_key(
        ciphertext.begin() + header_size,
        ciphertext.begin() + header_size + rsa_size);
    
    std::vector<unsigned char> encrypted_lattice_key(
        ciphertext.begin() + header_size + rsa_size,
        ciphertext.begin() + header_size + rsa_size + lattice_size);
    
    // Determine which key to use for decryption (try both, prefer RSA)
    std::string session_key_str;
    bool rsa_success = false;
    
    // Try to decrypt with RSA
    std::string rsa_decrypted = crypto::classical::rsa_decrypt(
        encrypted_rsa_key, rsa_private_key_file);
    
    if (!rsa_decrypted.empty()) {
        session_key_str = rsa_decrypted;
        rsa_success = true;
    }
    
    // If RSA failed, try lattice
    if (!rsa_success) {
        std::string lattice_decrypted = lattice_decrypt(
            encrypted_lattice_key, lattice_private_key);
        
        if (!lattice_decrypted.empty()) {
            session_key_str = lattice_decrypted;
        } else {
            return "Failed to decrypt session key";
        }
    }
    
    // Extract the encrypted message
    std::vector<unsigned char> encrypted_message(
        ciphertext.begin() + header_size + rsa_size + lattice_size,
        ciphertext.end());
    
    // Decrypt the message with the session key
    // VULNERABILITY: Using a hardcoded IV
    std::string iv = "0123456789012345"; // 16 bytes for AES
    std::string plaintext = crypto::classical::aes_decrypt(
        encrypted_message, session_key_str, iv);
    
    return plaintext;
}

} // namespace quantum
} // namespace crypto 