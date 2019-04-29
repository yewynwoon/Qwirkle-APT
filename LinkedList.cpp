#include "LinkedList.h"

LinkedList::LinkedList() 
{
    head = nullptr;
	tail = nullptr;
	size = 0;
}

LinkedList::~LinkedList() 
{
	Node* curr = head;
	Node* prev;
	while (curr != nullptr)
	{
		prev = curr;
		curr = curr->next;
		delete prev;
	}
}

// Adds the supplied tile to the back of the list
void LinkedList::add_back(Tile* tile)
{
	//TODO
}

// Removes the tile at the start of the list and returns it
Tile* LinkedList::pop_front()
{
	//TODO
}

// Removes the specified tile from the list
void LinkedList::remove(Tile* tile)
{
	Node* current = nullptr;
	Node* previous = nullptr;
	current = head->next;
	previous = head;
	while (true) {
		if (current->tile == tile)
		{
			previous->next = current->next;
			delete current;
			break;
		}
		else if (current != tail)
		{
			previous = current;
			current = current->next;
		}
		else
		{
			break;
		}
	}
}
