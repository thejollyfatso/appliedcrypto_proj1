/*
  Cryptoanalysis of a modified Vigenere Cipher
*/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <numeric>

using namespace std;

// Globals
string LSYMBOLS = " abcdefghijklmnopqrstuvwxyz";

/* Function Prototypes */
vector<float> genLSymbolFrequency (const string lstring);
vector<float> genShiftedVector (const vector<float> ogVector, const int shift);
string genShiftedString (const string ogCipher, const int shift);

int countCoincidences (const string ogCipher, const string cmpString, const int shift);
vector<int> genKeyLengthGuesses (const vector<int> coincidenceCount, const int maxLength);

float getFrequencyProduct (const vector<float> freq1, const vector<float> freq2);
vector<string> genKeyPosCandidates (const int keyLength, const string cText, const vector<float>& trueFreq);
vector<string> genPossibleKeys (const vector<string> keyPosCandidates, const int strengthIndex);

const string decryptMessage(const string& ciphertext, const string& key);
char decryptCharacter(char cipherChar, char keyChar);
int scorePlaintext(const string& plaintext, const vector<string>& dictionary);


int main()
{
  // read in ciphertext to string cText
  ifstream ifsC("cipher.txt");
  if (!ifsC)
  {
    cerr << "Could not open the file.\n";
    exit(1);
  }
  string cText( ( istreambuf_iterator<char>(ifsC) ),
                ( istreambuf_iterator<char>() ) );
  ifsC.close();
  cText.pop_back(); // get rid of superfluous newline character

  /* Coincidence Indexing to determine likely key lengths */
  // for length of string, count coincidences b/w resulting shift of i
  vector<int> coincidences;
  for ( int i = 0; i < cText.length(); i++ )
  {
    string shiftedString = genShiftedString(cText, i);
    coincidences.push_back(countCoincidences(cText, shiftedString, i));
  }

  // generate key length guesses based on coincidences and factors
  vector<int> keyLengthCandidates = genKeyLengthGuesses(coincidences, cText.length());

  /* Frequency Analysis to guess keys */
  // get true frequency of characters from provided dictionaries

  // read dictionaries into strings
  ifstream ifsD1("plaintext_dictionary_test1.txt");
  if (!ifsD1)
  {
    cerr << "Could not open the file.\n";
    exit(1);
  }
  string dict1( ( istreambuf_iterator<char>(ifsD1) ),
                ( istreambuf_iterator<char>() ) );
  ifsD1.close();
  ifstream ifsD2("plaintext_dictionary_test2.txt");
  if (!ifsD2)
  {
    cerr << "Could not open the file.\n";
    exit(1);
  }
  string dict2( ( istreambuf_iterator<char>(ifsD2) ),
                ( istreambuf_iterator<char>() ) );
  ifsD2.close();

  // get dict1,dict2 frequencies then combine into first
  vector<float> trueFreq = genLSymbolFrequency(dict1);
  vector<float> dict2Freq = genLSymbolFrequency(dict2);
  for ( int i = 0; i < 27; i++ )
  {
    trueFreq[i] += dict2Freq[i];
  }

  // generate possible keys from frequencies
  vector<string> keys;
  int atkStrength = 1; // see genKeyCandidates() for more on strength
  for ( int i = 1; i < keyLengthCandidates.size(); i++ )
  {
    vector<string> keyPosCandidates = genKeyPosCandidates( keyLengthCandidates[i], cText, trueFreq );
    vector<string> keyCandidates = genPossibleKeys( keyPosCandidates, atkStrength );
    for ( auto kc : keyCandidates )
    {
      keys.push_back(kc);
    }
  }
  //DEBUG
  /*
  for ( auto k : keys )
  {
    cout << k << ' ';
  }
  */

  // Load both dictionary files into vector of strings for a compiled dictionary
  ifstream dictionaryFile("plaintext_dictionary_test1.txt");
  if (!dictionaryFile)
  {
    cerr << "Could not open the file.\n";
    exit(1);
  }
  vector<string> dictionary;
  string word;
  while (dictionaryFile >> word) {
      dictionary.push_back(word);
  }
  dictionaryFile.close();
  ifstream dictionaryFile2("plaintext_dictionary_test2.txt");
  if (!dictionaryFile2)
  {
    cerr << "Could not open the file.\n";
    exit(1);
  }
    while (dictionaryFile2 >> word) {
      dictionary.push_back(word);
  }
  dictionaryFile2.close();

  // Find key with best score
  /*
  int bestScore = 0;
  string bestGuess;
  for ( auto key : keys )
  {
    string plaintext = decryptMessage(cText,key); 
    int currScore = scorePlaintext( plaintext, dictionary );
    if ( bestScore < currScore )
    {
      bestScore = currScore;
      bestGuess = plaintext;
    }
  }

  cout << "My guess for the plaintext is:\n" << bestGuess << endl;
  */

  return 0;
}

