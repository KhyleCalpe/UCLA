#include "Sequence.h"
#include <iostream>
using namespace std;

Sequence::Sequence()		// Create an empty circular doubly-linked list
	: m_size(0)
{ 
	head = new Node;
	head->next = head;
	head->prev = head;
}

Sequence::~Sequence()
{
	while (head->next != head)		// delete each node until head is reached
	{
		Node* p = head->next;
		p->prev->next = p->next;
		p->next->prev = p->prev;
		delete p;
	}
	delete head;
}

Sequence::Sequence(const Sequence & other)
	: m_size(other.m_size)
{
	head = new Node;
	head->next = head;
	head->prev = head;

	for (Node* p = other.head->next; p != other.head; p = p->next)		// Loop through the other sequence
	{
		Node* newNode = new Node;										// Declare new nodes to copy other's nodes
		newNode->value = p->value;										// Assign pointers to point to proper nodes
		newNode->next = head;
		newNode->prev = head->prev;
		head->prev->next = newNode;
		head->prev = newNode;
	}
}

Sequence & Sequence::operator=(const Sequence & rhs)
{
	if (this != &rhs)
	{
		Sequence temp(rhs);
		swap(temp);
	}
	return *this;
}

bool Sequence::empty() const
{
	return (m_size == 0);
}

int Sequence::size() const
{
	return m_size;
}

bool Sequence::insert(int pos, const ItemType & value)
{

	if (pos < 0 || pos > size())		// If the position is out of bounds, return false;
		return false;
	
	Node* p = new Node;					// Create a node for traversing the list
	Node* newNode = new Node;			// Create a node for insertion
	
	if (pos == size())					// Add a new node if the position is at the end of the list
	{
		p = head->prev;
		newNode->value = value;
		newNode->prev = p;
		newNode->next = head;
		head->prev = newNode;
		p->next = newNode;
	}
	else								// Add a new node at position pos
	{
		p = head;
		for (int i = 0; i < size(); i++)
		{
			p = p->next;
			if (i == pos)
			{
				newNode->value = value;
				newNode->prev = p->prev;
				newNode->next = p;
				p->prev->next = newNode;
				p->prev = newNode;
			}
		}
	}

	m_size++;		// Increment the size of the list
	return true;
}

int Sequence::insert(const ItemType & value)
{
	int pos = 0;
	Node* p = head->next;			// Create a new node for traversing the list
	
	for (; pos < size() && value > p->value; pos++)		// Increment pos until the amount at pos > value
	{
		p = p->next;
	}

	if (p->value >= value)			// Insert the new value if the new value is less than or equal to the next value
	{
		insert(pos, value);
		return pos;
	}

	pos = size();					// If the value is larger than any other amount, insert the value at the end
	insert(pos, value);
	return pos;
}

bool Sequence::erase(int pos)
{
	if (pos >= 0 && pos < size())							// Check if the position is within the bounds of the list
	{
		Node* p = head;										// Declare a temporary pointer for traversing the list and erasing a node
		for (int i = 0; i < size(); i++, p = p->next)		// Traverse the list and erase the node at position i 
		{
			if (i == pos)
			{
				p->prev->next = p->next;
				p->next->prev = p->prev;
				delete p;
				m_size--;
				return true;
			}
		}
	}
	return false;	// If unsuccessful, return false
}

int Sequence::remove(const ItemType & value)
{
	int rmvdItems = 0;
	Node* p = head;								// Create a node for traversion
	for (int i = 0; i < size(); i++)					// Loop through the list 
	{
		p = p->next;
		if (p->value == value)
		{
			Node* tempNode = p;							// Create a temporary pointer for deletion
			p->prev->next = tempNode->next;				
			p->next->prev = tempNode->prev;
			rmvdItems++;
			m_size--;
			delete tempNode;							
		}
	}
	return rmvdItems;
}

bool Sequence::get(int pos, ItemType & value) const
{
	if (pos >= 0 && pos < size())							// Check if the position is within bounds
	{
		Node* p = head;								// Declare a pointer for traversion
		for (int i = 0; i < size(); i++)					// Loop until the position is found
		{
			p = p->next;
			if (i == pos)									// If the position is found, copy the value
			{
				value = p->value;
				return true;
			}
		}
	}
	return false;
}

bool Sequence::set(int pos, const ItemType & value)
{
	if (pos >= 0 && pos < size())							// Check if the position os within bounds
	{
		Node* p = head;										// Declare a pointer for traversion
		for (int i = 0; i < size(); i++)					// Loop until the position is found
		{
			p = p->next;
			if (i == pos)									// If the position is found, value replaces the node's initial value
			{
				p->value = value;
				return true;
			}
		}
	}
	return false;
}

int Sequence::find(const ItemType & value) const
{
	Node* p = head;										// Declare a pointer for traversion
	for (int i = 0; i < size(); i++)					// loop until the value is found
	{
		p = p->next;
		if (p->value == value)							// If found, return the position, else return -1
		{
			return i;
		}
	}
	return -1;
}

void Sequence::swap(Sequence & other)
{
	int tempSize = m_size;				// Swap the sizes
	m_size = other.m_size;
	other.m_size = tempSize;

	Node* tempHead = new Node;			// Swap the heads
	head = other.head;
	other.head = tempHead;
}

void Sequence::dump() const
{
	Node* p = head;							
	for (int i = 0; i < size(); i++)	// Print out the sequence
	{
		p = p->next;
		cout << p->value << endl;
	}
}

int subsequence(const Sequence& seq1, const Sequence& seq2)
{
	if (seq2.empty() || seq2.size() > seq1.size())		// Check if seq2 is empty or seq2 is larger than seq1
		return -1;

	bool isSubseq = false;
	ItemType val_1, val_2;

	for (int i = 0; i < seq1.size(); i++)				// Loop through seq1  
	{
		seq1.get(i, val_1);
		seq2.get(0, val_2);
		if (val_1 == val_2)								// Check seq1 for a value that matches seq2's first value 
		{
			isSubseq = true;
			for (int j = 0; j < seq2.size(); j++)		// Loop through seq1 and seq2 
			{
				seq1.get(i + j, val_1);
				seq2.get(j, val_2);
				if (val_1 != val_2)						// Check if the values from both sequence are equal to each other
				{
					isSubseq = false;
					break;
				}
			}
			if (isSubseq)								// Check if seq2 is a subsequence
				return i;								// Return the starting position of the subseqence
		}
	}
	return -1;
}

void interleave(const Sequence& seq1, const Sequence& seq2, Sequence& result) 
{
	Sequence tempSeq = result;
	ItemType val_1, val_2;
	int pos_1 = 0, pos_2 = 0, currPos = 0;
	
	for (int i = tempSeq.size(); i > 0; i--)	// Erase tempSeq
		tempSeq.erase(i);
	if (seq1.empty())				// If a sequence is empty, set tempSeq to the non-empty sequence
		tempSeq = seq2;
	if (seq2.empty())
		tempSeq = seq1;
	
	while (pos_1 < seq1.size() || pos_2 < seq2.size())		// Loop until the end of either sequence is reached
	{
		if (pos_1 < seq1.size())							// While either sequence's capacity is not reached, insert values
		{
			seq1.get(pos_1, val_1);
			tempSeq.insert(currPos, val_1);
			pos_1++, currPos++;
		}
		if (pos_2 < seq2.size())
		{
			seq2.get(pos_2, val_2);
			tempSeq.insert(currPos, val_2);
			pos_2++, currPos++;
		}
	}
	result = tempSeq;
}