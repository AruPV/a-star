#ifndef STACK_HPP
#define STACK_HPP
#include <vector>
#include <string>
#include <ranges>
#include "../incl/cell.hpp"

template <typename T>
class Stack{

	private:
		std::vector<T> data;	
	public:
		Stack(){}
		size_t getSize()      {return data.size();}
		void   push(T element){data.push_back(element);}
		T&     top()          {return data.top();}
		T      pop()          {
			T return_val = data.back();
			data.pop_back();
			return return_val;
		}
		bool   isEmpty()      {return (data.size() == 0);}

		std::string toString(){
			if (this->isEmpty()){return "[EMPTY]";}
			std::string result = "---";
			// Loop w iterators
			for (auto it = data.end(); it != data.begin(); it++){
				// iterator dereference with the * operator
				result += *it + "---\n";
			}
			return result;
		}


};

// So, you can't do partial template specialization for a function in c++ as
// of c++20, but you CAN do a partial template specialization for the whole
// class.
template <typename T>
class Stack<T*>{

	private:
		std::vector<T*> data;	
	public:
		Stack(){}
		size_t getSize()       {return data.size();}
		void   push(T* element){data.push_back(element);}
		T*&    top()           {return data.top();}
		T*     pop()           {
			T* return_val = data.back();
			data.pop_back();
			return return_val;
		}
		bool   isEmpty()      {return (data.size() == 0);}

		std::string toString(){
			if (this->isEmpty()){return "[EMPTY]";}
			std::string result = "---";
			// Loop w iterators
			for (auto it = data.end(); it != data.begin(); it++){
				result = result + "\n" + **it + "\n---";
			}
			return result;
		}


};

// I should really just add an operator++ method specialized for
// char[] for the cell class...
template<>
inline std::string Stack<Cell*>::toString(){
	if (this->isEmpty()){return "[EMPTY]";}
	std::string result = "+-----+\n";
	for (int i= this->getSize()-1; i > -1; i--){
		if (i<this->getSize()-1){ result = result + "---\n";}
		result = result 
			+ posToString((*data[i]).getPosition())
			+ "\n";
	}
	result = result + "+-----+";
	return result;
}

#endif
