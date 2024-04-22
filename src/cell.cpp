#include "../incl/cell.hpp"
#include <string>
#include <iostream>

Cell::Cell(Position position, Contents contents):
	position 	{position}, 
	contents 	{contents},
	parent 		{nullptr}{}

Cell::Cell(Contents contents):
	position 	{-1,-1}, 
	contents 	{contents},
	parent 		{nullptr}{}

Cell::Cell():
	position 	{-1,-1}, 
	contents 	{Contents::UNINT},
	parent 		{nullptr}{
}

Position Cell::getPosition()              {return position;}
Contents Cell::getContents()              {return contents;}
Cell*    Cell::getParent()                {return parent;}

std::string posToString(Position pos){
	std::string return_string = std::string("");	
	return_string 
		= return_string
		+ "("
		+ std::to_string(pos.row)
		+ ","
		+ std::to_string(pos.col)
		+ ")";
	return return_string;
}

void	 Cell::setPosition(int row, int col){position = Position(row,col);}
void     Cell::setContents(Contents contents){contents = contents;};

bool     Cell::isBlocked()                {return contents == Contents::BLOCKED;}
bool     Cell::isGoal()                   {return contents == Contents::GOAL;}

void     Cell::markOnPath()	              {contents = Contents::PATH;}
void     Cell::markAsBlocked()            {contents = Contents::BLOCKED;}
void     Cell::setParent(Cell& parent_cell){
	Cell* new_ptr = &parent_cell;
	this->parent = new_ptr;
}

std::string	Cell::toString() {
	std::string str_contents = std::string(1, static_cast<char>(contents));
	return str_contents;
}

bool Cell::operator==(Cell other) {
	return (
		(position.col 	== other.getPosition().col) &&
		(position.row 	== other.getPosition().row) &&
		(contents 		== other.getContents()) 	&&
		(parent 		== other.getParent())
	);
}

int Cell::toInt(){
	Position pos = this->getPosition();
	return(pos.row*10+pos.col);
}
