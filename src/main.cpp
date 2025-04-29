#include "classical_crypto.h"
#include "quantum_crypto.h"
#include "vulnerable_crypto.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

// Function to demonstrate classical crypto
void demonstrate_classical_crypto() {
    std::cout << "\n=== Classical Cryptography Demonstration ===\n" << std::endl;
    
    // Example message
    std::string message = "This is a secret message for classical encryption.";
    std::cout << "Original message: " << message << std::endl;
    
    // AES encryption/decryption
    std::string key = "AES128BitKey1234"; // 16 bytes for AES-128
    std::string iv = "RandomIV12345678";  // 16 bytes IV
    
    std::cout << "\nPerforming AES encryption..." << std::endl;
    std::vector<unsigned char> encrypted = crypto::classical::aes_encrypt(message, key, iv);
    std::cout << "Encrypted (hex): " << crypto::classical::bytes_to_hex(encrypted) << std::endl;
    
    std::string decrypted = crypto::classical::aes_decrypt(encrypted, key, iv);
    std::cout << "Decrypted: " << decrypted << std::endl;
    
    // Generate RSA key pair
    std::cout << "\nGenerating RSA key pair..." << std::endl;
    std::string public_key_file = "rsa_public.pem";
    std::string private_key_file = "rsa_private.pem";
    
    // VULNERABILITY: Key size of 1024 is considered weak by modern standards
    if (crypto::classical::generate_rsa_key_pair(public_key_file, private_key_file, 1024)) {
        std::cout << "RSA key pair generated successfully." << std::endl;
        
        // RSA encryption/decryption
        std::string rsa_message = "This is a message for RSA encryption.";
        std::cout << "\nOriginal message for RSA: " << rsa_message << std::endl;
        
        std::vector<unsigned char> rsa_encrypted = crypto::classical::rsa_encrypt(rsa_message, public_key_file);
        std::cout << "RSA encrypted (hex): " << crypto::classical::bytes_to_hex(rsa_encrypted) << std::endl;
        
        std::string rsa_decrypted = crypto::classical::rsa_decrypt(rsa_encrypted, private_key_file);
        std::cout << "RSA decrypted: " << rsa_decrypted << std::endl;
    } else {
        std::cerr << "Failed to generate RSA key pair." << std::endl;
    }
}

// Function to demonstrate quantum crypto simulations
void demonstrate_quantum_crypto() {
    std::cout << "\n=== Quantum Cryptography Simulation ===\n" << std::endl;
    
    // QKD key generation
    std::cout << "Generating quantum key distribution (QKD) key..." << std::endl;
    std::vector<unsigned char> qkd_key = crypto::quantum::generate_qkd_key(32);
    std::cout << "QKD key (hex): " << crypto::classical::bytes_to_hex(qkd_key) << std::endl;
    
    // Generate lattice key pair
    std::cout << "\nGenerating lattice-based key pair..." << std::endl;
    std::vector<unsigned char> lattice_public_key, lattice_private_key;
    if (crypto::quantum::generate_lattice_key_pair(lattice_public_key, lattice_private_key, 3)) {
        std::cout << "Lattice key pair generated successfully." << std::endl;
        std::cout << "Public key size: " << lattice_public_key.size() << " bytes" << std::endl;
        std::cout << "Private key size: " << lattice_private_key.size() << " bytes" << std::endl;
        
        // Lattice encryption
        std::string message = "This is a message for post-quantum lattice encryption.";
        std::cout << "\nOriginal message: " << message << std::endl;
        
        std::vector<unsigned char> lattice_encrypted = crypto::quantum::lattice_encrypt(message, lattice_public_key);
        std::cout << "Lattice encrypted (hex): " << crypto::classical::bytes_to_hex(lattice_encrypted) << std::endl;
        
        // Lattice decryption
        std::string lattice_decrypted = crypto::quantum::lattice_decrypt(lattice_encrypted, lattice_private_key);
        std::cout << "Lattice decrypted: " << lattice_decrypted << std::endl;
        
        // Hybrid encryption (classical + quantum)
        std::cout << "\nPerforming hybrid encryption (RSA + Lattice)..." << std::endl;
        std::string hybrid_message = "This message is protected by hybrid encryption.";
        
        std::vector<unsigned char> hybrid_encrypted = crypto::quantum::hybrid_encrypt(
            hybrid_message, "rsa_public.pem", lattice_public_key);
        
        std::cout << "Hybrid encrypted size: " << hybrid_encrypted.size() << " bytes" << std::endl;
        
        // Hybrid decryption
        std::string hybrid_decrypted = crypto::quantum::hybrid_decrypt(
            hybrid_encrypted, "rsa_private.pem", lattice_private_key);
        
        std::cout << "Hybrid decrypted: " << hybrid_decrypted << std::endl;
    } else {
        std::cerr << "Failed to generate lattice key pair." << std::endl;
    }
}

