#ifndef CELL_HPP
#define CELL_HPP
#include <string>
#include <cmath>

enum class Contents : char {
	EMPTY 	= ' ',
	START 	= 'S',
	GOAL 	= 'G',
	BLOCKED = 'x',
	PATH 	= '*',
	UNINT 	= '?'
};

struct Position {
	int row;
	int col;
};

std::string posToString(Position pos);

class Cell{

	private:
		Position 	position;
		Contents 	contents;
		Cell* 		parent;

	public:
		double g_n = -1;
		double h_n = -1;
		Cell(Contents contents);
		Cell(Position position, Contents contents);
		Cell();

		Position getPosition();	
		Contents getContents();
		Cell* getParent();

		void setPosition(int row, int col);
		void setContents(Contents contents);

		bool isBlocked();
		bool isGoal();

		void setParent(Cell& parent_cell);
		void markOnPath();
		void markAsBlocked();

		std::string	toString();
		bool operator==(Cell other);
		int toInt();		//used for indexing in map
};

#endif
