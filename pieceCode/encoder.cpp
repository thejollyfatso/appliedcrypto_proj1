#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

const string encryptMessage(const string& message, const string& key);
char encryptCharacter(char msgChar, char keyChar);

int main()
{
  ifstream ifs("plaintext.txt");
  if (!ifs)
  {
    cerr << "Could not open the file.\n";
    exit(1);
  }

  string message;
  string ciphertext;
  string key = "abc";

  getline(ifs, message);
  ciphertext = encryptMessage(message, key);

  // Check outputs
  cout << "Plaintext:  " << message << endl;
  cout << "Ciphertext:  " << ciphertext << endl;
  return 0;
}

const string encryptMessage(const string& message, const string& key)
{
  // encrypt each character in the message
  string ciphertext;
  ciphertext.assign(message);
  for (size_t i = 0; i < ciphertext.size(); ++i)
  {
    ciphertext[i] = encryptCharacter(message[i], key[i % key.size()]);
  }

 return ciphertext;
}

char encryptCharacter(char msgChar, char keyChar)
{
  string messageSpace = " abcdefghijklmnopqrstuvwxyz";
  char encryptedChar;
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

  // Find index of the Message Char within the Message Space
  int posMsgChar = (msgChar == ' ') ? 0 : (int(msgChar) - 'a' + 1);

  // Shift the Message Char by Shift Distance
  encryptedChar = ((posMsgChar + shiftDistance) % messageSpace.size()) + ('a' - 1);
  
  // account for ' '
  if (int(encryptedChar) == 96) encryptedChar = ' ';

  /*
  cout << "Message Char:  " << msgChar << endl;
  cout << "ASCII / int(msgChar):  " << int(msgChar) << endl;
  cout << "Key Char:  " << keyChar << endl;
  cout << "posMsgChar:  " << posMsgChar << endl;
  cout << "Shift Distance: " << shiftDistance << endl;
  cout << "Shift Message Char by Shift Distance:  " << posMsgChar + shiftDistance << endl;
  cout << "Encrypted Char:  " << encryptedChar << endl;
  cout << "----" << endl;
  */
 
  return encryptedChar;
}