/* Function Definitions */

/*
  Takes a string and scans through for occurrences of each L-symbol.
  Returns a length-27 vector of floats representing the ratio frequency
  of each l-symbol.
*/
vector<float> genLSymbolFrequency (const string lstring)
{
  vector<float> occurrences(27);
  int lIndex; // ascii converted to index within LSYMBOLS
  for ( auto c : lstring )
  {
    if ( LSYMBOLS.find(c) != string::npos )
    {
      lIndex = (c == ' ') ? 0 : (int(c) - 'a' + 1);
      occurrences[lIndex] += 1;
    }
  }

  // converting from occurrences to frequencies
  vector<float> freqs(27);
  for ( int i = 0; i < 27; i++ )
  {
    freqs[i] = occurrences[i]/lstring.length();
  }

  return freqs;
}

/*
  Takes in a l-symbol frequency map represented as a vector of floats
  and shifts it backwards by int shift, wrapping around.
  Returns the shifted vector.
*/
vector<float> genShiftedVector (const vector<float> ogVector, const int shift)
{
  vector<float> shiftVec;
  for ( int i = shift; i < ogVector.size(); i++ )
  {
    shiftVec.push_back(ogVector[i]);
  }
  for ( int i = 0; i < shift; i++ )
  {
    shiftVec.push_back(ogVector[i]);
  }

  return shiftVec; 
}

/*
  Takes in a string and shifts it forwards by int shift and
  returns the shifted string.
*/
string genShiftedString (const string ogCipher, const int shift)
{
  string shiftString;
  for ( int i = 0; i < ogCipher.length() - shift; i++ )
  {
    shiftString += ogCipher[i];
  }
  return shiftString;
}

/*
  Takes in two strings and an integer.  One string represents the
  original ciphertext, the other represents the ciphertext after
  forward shifting by int shift. Returns the number of coincidences
  or matching characters at shifted indices between the two strings.
*/
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

