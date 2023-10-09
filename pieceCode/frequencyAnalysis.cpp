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

// Globals
string LSYMBOLS = " abcdefghijklmnopqrstuvwxyz";

/* Function Declarations */
vector<int> genLSymbolFrequency (const string lstring);
vector<int> genShiftedVector (const vector<int> ogVector, const int shift);
string genShiftedString (const string ogCipher, const int shift);
int countCoincidences (const string ogCipher, const string cmpString, const int shift);
vector<int> genKeyLengthGuesses (const vector<int> coincidenceCount, const int maxLength);
int getFrequencyProduct (const vector<int> freq1, const vector<int> freq2);

int main()
{
  // read in ciphertext
  ifstream ifs("cipher.txt");
  string cText( ( istreambuf_iterator<char>(ifs) ),
                ( istreambuf_iterator<char>() ) );
  ifs.close();

  /* Coincidence Indexing to determine likely key lengths */
  // for length of string, count coincidences of resulting shift of i
  vector<int> coincidences;
  for ( int i = 0; i < cText.length(); i++ )
  {
    string shiftedString = genShiftedString(cText, i);
    coincidences.push_back(countCoincidences(cText, shiftedString, i));
  }

  // generate key length guesses based on coincidences and factors
  vector<int> keyLengthCandidates = genKeyLengthGuesses(coincidences, cText.length());

  /* Frequency Analysis to guess keys */
  // get true frequency from dictionaries
  // read dictionaries into strings
  ifstream ifs2("plaintext_dictionary_test1.txt");
  string dict1( ( istreambuf_iterator<char>(ifs2) ),
                ( istreambuf_iterator<char>() ) );
  ifs2.close();
  ifstream ifs3("plaintext_dictionary_test2.txt");
  string dict2( ( istreambuf_iterator<char>(ifs3) ),
                ( istreambuf_iterator<char>() ) );
  ifs3.close();
  // get dict1,dict2 frequencies then combine into first
  vector<int> trueFreq = genLSymbolFrequency(dict1);
  vector<int> dict2Freq = genLSymbolFrequency(dict2);
  //DEBUG
  vector<int> testPrint = genShiftedVector(trueFreq, 1);
  for ( int i = 0; i < 27; i++ )
  {
    trueFreq[i] += dict2Freq[i];
  }

  // get frequencies from possible keys
  for ( auto length : keyLengthCandidates )
  {
    for ( int i = 0; i < length; i++ )
    {
      string substring;
      for ( int j = i; j < cText.length(); j += length )
      {
        // for every length-th element add to substring
        substring += cText[j];
      }

      // analyze substr freq
      vector<int> substrFreq = genLSymbolFrequency(substring);
      // i have make shifted vectors to compare
      int freqProd = getFrequencyProduct(substrFreq, trueFreq);

    }
  }

  return 0;
}

/* Function Definitions */

vector<int> genLSymbolFrequency (const string lstring)
{
  vector<int> freqs(27);
  int lIndex; // ascii converted to index within LSYMBOLS
  for ( auto c : lstring )
  {
    if ( LSYMBOLS.find(c) != string::npos )
    {
      lIndex = (c == ' ') ? 0 : (int(c) - 'a' + 1);
      freqs[lIndex] += 1;
    }
  }

  return freqs;
}

vector<int> genShiftedVector (const vector<int> ogVector, const int shift)
{
  vector<int> shiftVec;
  for ( int i = shift; i < ogVector.size(); i++ )
  {
    shiftVec.push_back(ogVector[i]);
  }
  for ( int i = 0; i < shift; i++ )
  {
    shiftVec.push_back(ogVector[i]);
  }

  /*
  for ( auto v : ogVector ) { cout << v << ' '; }
  cout << endl;
  for ( auto v : shiftVec ) { cout << v << ' '; }
  */

  return shiftVec; 
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

vector<int> genKeyLengthGuesses (const vector<int> coincidenceCount, const int maxLength)
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

int getFrequencyProduct (const vector<int> freq1, const vector<int> freq2)
{
  int prodSum = 0;
  for ( int i = 0; i < 27; i++ )
  {
    prodSum += freq1[i] * freq2[i];
  }

  return prodSum;
}
