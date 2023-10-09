/*
  This code follows findingCoincidences.cpp
  Given the same cipher text and the guessed key length,
  frequency analysis will determine the most likely key.
*/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

// Functions
string genShiftedString (const string ogCipher, const int shift);
int countCoincidences (const string ogCipher, const string cmpString, const int shift);
vector<int> genKeyLengthGuesses(const vector<int> coincidenceCount, const int maxLength);

int main()
{
  // read in file
  ifstream ifs("cipher.txt");
  string cText( ( istreambuf_iterator<char>(ifs) ),
                ( istreambuf_iterator<char>() ) );

  // for length of string, count coincidences of resulting shift of i
  vector<int> coincidences;
  for ( int i = 0; i < cText.length(); i++ )
  {
    string shiftedString = genShiftedString(cText, i);
    coincidences.push_back(countCoincidences(cText, shiftedString, i));
  }

  // generate key length guesses based on coincidences and factors
  vector<int> keyLengthCandidates = genKeyLengthGuesses(coincidences, cText.length());

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

vector<int> genKeyLengthGuesses(const vector<int> coincidenceCount, const int maxLength)
{
  vector<int> candidateGuesses;
  vector<int> guesses;
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
      candidateGuesses.push_back(guess);
      mean = totalCoincidences/denominator;
    }
  }

  vector<int> factors(candidateGuesses.back()+1);

  for ( int i = 0; i < candidateGuesses.size(); i++ )
  {
    for ( int j = 2; j < candidateGuesses[i]; j++)  // keep note of j set to 2
    {
      if ( candidateGuesses[i] % j == 0) factors[j] += 1;
    }
  }

  for ( int i = factors.size(); i > 1; i-- )
  {
    if ( factors[i] > factors[guess] ) 
    {
      guess = i;
      guesses.push_back(i);
    }
  }

  return guesses;
}
