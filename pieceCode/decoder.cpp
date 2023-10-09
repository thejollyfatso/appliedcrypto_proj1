#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

const string encryptMessage(const string& message, const string& key);
char encryptCharacter(char msgChar, char keyChar);
const string decryptMessage(const string& ciphertext, const string& key);
char decryptCharacter(char cipherChar, char keyChar);

int main()
{
  ifstream ifs("ciphertext.txt");
  if (!ifs)
  {
    cerr << "Could not open the file.\n";
    exit(1);
  }

  string message;
  string ciphertext;
  string key = "abc";

  getline(ifs, ciphertext);
  message = decryptMessage(ciphertext, key);

  // Check outputs
  cout << "Plaintext:  " << message << endl;
  cout << "Ciphertext:  " << ciphertext << endl;
  return 0;
}

const string decryptMessage(const string& ciphertext, const string& key)
{
  // decrypt each character in the message
  string message;
  message.assign(ciphertext);
  for (size_t i = 0; i < message.size(); ++i)
  {
    message[i] = decryptCharacter(ciphertext[i], key[i % key.size()]);
  }
  return message;
}

char decryptCharacter(char cipherChar, char keyChar)
{
  string messageSpace = " abcdefghijklmnopqrstuvwxyz";
  char decryptedChar;
  int shiftDistance;

  // Find shift distance - this is the index of the Key character within the Message Space
  for (size_t i = 0; i < messageSpace.size(); ++i)
  {
    if (messageSpace[i] == keyChar)
    {
      shiftDistance = i;
      break;
    }
  }
  
  // Find index of the Cipher Char within the Message Space
  int posCipherChar = (cipherChar == ' ') ? 0 : (int(cipherChar) - 'a' + 1);

  // shift the cipher char by shift distance
  int shift = posCipherChar - shiftDistance;
  decryptedChar = ((posCipherChar - shiftDistance) % messageSpace.size()) + ('a' - 1);
  if (int(decryptedChar) == 96) decryptedChar = ' ';
  if (shift < 0)
  {
    decryptedChar = 'z' + 1 + shift;
  }

  /*
  cout << "Cipher Char:  " << cipherChar << endl;
  cout << "ASCII / int(cipherChar):  " << int(cipherChar) << endl;
  cout << "Key Char:  " << keyChar << endl;
  cout << "posCipherChar:  " << posCipherChar << endl;
  cout << "Shift Distance: " << shiftDistance << endl;
  cout << "Shift Cipher Char by Shift Distance:  " << posCipherChar - shiftDistance << endl;
  cout << "Modulo:  " << (posCipherChar - shiftDistance) % messageSpace.size() << endl;
  cout << "decryptedChar Char int:  " << int(decryptedChar) << endl;
  cout << "----" << endl;
  */
 
  return decryptedChar;
}

