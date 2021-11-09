// Dictionary.cpp

// This is a correct but horribly inefficient implementation of Dictionary
// functionality.  Your job is to change DictionaryImpl to a more efficient
// correct implementation.  You can change it any way you like, subject to
// restrictions in the spec (e.g., that the only C++ library container you
// are allowed to use are vector, list, stack, and queue (and string); if you
// want anything fancier, implement it yourself).

#include "Dictionary.h"
#include <string>
#include <vector>
#include <list>
#include <cctype>
#include <utility>  // for swap
using namespace std;

const int PRIME_NUMS[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101 };
const int MAP_SIZE = 50000;

void removeNonLetters(string& s);
void generateNextPermutation(string& permutation);

  // This class does the real work of the implementation.

struct Node
{
	Node(string val) 
		: word(val), next(nullptr) {};
	Node* next;
	string word;
};

class DictionaryImpl
{
  public:
    DictionaryImpl();
	~DictionaryImpl() {};
	unsigned int hashFoo(string word) const;
	unsigned int toInt(string word) const;
    void insert(string word);
    void lookup(string letters, void callback(string)) const;
  private:
    list<string> m_words;
	Node* hash_map[MAP_SIZE];
};

DictionaryImpl::DictionaryImpl()
{
	for (int i = 0; i < MAP_SIZE; i++)
	{
		hash_map[i] = nullptr;
	}
}

unsigned int DictionaryImpl::hashFoo(string word) const
{
	removeNonLetters(word);
	return (toInt(word) % MAP_SIZE);
}

unsigned int DictionaryImpl::toInt(string word) const
{
	unsigned int key = 1;

	for (size_t i = 0; i < word.size(); i++)
	{
		key *= (PRIME_NUMS[word[i] - 'a']);
	}

	return key;
}

void DictionaryImpl::insert(string word)
{
	removeNonLetters(word);

	unsigned int pos = hashFoo(word);
	Node*& newNode = hash_map[pos];

	if (newNode == nullptr)
	{
		newNode = new Node(word);
	}
	else
	{
		Node* temp = new Node(word);
		temp->next = newNode->next;
		newNode->next = temp;
	}
}

void DictionaryImpl::lookup(string letters, void callback(string)) const
{
	if (callback == nullptr)
		return;

	removeNonLetters(letters);

	if (letters.empty())
		return;

	unsigned int pos = hashFoo(letters);
	Node* curr = hash_map[pos];
	while (curr != nullptr)
	{
		if (toInt(curr->word) == toInt(letters))
		{
			callback(curr->word);
			curr = curr->next;
		}
		else
			curr = curr->next;
	}
}
    
void removeNonLetters(string& s)
{
    string::iterator to = s.begin();
    for (string::const_iterator from = s.begin(); from != s.end(); from++)
    {
        if (isalpha(*from))
        {
            *to = tolower(*from);
            to++;
        }
    }
    s.erase(to, s.end());  // chop everything off from "to" to end.
} 

  // Each successive call to this function generates the next permutation of the
  // characters in the parameter.  For example,
  //    string s = "eel";
  //    generateNextPermutation(s);  // now s == "ele"
  //    generateNextPermutation(s);  // now s == "lee"
  //    generateNextPermutation(s);  // now s == "eel"
  // You don't have to understand this function's implementation.
void generateNextPermutation(string& permutation)
{
    string::iterator last = permutation.end() - 1;
    string::iterator p;

    for (p = last; p != permutation.begin()  &&  *p <= *(p-1); p--)
        ;
    if (p != permutation.begin())
    {
        string::iterator q;
        for (q = p+1; q <= last  &&  *q > *(p-1); q++)
            ;
        swap(*(p-1), *(q-1));
    }
    for ( ; p < last; p++, last--)
        swap(*p, *last);
}

//******************** Dictionary functions ********************************

// These functions simply delegate to DictionaryImpl's functions.
// You probably don't want to change any of this code

Dictionary::Dictionary()
{
    m_impl = new DictionaryImpl;
}

Dictionary::~Dictionary()
{
    delete m_impl;
}

void Dictionary::insert(string word)
{
    m_impl->insert(word);
}

void Dictionary::lookup(string letters, void callback(string)) const
{
    m_impl->lookup(letters,callback);
}
