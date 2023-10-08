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
int guessKeyLength(const vector<int> coincidenceCount, const int maxLength);

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

  // DEBUG: print result
  cout << "key length guess:" << guessKeyLength(coincidences, 24) << endl; // BAD BAD BAD hard code numbeer go to jail
  //cout << "key length guess:" << guessKeyLength(coincidences, cText.length()) << endl;

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

int guessKeyLength(const vector<int> coincidenceCount, const int maxLength)
{
  int guess;
  int mean = 0;

  for ( int i = 1; i < maxLength; i++ )
  {
    int totalCoincidences = 0;
    int denominator = 0;
    for ( int j = 0; j < coincidenceCount.size(); j += i )
    {
      totalCoincidences += coincidenceCount[j];
      denominator++;
    }
    
    if ( mean < totalCoincidences/denominator )
    {
      guess = i;
      mean = totalCoincidences/denominator;
    }
  }

  return guess;
}