/*
  Follows countCoincidences. Takes in the vector assumedly generated
  by countCoincidences along with the maxLength we will scan for the key.
  It will return a set of candidate key lengths as a vector of ints.
*/
vector<int> genKeyLengthGuesses (const vector<int> coincidenceCount, const int maxLength)
{
  vector<int> candidateGuesses; // candidates determined by coincidences
  vector<int> guesses;          // guesses to be returned after factoring
  int guess;
  int mean = 0;

  // for each possible size of key, represented by i
  for ( int i = 1; i < maxLength; i++ )
  {
    int totalCoincidences = 0;
    int denominator = 0;
    // determine the average number of coincidences every i-th character
    for ( int j = 0; j < coincidenceCount.size(); j += i )
    {
      totalCoincidences += coincidenceCount[j];
      denominator++;
    }
    
    // if average is the highest so far, replace guess, add to candidates
    if ( mean < totalCoincidences/denominator )
    {
      guess = i;
      candidateGuesses.push_back(guess);
      mean = totalCoincidences/denominator;
    }
  }

  // sort out superfluous multiples
  vector<int> factors(candidateGuesses.back()+1);

  // from candidates, determine commonly occuring factors
  for ( int i = 0; i < candidateGuesses.size(); i++ )
  {
    for ( int j = 1; j < candidateGuesses[i]; j++)  
    {
      if ( candidateGuesses[i] % j == 0) factors[j] += 1;
    }
  }

  // as with previous candidates, update best key legnth guesses
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

/*
  Takes two frequency maps represented as vectors of floats and
  sums the product of the floats at each index.
*/
float getFrequencyProduct (const vector<float> freq1, const vector<float> freq2)
{
  float prodSum = 0;
  for ( int i = 0; i < 27; i++ )
  {
    prodSum += freq1[i] * freq2[i];
  }

  return prodSum;
}

/*
  Takes in an integer value for local key length and the original
  ciphertext, as well as the true frequency from dictionaries
  as a comparison reference.
  Returns a keyLength sized vector of length 27 strings
  of all l-symbols ordered by their likelihood of correctness.
*/
vector<string> genKeyPosCandidates (const int keyLength, const string cText, const vector<float>& trueFreq )
{
  vector<string> keyPosCandidates(keyLength);
  for ( int i = 0; i < keyLength; i++ )
  {
    // get substring determined by every i-th letter of the cipher text
    string substring;
    for ( int j = i; j < cText.length(); j += keyLength )
    {
      substring += cText[j];
    }

    // get all frequency products of all 27 substring shifts
    vector<float> frequencyProducts;
    vector<float> substrFreq = genLSymbolFrequency(substring);
    for ( int j = 0; j < 27; j++ )
    {
      frequencyProducts.push_back( getFrequencyProduct(genShiftedVector(substrFreq, j), trueFreq) );
    }

    // sort vector of indexes by frequency products to get order of max indexes
    vector<int> index(27);
    int idx = 0;
    while ( idx < 27 )
    {
      index[idx] = idx++;
    }
    sort( index.begin(), index.end(), [&](int i, int j)
      { return frequencyProducts[i]<frequencyProducts[j]; } );

    for ( int j = 0; j < 27; j++ )
    {
      keyPosCandidates[i] += LSYMBOLS[index[j]];
    }
  }

  return keyPosCandidates;
}

/*
  Takes a vector of strings generated from genKeyPosCandidates()
  and a integer for attack strength. Generates every possible combination
  of the strengthIndex number of the most likely candidates.
  (i.e. strengthIndex of 4 selects the four most likely characters for each
  position in the string. Note a strength index of 27 results in a complete
  brute force attack)
*/
vector<string> genPossibleKeys (const vector<string> keyPosCandidates, const int strengthIndex)
{
  vector<string> possibleKeys;
  vector<int> incrementer (keyPosCandidates.size()); // key sized vector
  while ( accumulate( incrementer.begin(), incrementer.end(), 0 ) <=
          keyPosCandidates.size() * strengthIndex )
  {
    // first add resulting key from incrementer
    string possibleKey;
    for ( int j = 0; j < keyPosCandidates.size(); j++ )
    {
      possibleKey += keyPosCandidates[j][incrementer[j]];
    }
    possibleKeys.push_back(possibleKey);
    cout << possibleKey << '\n'; // DEBUG

    // increment the incrementer, base strengthIndex
    int inc = 0; // functions as a temporary placeholder for index
    while ( incrementer[inc] > strengthIndex ) { inc++; }
    incrementer[inc]++;
    if ( inc > 0 ) { incrementer[inc-1] = 0; };
  }

  return possibleKeys;
}

/*
  Given ciphertext and key, returns the plaintext that would result.
*/
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

/*
  Helper function to decryptMessage.
  Given the cipher character and key character, returns plain character.
*/
char decryptCharacter(char cipherChar, char keyChar)
{
  char decryptedChar;
  int shiftDistance;

  // Find shift distance - this is the index of the Key character within the Message Space
  for (size_t i = 0; i < LSYMBOLS.size(); ++i)
  {
    if (LSYMBOLS[i] == keyChar)
    {
      shiftDistance = i;
      break;
    }
  }
  
  // Find index of the Cipher Char within the Message Space
  int posCipherChar = (cipherChar == ' ') ? 0 : (int(cipherChar) - 'a' + 1);

  // shift the cipher char by shift distance
  int shift = posCipherChar - shiftDistance;
  decryptedChar = ((posCipherChar - shiftDistance) % LSYMBOLS.size()) + ('a' - 1);
  if (int(decryptedChar) == 96) decryptedChar = ' ';
  if (shift < 0)
  {
    decryptedChar = 'z' + 1 + shift;
  }

  return decryptedChar;
}

/* 
  Takes in decoded text and a dictionary of words and returns int score
  based on how many words from the text are found in dictionary.
*/
int scorePlaintext(const string& plaintext, const vector<string>& dictionary) {
  int score = 0;
  string word = "";
    
  for (char c : plaintext) {
    if (c == ' ') {
      // Check if the word is in the dictionary
      if (find(dictionary.begin(), dictionary.end(), word) != dictionary.end()) {
          score++;
      }
      word = "";
    } else {
      word += c;
    }
  }
    
  return score;
}
