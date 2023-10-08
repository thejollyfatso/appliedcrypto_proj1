/*
  This code is used to guess the length of the key.
  Given a cipher text, it will generate a cascading list and calculate coincidences.
  Key length will be guessed based on spacing between outliers.
*/
#include <iostream>
#include <vector>
#include <string>

using namespace std

// Functions
string genShiftedString (string ogCipher, int shift);
int countCoincidences (string ogCipher, string cmpString);

int main()
{
  return 0;
}

string genShiftedString (string ogCipher, int shift)
{
  string shiftString;
  int j = 0;
  for ( int i = shift; i < ogCipher.length(); i++ )
  {
    shiftString[j++] = ogCipher[i];
  }
  return shiftString;
}

int countCoincidences (string ogCipher, string cmpString)
{
  int cnt = 0;
  for ( int i = 0; i < cmpString.length(); i++ )
  {
    if ogCipher[i] == cmpString[i] cnt++;
  }
  return cnt;
}
