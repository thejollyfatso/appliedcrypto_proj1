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
  // read in file
  ifstream cTextStream("dummy.txt");
  string cText( ( istreambuf_iterator<char>(cTextStream) ),
                ( istreambuf_iterator<char>() ) );

  // for length of string, count coincidences of resulting shift of i
  vector<int> coincidences;
  for ( int i = 0; i < cText.length(); i++ )
  {
    string shiftedString = genShiftedString(cText, i);
    coincidences.push_back(countCoincidences(cText, shiftedString, i));
  }

  // DEBUG: print resulting coincidence vector
  for ( int i = 0; i < coincidences.size(); i++ )
  {
    cout << coincidences[i] << " ";
  }
  cout << endl;

  return 0;
}

string genShiftedString (const string ogCipher, const int shift)
{
  string shiftString;
  for ( int i = 0; i < ogCipher.length() - shift; i++ )
  {
    shiftString += ogCipher[i];
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
