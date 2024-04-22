#ifndef PRIORITY_QUEUE_CPP
#define PRIORITY_QUEUE_CPP
/*
 *	This codebase uses javadoc-style banners. If using doxygen, make
 *	sure to set JAVADOC_BANNER to YES to get the appropriate hinting
 *	when using it!
 */

#define NDEBUG
#ifndef NDEBUG 
#define DEBUG_MSG(str) do { std::cout << str << "\n"; } while (false)
#else
#define DEBUG_MSG(str) do {} while(false)
#endif
#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include "../incl/utils.hpp"

template<typename K, typename V>
class Entry{

	public:
		K   key;
		V   value;
		int index;

		Entry(K key, V value)           : key{key}, value{value}{}
		Entry(K key, V value, int index): key{key}, value{value}, index{index}{}

		bool operator< (Entry& other){ return (this->key < other.key); }
		bool operator> (Entry& other){ return (this->key > other.key); }
		bool operator<=(Entry& other){ return (!(*this>other)); }
		bool operator>=(Entry& other){ return (!(*this<other)); }
		bool operator==(Entry& other){
			bool eq_keys   = this->key   == other.key;
			bool eq_values = this->value == other.value;
			return (eq_keys && eq_values);
		}

		std::string to_string(){
			/*****************************************************************
			 * @brief Produces std::string representation of the entry. 
			 * @return an std::string formatted as "(K,V)".
			 *****************************************************************/
			std::string output_str;
			auto key_string  = stringify(key);
			auto val_string  = stringify(value);
			output_str = output_str 
				+ "("
				+ key_string
				+ ","
				+ val_string
				+ ")" ;
			return output_str;
		}

};

template<typename K, typename V>
class PriorityQueue{
	
	private:

		// Has to be a pointer or else it will fill up the stack
		// unique_ptr handles object deletion when they themselves
		// are deleted.
		std::vector<std::unique_ptr<Entry<K,V>>> container;
		
		int parent_i(int i){
			/*****************************************************************
			 * @brief Get the index of the parent of an Entry 
			 * @param i the index of the child
			 * @return int index of parent or -1 if there is no parent 
			 * @exception std::out_of_range if i would be out of bounds
			 * Time Complexity: O(1)
			 ****************************************************************/

			if (i >= this->size() || i < 0){ throw std::out_of_range("Index out of bounds.");}
			if (i == 0){ return -1;}		

			const bool is_odd    = i % 2;
			const int difference = is_odd ? 1 : 2;

			const int parent_index = (i-difference)/2;
			return parent_index;
		}

		int right_child_i(int i){
			/*****************************************************************
			 * @brief Get the index of the right child for an Entry
			 * @param i index of entry in container
			 * @returns -1 if the entry has no right child
			 * @exception std::out_of_range if i would be out of bounds
			 * Time Complexity: O(1)
			 ****************************************************************/

			if (i >= this->size()){ throw std::out_of_range("Index out of bounds.");}

			const int child_index = (i * 2) + 2;
			if (child_index > (this->size()-1)){ return -1;}

			return child_index;
		}

		int left_child_i(int i){
			/*****************************************************************
			 * @brief Get the index of the left child for an Entry
			 * @param i index of entry in container
			 * @return -1 if the entry has no left child
			 * @exception std::out_of_range if i would be out of bounds
			 * Time Complexity: O(1)
			 ****************************************************************/

			if (i >= this->size()){ throw std::out_of_range("Index out of bounds.");}

			const int child_index = (i * 2) + 1;
			if (child_index > (this->size()-1)){ return -1;}

			return child_index;
		}

		void swap_entries(int first_ind, int second_ind){
			/*****************************************************************
			 * @brief Swap pointers and the cached indeces at their Entries
			 * @param first_ind index of the first pointer
			 * @param second_ind index of the second pointer
			 * @exception std::out_of_range if any param would be out of bounds
			 * Time Complexity: O(1)
			 ****************************************************************/

			DEBUG_MSG("In swap function");
			DEBUG_MSG(first_ind);
			DEBUG_MSG(second_ind);

			if (first_ind >= this->size() || second_ind >= this->size()){ 
				throw std::out_of_range("Index out of bounds for swap.");
			}
			std::swap(container[first_ind], container[second_ind]);
			std::swap(container[first_ind]->index, container[second_ind]->index);
		}

