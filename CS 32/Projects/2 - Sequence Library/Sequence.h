#ifndef SEQUENCE_INCLUDED
#define SEQUENCE_INCLUDED
#include <string>

using ItemType = unsigned long;

class Sequence 
{
public: 
		// Constructor, Destructor
	Sequence();
	~Sequence();

		// Copy Constructor, Assignment operator
	Sequence(const Sequence& other);
	Sequence& operator=(const Sequence& rhs);

		// Accessors
	int size() const;

		// Member functions
	bool empty() const;
	bool insert(int pos, const ItemType& value);
	int insert(const ItemType& value);
	bool erase(int pos);
	int remove(const ItemType& value);
	bool get(int pos, ItemType& value) const;
	bool set(int pos, const ItemType& value);
	int find(const ItemType& value) const;
	void swap(Sequence& other);
	void dump() const;

private:
	struct Node				
	{
		ItemType value;
		Node* next;
		Node* prev;
	};
	Node* head;
	int	  m_size;
};

void interleave(const Sequence& seq1, const Sequence& seq2, Sequence& result);
int subsequence(const Sequence& seq1, const Sequence& seq2);

#endif // SEQUENCE_INCLUDED