#ifndef LINKED_LISTS_HPP
#define LINKED_LISTS_HPP
#include <cstddef>
#include <stdexcept>
#include <string>

template<typename T>
class Node{
	public:
		T data;
		Node* next;
		Node* prev;
		Node(T data, Node* next = nullptr, Node* prev = nullptr): 
			data{data}, 
			next{next},
			prev{prev}{}	
		std::string toString(){
			return std::to_string(data);
		}
};

template<typename T>
class LinkedList{
	private:
		Node<T>* head;
		Node<T>* tail;
		size_t size;
	public:
		LinkedList(): head{nullptr}, tail{nullptr}, size{0}{}
	
		int      getSize(){return size;}
		Node<T>* getHead(){return head;}
		Node<T>* getTail(){return tail;}
		bool     isEmpty(){return (size == 0);}
		
		void addLeft(T value){
			Node<T>* new_node = new Node<T>(value);
			if (head == nullptr){
				size ++;
				head = new_node; 
				tail = new_node; 
				return;
			}
			size ++;
			new_node -> next = head;
			head->prev = new_node;
			head = new_node;
		}

		void addRight(T value){
			Node<T>* new_node = new Node<T>(value);
			if (head == nullptr){
				size ++;
				head = new_node; 
				tail = new_node; 
				return;
			}
			size ++;
			tail->next = new_node;
			new_node->prev = tail;
			tail = new_node;
		}
		
		T removeLeft(){
			if (size == 0){
				throw std::length_error("Cannot remove from an empty list");
			}
			T return_data = head->data;
			if (size == 1){
				delete head;
				head = nullptr;
				tail = nullptr;
				size --;
				return return_data;
			}
			Node<T>& new_head = *head->next;
			delete head;
			head = &new_head;
			head->prev = nullptr;
			size --;
			return return_data;
		}	

		T removeRight(){
			if (size == 0){
				throw std::length_error("Cannot remove from an empty list");
			}
			T return_data  = tail->data;
			if (size == 1){
				delete tail;
				head = nullptr;
				tail = nullptr;
				size --;
				return return_data;
			}
			Node<T>* new_tail_ptr = tail->prev;
			delete tail;
			tail = new_tail_ptr;
			new_tail_ptr->next = nullptr;
			size --;
			return return_data;
		}	

		std::string toString(){ 
			if (size == 0){return std::string("EMPTY");}
			std::string return_string = "head";
			Node<T>* cur_node = head;
			while (cur_node != nullptr){
				return_string += "->[" + cur_node->toString() + "]";
				cur_node = cur_node->next;
			}
			return_string += "<-tail";
			return return_string;
		}		
};

//Template specialization because std::to_string does not have an overload
//for std::string

template<>
inline std::string Node<std::string>::toString(){
	return data;
}

#endif
