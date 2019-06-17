//
// Created by nadav on 11/28/2018.
//

#ifndef EX2_MAP_LIST_H
#define EX2_MAP_LIST_H

#include <stdexcept>
//#include "library1_for_list.h"
#include <iostream>
using namespace std;

template <class K, class D>
class ListNode{
    K key;
    D data;
    ListNode* next_node;
    ListNode* prev_node;

public:
    ListNode(const K& key, const D& data);

/*
 * Description: gets the node's key
 * Input:   none.
 * Output:  the node's key
 */
    const K& get_key();

/*
 * Description: gets the node's data
 * Input:   none.
 * Output:  the node's data
 */
    const D& get_data();

/*
 * Description: gets the node's next_node
 * Input:   none.
 * Output:  the node's next_node
 */
    ListNode* get_next();           //check about constant!!!

/*
 * Description: gets the node's prev_node
 * Input:   none.
 * Output:  the node's prev_node
 */
    ListNode* get_prev();

/*
 * Description: sets the node's next_node
 * Input:   next - the requested node's next_node
 * Output:  none.
 */
    void set_next(ListNode* next);

/*
 * Description: sets the node's prev_node
 * Input:   previous - the requested node's prev_node
 * Output:  none.
 */
    void set_prev(ListNode* previous);

/*
 * Description: inside function for printing
 */
    ostream& printNode(ostream& os);

};

//------------------------------------------------------------------------------
template <class K, class D>
class Node_list{
    ListNode<K,D>* first;
    int size;
public:
    Node_list();
    ~Node_list();

/*
 * Description: gets the lists first node
 * Input:   none.
 * Output:  the lists first node
 */
    ListNode<K,D>* get_first();

/*
 * Description: sets the lists first node
 * Input:   n - the requested node to inserts to list first
 * Output:  none
 */
    void set_first(ListNode<K,D>* n);

/*
 * Description: gets the list's size
 * Input:   none.
 * Output:  the tree's size
 */
    int get_size() const;

/*
 * Description: adds a new node to the list
 * inserts the node in the beginning of the list
 * Input:   key - the key of new node to add
 *          data - the data of new node to add
 * Output:  none
 */
    void add_node(const K& key, const D& data);

/*
 * Description: find's the node with the given key, using recurse finc find_recurse
 * Input:   key - the key of the node to find
 * Output:  result - the node with the given key. if there is no node in the tree with the key, returns null
 */
    ListNode<K,D>* find(const K& key);

/*
 * Description: removes the node from the list
 * Input:   node_ptr - the required node to remove.
 * Output:  none.
 */
    void remove_node(ListNode<K,D>* node_ptr);

/*
 * Description: inside function for printing
 */
    ostream& printList(ostream& os);

};

//----------------------------------------------------------------------------//
//------------------------------ListNode Implement--------------------------------//
//----------------------------------------------------------------------------//
template <class K, class D>
ListNode<K,D>::ListNode(const K& key, const D& data) : key(key), data(data),
                                               next_node(nullptr), prev_node(nullptr){
}

template <class K, class D>
const K& ListNode<K,D>::get_key() {
    return this->key;
}

template <class K, class D>
const D& ListNode<K,D>::get_data() {
    return this->data;
}

template <class K, class D>
ListNode<K,D>* ListNode<K,D>::get_next() {
    return this->next_node;
}

template <class K, class D>
ListNode<K,D>* ListNode<K,D>::get_prev() {
    return this->prev_node;
}

template <class K, class D>
void ListNode<K,D>::set_next(ListNode<K,D> *next) {
    this->next_node=next;
}

template <class K, class D>
void ListNode<K,D>::set_prev(ListNode<K,D> *previous) {
    this->prev_node=previous;
}

template <class K, class D>
ostream& ListNode<K,D>::printNode(ostream& os){
    return os <<"Key: " << this->get_key() << endl;

}
//-------------------------------------------------------------------------------

template <class K, class D>
Node_list<K,D>::Node_list() : first(nullptr), size(0) {
}

template <class K, class D>
Node_list<K,D>::~Node_list() {
    ListNode<K,D>* node_to_delete=this->first;
    ListNode<K,D>* next_node;
    while(node_to_delete!= nullptr){
        next_node=node_to_delete->get_next();
        delete node_to_delete;
        node_to_delete=next_node;
    }
}

template <class K, class D>
ListNode<K,D>* Node_list<K,D>::get_first() {
    return this->first;
}
template <class K, class D>
void Node_list<K,D>::set_first(ListNode<K,D>* n){
    this->first=n;
}

template <class K, class D>
void Node_list<K,D>::add_node(const K& key, const D& data) {
    ListNode<K,D>* new_node = new ListNode<K,D>(key, data);
    new_node->set_next(this->first);
    if(this->first!= nullptr)
        this->first->set_prev(new_node);
    this->first=new_node;
    this->size++;
}

template <class K, class D>
ListNode<K,D>* Node_list<K,D>::find(const K& key){
    ListNode<K,D>* result=this->get_first();
    while(result!= nullptr && result->get_key()!=key){
        result=result->get_next();
    }
    return result;
}

template <class K, class D>
void Node_list<K,D>::remove_node(ListNode<K,D>* node_ptr){
    if(node_ptr->get_next() != nullptr){
        node_ptr->get_next()->set_prev(node_ptr->get_prev());
    }

    if(node_ptr->get_prev() != nullptr){
        node_ptr->get_prev()->set_next(node_ptr->get_next());
    }
    if(this->get_first()==node_ptr){
        this->set_first(node_ptr->get_next());
    }
    delete node_ptr;
    this->size--;
}

template <class K, class D>
int Node_list<K,D>::get_size() const {
    return this->size;
}

template <class K, class D>
ostream& Node_list<K,D>::printList(ostream& os){
    ListNode<K,D>* current_node=this->get_first();
    if(current_node == nullptr)
        return os << "Empty List!!!" << endl;
    while (current_node!= nullptr){
        current_node->printNode(os);
        current_node=current_node->get_next();
    }
    return os;
};

#endif //EX2_MAP_LIST_H
