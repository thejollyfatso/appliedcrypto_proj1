/*
  This code is used to guess the length of the key.
  Given a cipher text, it will generate a cascading list and calculate coincidences.
  Key length will be guessed based on spacing between outliers.
*/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

// Functions
string genShiftedString (const string ogCipher, const int shift);
int countCoincidences (const string ogCipher, const string cmpString, const int shift);

int main()
{
  ifstream cTextStream("dummy.txt");
  string cText( ( istreambuf_iterator<char>(cTextStream) ),
                ( istreambuf_iterator<char>() ) );
  cout << cText << endl;
  return 0;
}

string genShiftedString (const string ogCipher, const int shift)
{
  string shiftString;
  for ( int i = 0; i < ogCipher.length() - shift; i++ )
  {
    shiftString[i] = ogCipher[i];
  }
  return shiftString;
}

int countCoincidences (const string ogCipher, const string cmpString, const int shift)
{
  int cnt = 0;
  int j = shift;
  for ( int i = 0; i < cmpString.length(); i++ )
  {
    if ( cmpString[i] == ogCipher[j++] ) cnt++;
  }
  return cnt;
}
