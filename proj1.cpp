/*
  Cryptoanalysis of a modified Vigenere Cipher
*/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

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

const string decryptMessage(const string& ciphertext, const string& key);
char decryptCharacter(char cipherChar, char keyChar);

int main()
{
  // read in ciphertext to string cText
  ifstream ifsC("cipher.txt");
  if (!ifsC)
  {
    cerr << "Could not open the file.\n";
    exit(1);
  }

  string cText;
  getline(ifsC, cText);
  ifsC.close();

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
  string dict1( ( istreambuf_iterator<char>(ifsD1) ),
                ( istreambuf_iterator<char>() ) );
  ifsD1.close();
  ifstream ifsD2("plaintext_dictionary_test2.txt");
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
  // for each possible length determined previously through coincidences
  for ( auto length : keyLengthCandidates )
  {
    // determine most likely key character for each character of key
    string candidateKey;
    for ( int i = 0; i < length; i++ )
    {
      // get substring determined by every i-th letter of the cipher text
      string substring;
      for ( int j = i; j < cText.length(); j += length )
      {
        substring += cText[j];
      }

      // find the most likely "shift" of L-symbol frequency
      vector<float> substrFreq = genLSymbolFrequency(substring);
      float maxFreq = 0;
      int guess = 0;
      for ( int i = 0; i < 27; i++ )
      {
        // comparing each possible shift of the substring to true frequency
        float freqProd = getFrequencyProduct(genShiftedVector(substrFreq, i), trueFreq);
        if ( freqProd > maxFreq ) 
        {
          maxFreq = freqProd;
          guess = i;
        }
      }
      // after finding most likely key char, append to key guess
      candidateKey += LSYMBOLS[guess];
    }

    keys.push_back(candidateKey);
  }

  // DEBUG: Currently outputs series of guesses. Will need to score most accurate one somehow.
  for ( auto key : keys )
  {
    cout << "Using key: " << key << endl << "My plaintext guess is: " << decryptMessage(cText, key) << endl << endl;
  }

  return 0;
}

/* Function Definitions */

/*
  Takes a string and scans through for occurrences of each L-symbol.
  Returns a length-27 vector of integers uh oh lmao
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
    for ( int j = 1; j < candidateGuesses[i]; j++)  
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

float getFrequencyProduct (const vector<float> freq1, const vector<float> freq2)
{
  float prodSum = 0;
  for ( int i = 0; i < 27; i++ )
  {
    prodSum += freq1[i] * freq2[i];
  }

  return prodSum;
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

