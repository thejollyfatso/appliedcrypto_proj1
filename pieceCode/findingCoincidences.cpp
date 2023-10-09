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
int guessKeyLength(const vector<int> candidates);
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

  // DEBUG: print result
  //cout << "key length guess:" << guessKeyLength(coincidences, 24) << endl; // BAD BAD BAD hard code numbeer go to jail
  //cout << "key length guess:" << guessKeyLength(coincidences, cText.length()) << endl;
  vector<int> guess = genKeyLengthGuesses(coincidences, cText.length());
  for ( int i = 0; i < guess.size(); i++)
  { cout << guess[i] << ' '; }
  cout << endl;
  /*
  for ( int i = 0; i < guess.size(); i++)
  { cout << guess[i] << ' '; }
  cout << endl;
  */

  //cout << "key length guess:" << guessKeyLength(guess) << endl;
  //cout << "best guesses factored | " << guessKeyLength(guess) << endl; //DEBUG message

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

int guessKeyLength(const vector<int> candidates)
{
  int guess = 0;
  vector<int> factors(candidates.back()+1);

  for ( int i = 0; i < candidates.size(); i++ )
  {
    for ( int j = 2; j < candidates[i]; j++)  // keep note of j set to 2
    {
      if ( candidates[i] % j == 0) factors[j] += 1;
    }
  }

  vector<int> guesses;
  //for ( int i = 0; i < factors.size(); i++ )
  for ( int i = factors.size(); i > 1; i-- )
  {
    if ( factors[i] > factors[guess] ) 
    {
      guess = i;
      guesses.push_back(i);
    }
  }

  for ( int i = 0; i < guesses.size(); i++)
  { cout << guesses[i] <<':'<< factors[guesses[i]] << ' '; }
  cout << endl;
  /*
  for ( int i = 0; i < factors.size(); i++)
  { cout << i << ':' << factors[i] << ' '; }
  cout << endl;
  */

  return guess;
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
