#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

// Function to decrypt the ciphertext using a given key
std::string decrypt(const std::string& ciphertext, const std::string& key) {
    std::string plaintext = "";
    int key_length = key.length();
    int ciphertext_length = ciphertext.length();
    
    for (int i = 0; i < ciphertext_length; ++i) {
        char ciphertext_char = ciphertext[i];
        char key_char = key[i % key_length];
        
        // Decrypt the character using the key
        char decrypted_char;
        if (ciphertext_char == ' ') {
            decrypted_char = ' ';
        } else {
            decrypted_char = 'a' + ((ciphertext_char - key_char + 26) % 26);
        }
        
        plaintext += decrypted_char;
    }
    
    return plaintext;
}

// Function to calculate the score of a plaintext based on word frequency
int scorePlaintext(const std::string& plaintext, const std::vector<std::string>& dictionary) {
    int score = 0;
    std::string word = "";
    
    for (char c : plaintext) {
        if (c == ' ') {
            // Check if the word is in the dictionary
            if (std::find(dictionary.begin(), dictionary.end(), word) != dictionary.end()) {
                score++;
            }
            word = "";
        } else {
            word += c;
        }
    }
    
    return score;
}

int main() {
    // Read the ciphertext from the user
    std::cout << "Enter the ciphertext: ";
    std::string ciphertext;
    std::getline(std::cin, ciphertext);
    
    // Load the dictionary into memory
    std::ifstream dictionaryFile("plaintext_dictionary_test1.txt");
    std::vector<std::string> dictionary;
    std::string word;
    while (dictionaryFile >> word) {
        dictionary.push_back(word);
    }
    dictionaryFile.close();
    
    std::string bestGuess;
    int bestScore = 0;
    
    // Try all possible keys (key space: {0,..,26}^t)
    for (int t = 1; t <= ciphertext.length(); ++t) {
        for (int key = 1; key <= t; ++key) {
            std::string keyStr;
            
            // Convert key number to base-27 string
            int tmpKey = key;
            while (tmpKey > 0) {
                char keyChar = 'a' + (tmpKey % 27);
                keyStr = keyChar + keyStr;
                tmpKey /= 27;
            }
            
            // Decrypt the ciphertext using the current key
            std::string plaintext = decrypt(ciphertext, keyStr);
            if (bestGuess.empty()) { bestGuess = plaintext; }
            
            // Score the plaintext
            int score = scorePlaintext(plaintext, dictionary);
            
            // Update the best guess if this is a better score
            if (score > bestScore) {
                bestGuess = plaintext;
                bestScore = score;
            }
        }
    }
    
    // Output the best guess
    std::cout << "My plaintext guess is: " << bestGuess << std::endl;
    
    return 0;
}