// Function to demonstrate vulnerable crypto (INTENTIONAL VULNERABILITIES)
void demonstrate_vulnerable_crypto() {
    std::cout << "\n=== Vulnerable Cryptography Demonstration (INTENTIONAL INSECURE CODE) ===\n" << std::endl;
    
    // Hardcoded key encryption
    std::cout << "Using hardcoded encryption key (VULNERABILITY)..." << std::endl;
    std::string message = "This message is 'protected' with a hardcoded key.";
    std::vector<unsigned char> hard_encrypted = crypto::vulnerable::encrypt_with_hardcoded_key(message);
    std::string hard_decrypted = crypto::vulnerable::decrypt_with_hardcoded_key(hard_encrypted);
    std::cout << "Original: " << message << std::endl;
    std::cout << "Decrypted: " << hard_decrypted << std::endl;
    
    // Weak RSA key generation
    std::cout << "\nGenerating weak RSA key (VULNERABILITY)..." << std::endl;
    if (crypto::vulnerable::generate_weak_rsa_key("weak_public.pem", "weak_private.pem")) {
        std::cout << "Weak RSA key pair generated successfully." << std::endl;
    }
    
    // Insecure random number generation
    std::cout << "\nGenerating insecure random bytes (VULNERABILITY)..." << std::endl;
    std::vector<unsigned char> insecure_bytes = crypto::vulnerable::generate_insecure_random_bytes(16);
    std::cout << "Insecure random bytes: " << crypto::classical::bytes_to_hex(insecure_bytes) << std::endl;
    
    // ECB mode encryption
    std::cout << "\nUsing ECB mode encryption (VULNERABILITY)..." << std::endl;
    std::string ecb_message = "ECB mode does not hide data patterns properly.";
    std::string ecb_key = "ECBModeInsecure1";
    std::vector<unsigned char> ecb_encrypted = crypto::vulnerable::encrypt_ecb_mode(ecb_message, ecb_key);
    std::string ecb_decrypted = crypto::vulnerable::decrypt_ecb_mode(ecb_encrypted, ecb_key);
    std::cout << "Original: " << ecb_message << std::endl;
    std::cout << "Decrypted: " << ecb_decrypted << std::endl;
    
    // Static IV
    std::cout << "\nUsing static IV (VULNERABILITY)..." << std::endl;
    std::string static_message = "This message uses a static IV.";
    std::string static_key = "StaticIVKeyTest12";
    std::vector<unsigned char> static_encrypted = crypto::vulnerable::encrypt_with_static_iv(static_message, static_key);
    std::string static_decrypted = crypto::vulnerable::decrypt_with_static_iv(static_encrypted, static_key);
    std::cout << "Original: " << static_message << std::endl;
    std::cout << "Decrypted: " << static_decrypted << std::endl;
    
    // Timing attack vulnerability
    std::cout << "\nDemonstrating timing attack vulnerability in password comparison..." << std::endl;
    std::string correct_password = "CorrectPassword123";
    std::string wrong_password = "WrongPassword123";
    bool auth1 = crypto::vulnerable::vulnerable_password_check(correct_password, correct_password);
    bool auth2 = crypto::vulnerable::vulnerable_password_check(wrong_password, correct_password);
    std::cout << "Correct password authentication: " << (auth1 ? "Success" : "Failure") << std::endl;
    std::cout << "Wrong password authentication: " << (auth2 ? "Success" : "Failure") << std::endl;
    
    // Memory leak vulnerability
    std::cout << "\nDemonstrating memory leak with sensitive data (VULNERABILITY)..." << std::endl;
    std::string leak_message = "This function leaks the key in memory.";
    std::string leak_key = "SensitiveKeyData";
    std::vector<unsigned char> leak_encrypted = crypto::vulnerable::encrypt_with_static_iv(leak_message, leak_key);
    std::string leak_decrypted = crypto::vulnerable::decrypt_and_leak_key(leak_encrypted, leak_key);
    std::cout << "Original: " << leak_message << std::endl;
    std::cout << "Decrypted: " << leak_decrypted << std::endl;
    std::cout << "Warning: The key has been leaked in memory!" << std::endl;
}

int main() {
    std::cout << "===== Cryptography Test Program =====\n" << std::endl;
    std::cout << "This program demonstrates various cryptographic methods including" << std::endl;
    std::cout << "classical algorithms, quantum-resistant algorithms, and examples" << std::endl;
    std::cout << "of INTENTIONALLY VULNERABLE code for security analysis." << std::endl;
    
    // Demonstrate classical cryptography
    demonstrate_classical_crypto();
    
    // Demonstrate quantum cryptography
    demonstrate_quantum_crypto();
    
    // Demonstrate vulnerable cryptography (INTENTIONAL VULNERABILITIES)
    demonstrate_vulnerable_crypto();
    
    std::cout << "\n===== End of Demonstration =====\n" << std::endl;
    return 0;
} 