	public:
		PriorityQueue(){}

		size_t size()  { return(container.size()); }
		bool is_empty(){ return(container.empty()); }

		std::string to_string(){
			/*****************************************************************
			 * @brief A primitive string representation of the PQ.
			 * @returns std::string representation of the PQ
			 * Time Complexity: O(n)
			 *****************************************************************/

			if (this->size() == 0) {return std::string("[EMPTY]");}
			DEBUG_MSG("DEBUG: size() is not 0!");

			std::string return_string;
			for (int i = 0; i < size(); i++){
				if (is_power_of_two(i+1)){return_string += "\n";}
				std::string entry_string = (container[i])->to_string();
				return_string += entry_string;
			}

			// Eliminate first \n (1 is a power of 2)
			return_string.erase(return_string.begin(), return_string.begin()+1);
			return return_string;

		}

		void insert(K key, V value){
			/*****************************************************************
			 * @brief Creates Entry and inserts pointer to it in PQ
			 * Time Complexity: O(1) - O(log n)
			 ****************************************************************/

			DEBUG_MSG("INSERTING");

			int new_i = size();
			container.push_back(std::make_unique<Entry<K,V>>(key,value,new_i));

			int parent_i = this->parent_i(new_i);
			if (parent_i == -1) {return;}

			while (container[new_i]->key < container[parent_i]->key){

				swap_entries(new_i, parent_i);
				new_i = parent_i;
				parent_i = this->parent_i(new_i);

				if (parent_i == -1) {return;}
			}

			DEBUG_MSG("FINISHED INSERTING");
		}

		Entry<K,V> remove_min(){
			/*****************************************************************
			 * @brief pops the Entry with the smallest key
			 * @returns a copy of such entry
			 * @exception std::out_of_range if the PQ is empty
			 ****************************************************************/
			DEBUG_MSG("Removing minimum");
			
			if (size() == 0){ throw std::out_of_range("PQ is empty");}

			Entry<K,V> return_entry = *container.front();
			swap_entries(0,size()-1);

			//No need to delete the underlying element as the unique_ptr will
			//take care of that for us.
			container.pop_back();		

			int cur_i   = 0;

			for(int x = 0; x < size(); x++){

				DEBUG_MSG("In the loop for restoring H.O.P");

				int left_i  = left_child_i(cur_i);
				int right_i = right_child_i(cur_i);
				int swap_i;
				
				bool is_gt_left  = false;
				bool is_gt_right = false;


				// Children should never have an index of 0. If they do there is
				// a big problem someplace else in the code.
				if (left_i > 0){
					DEBUG_MSG("Has left child");
					is_gt_left  = container[cur_i]->key > container[left_i]->key;
					swap_i = left_i;	
				}
				if (right_i > 0){
					DEBUG_MSG("Has right child");
					is_gt_right = container[cur_i]->key > container[right_i]->key;
					swap_i = right_i;	
				}
				if (right_i > 0 && left_i > 0){
					DEBUG_MSG("Has left and right child");
					int right_key = container[right_i]->key;
					int left_key  = container[left_i]->key;
					swap_i = right_key > left_key ? left_i : right_i;
				}

				if (!is_gt_left && !is_gt_right){ return return_entry; } 

				this->swap_entries(cur_i, swap_i);
				cur_i = swap_i;
			}

			return return_entry;

		}


		Entry<K,V> min(){
			/*****************************************************************
			 * @brief returns a copy of the smallest Entry
			 * @return Entry<K,V>& to smallest in PQ. 
			 * @exception std::out_of_range if the PQ is empty
			 ****************************************************************/
			if (size() == 0){ throw std::out_of_range("Tried to get min from an empty list"); }
			Entry<K,V> return_entry = *container.front();
			return return_entry;
		}

};
#endif
