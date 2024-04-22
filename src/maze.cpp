//Macros
#define NDEBUG
#ifndef NDEBUG 
#define DEBUG_MSG(str) do { std::cout << str << "\n"; } while (false)
#else
#define DEBUG_MSG(str) do {} while(false)
#endif

#include <ranges>
#include <map>
#include <algorithm>
#include <random>
#include <array> 
#include <iostream>
#include "../incl/queue.hpp"
#include "../incl/stack.hpp"
#include "../incl/maze.hpp"
#include "../incl/priority-queue.hpp"

/*************************************************************************************************/

Maze::Maze
	(Position start_pos
	,Position goal_pos
	,size_t	  rows
	,size_t	  cols
	,int      debug_seed 
	,float    blocked_proportion
	)
	:start (start_pos)
	,goal  (goal_pos)
	,rows  (rows)
	,cols  (cols){

	DEBUG_MSG("---   DEBUG ON   ---");
	DEBUG_MSG("Uncomment preprocessor NDEBUG definition to turn off.");

	DEBUG_MSG("Initializing maze");
	// Error handling
	if (rows == 1 && cols == 1){ throw std::invalid_argument("Invalid Maze size");}
	if (start_pos.row == goal_pos.row && start_pos.col == goal_pos.col){ 
		throw std::invalid_argument("Goal and Start must be at different positions");
	}
	bool less_than_zero_pos = (
		start_pos.row < 0 ||
		start_pos.col < 0 ||
		goal_pos.row  < 0 || 
		goal_pos.col  < 0 );

	bool bigger_than_maze_pos = (
		start_pos.row > rows-1 ||
		goal_pos.row  > rows-1 || 
		start_pos.col > cols-1 ||
		goal_pos.col  > cols-1 );

	if (less_than_zero_pos || bigger_than_maze_pos){
		throw std::invalid_argument("Illegal positions for size of maze");
	}
	
	// Variable initialization
	int blocked_count       = floor(rows*cols*blocked_proportion);
	int variable_cell_count = (rows*cols)-2;
	int start_i             = start_pos.row * cols + start_pos.col;
	int goal_i              = goal_pos.row  * cols + goal_pos.col;
	std::random_device rd; 		// Non-deterministic random device for seed
	std::mt19937 rng(rd());		// Merssene Twister from algorithm library. 
	if (debug_seed != -1){rng = std::mt19937(debug_seed);}

	DEBUG_MSG("Creating maze skeleton.");

	//Add all non-start or end cells
	for (int i: std::views::iota(0, (variable_cell_count))){
		Contents cell_contents = (i < blocked_count)
			? Contents::BLOCKED
			: Contents::EMPTY;
		grid.push_back(Cell(cell_contents));
	}

	DEBUG_MSG("Shuffling skeleton.");

	// Shuffle maze
	std::shuffle(std::begin(grid), std::end(grid), rng);
	
	DEBUG_MSG("Inserting start and end.");
	
	// Add start and goal.
	if (start_i < goal_i){
		grid.insert(grid.begin()+start_i, Cell(Contents::START));
		grid.insert(grid.begin()+goal_i, Cell(Contents::GOAL));
	} else {
		grid.insert(grid.begin()+goal_i , Cell(Contents::GOAL));
		grid.insert(grid.begin()+start_i, Cell(Contents::START));
	}

	DEBUG_MSG("Initializing positions.");

	// Initialize positions
	for (int row_i=0; row_i<rows; row_i++){
		for (int col_i=0; col_i<cols; col_i++){
			grid[(row_i*cols)+col_i].setPosition(row_i,col_i);
		}
	}
}


Cell& Maze::getCell(int row, int col){
	return grid[(row*cols)+col];
}


size_t Maze::getRows(){return rows;}
size_t Maze::getCols(){return cols;}
size_t Maze::getSize(){return (cols*rows);}

void Maze::pushSearchLocations(
	Cell*                cell, 
	Stack<Cell*>&        search_stack, 
	std::map<int, bool>& searched_index){
	/*******************************************************************
	* Goes through the adjacent cells to a cell and pushes those that  *
	* are "valid" (not blocked or in path) into a stack that is passed *
	* by the user.                                                     *
	********************************************************************/
	DEBUG_MSG(std::string("Called pushSearchLocations on the cell at: ")
			+ posToString((*cell).getPosition()));

	searched_index[(*cell).toInt()] = true;
	//None of these can be references because they're rvalues.
	Position start_pos   = cell->getPosition();
	Position west_pos  = Position(start_pos.row  , start_pos.col-1);
	Position east_pos  = Position(start_pos.row  , start_pos.col+1);
	Position north_pos = Position(start_pos.row-1, start_pos.col);
	Position south_pos = Position(start_pos.row+1, start_pos.col);

	//std::array is a pointer + size. Worth the small memory cost over c-style
	//arrays in most instances
	std::array<Position, 4> directions;		
	directions[0] = north_pos;
	directions[1] = south_pos;
	directions[2] = west_pos;
	directions[3] = east_pos;

	// Go through each item in directions
	for (int i = 0; i<4; i++){
		DEBUG_MSG("In directions loop for the position:"
				+ posToString(directions[i]));
		Position cur_pos = directions[i];
		if (cur_pos.col < 0 || cur_pos.row < 0)                    {continue;}
		if (cur_pos.col >= this->cols || cur_pos.row >= this->rows){continue;}

		Cell* cur_cell  = &this->getCell(cur_pos.row, cur_pos.col);
		bool is_searched = searched_index[(*cur_cell).toInt()]; 
		bool is_blocked  = cur_cell->getContents() == Contents::BLOCKED; 

		if (is_blocked || is_searched)                             {continue;}

		searched_index[(*cur_cell).toInt()] = 1;
		cur_cell->setParent(*cell);
		search_stack.push(cur_cell);
		this->push_count += 1;
		DEBUG_MSG("    Pushed successfully");
	}
		DEBUG_MSG("Exited directions Loop");
}

void Maze::pushSearchLocations(
	Cell*                cell, 
	Queue<Cell*>&        search_queue, 
	std::map<int, bool>& searched_index){
	/*******************************************************************
	* Goes through the adjacent cells to a cell and pushes those that  *
	* are "valid" (not blocked or in path) into a queue that is passed *
	* by the user.                                                     *
	********************************************************************/
	DEBUG_MSG(std::string("Called pushSearchLocations on the cell at: ")
			+ posToString((*cell).getPosition()));

	searched_index[(*cell).toInt()] = true;
	//None of these can be references because they're rvalues.
	Position start_pos = cell->getPosition();
	Position west_pos  = Position(start_pos.row  , start_pos.col-1);
	Position east_pos  = Position(start_pos.row  , start_pos.col+1);
	Position north_pos = Position(start_pos.row-1, start_pos.col);
	Position south_pos = Position(start_pos.row+1, start_pos.col);

	//std::array is a pointer + size. Worth the small memory cost over c-style
	//arrays in most instances
	std::array<Position, 4> directions;		
	directions[0] = north_pos;
	directions[1] = south_pos;
	directions[2] = west_pos;
	directions[3] = east_pos;

	// Go through each item in directions
	for (int i = 0; i<4; i++){
		DEBUG_MSG("In directions loop for the position:"
				+ posToString(directions[i]));
		Position cur_pos = directions[i];
		if (cur_pos.col < 0 || cur_pos.row < 0)                    {continue;}
		if (cur_pos.col >= this->cols || cur_pos.row >= this->rows){continue;}

		Cell* cur_cell  = &this->getCell(cur_pos.row, cur_pos.col);
		bool is_searched = searched_index[(*cur_cell).toInt()]; 
		bool is_blocked  = cur_cell->getContents() == Contents::BLOCKED; 

		if (is_blocked || is_searched)                             {continue;}

		searched_index[(*cur_cell).toInt()] = 1;
		cur_cell->setParent(*cell);
		search_queue.push(cur_cell);
		this->push_count += 1;
		DEBUG_MSG("    Pushed successfully");
	}
		DEBUG_MSG("Exited directions Loop");
}

double Maze::manhattan(Cell* n){

	Cell& goal = this->getCell(this->goal.row,this->goal.col);
	Position goal_pos = goal.getPosition();
	Position n_pos = n->getPosition();

	double row_diff = std::abs(goal_pos.row - n_pos.row);
	double col_diff = std::abs(goal_pos.col - n_pos.col);
		

	return (row_diff + col_diff);
}

void Maze::pushSearchLocations(
	Cell*                         n, 
	PriorityQueue<double, Cell*>& to_explore, 
	std::map<Position, double>&   explored){

	
	DEBUG_MSG(std::string("Called pushSearchLocations on the cell at: ")
			+ posToString((*cell).getPosition()));

	//None of these can be references because they're rvalues.
	Position start_pos = n->getPosition();
	Position west_pos  = Position(start_pos.row  , start_pos.col-1);
	Position east_pos  = Position(start_pos.row  , start_pos.col+1);
	Position north_pos = Position(start_pos.row-1, start_pos.col);
	Position south_pos = Position(start_pos.row+1, start_pos.col);

	//std::array is a pointer + size. Worth the small memory cost over c-style
	//arrays in most instances
	std::array<Position, 4> directions;		
	directions[0] = north_pos;
	directions[1] = south_pos;
	directions[2] = west_pos;
	directions[3] = east_pos;

	// Go through each item in directions
	for (int i = 0; i<4; i++){
		DEBUG_MSG("In directions loop for the position:"
				+ posToString(directions[i]));
		Position m_pos = directions[i];
		if (m_pos.col < 0 || m_pos.row < 0)                    {continue;}
		if (m_pos.col >= this->cols || m_pos.row >= this->rows){continue;}

		Cell* m         = &this->getCell(m_pos.row, m_pos.col);

		bool is_blocked = m->getContents() == Contents::BLOCKED; 
		if (is_blocked) {continue;}

		//Get Updated values
		double updated_g_m = explored[n->getPosition()]+1;
		double updated_h_m = this->manhattan(m);		//This is not needed i think
		double updated_f_m = updated_h_m + updated_g_m;

		double cur_g_m  = explored[m->getPosition()]; 
		if (!(updated_g_m < cur_g_m) && cur_g_m != -1){continue;}
		
		//Update state in cell
		m->setParent(*n);
		m->h_n = updated_h_m;	
		m->g_n = updated_g_m;

		//Update state in explored and to_explore
		explored[m->getPosition()] = updated_g_m;
		to_explore.insert(updated_f_m, m);

		//Book-keeping
		this->push_count += 1;
		DEBUG_MSG("    Pushed successfully");
	}
		DEBUG_MSG("Exited directions Loop");

}

void Maze::resetStats(){
	this->path_length = 0;
	this->push_count  = 0;
	this->path_found  = false;
}

void Maze::updatePath(){
	/****************************************************************
	 * Goes throught the path, from end to beginning, and marks the *
	 * cells in path as such. If there is no path, then the method  *
	 * does not change anything                                     *
	 ****************************************************************/
	Cell* cur_cell = &this->getCell(goal.row,goal.col);
	while (cur_cell->getParent() != nullptr){
		DEBUG_MSG(std::string("Inside loop for: ") + posToString(cur_cell->getPosition()));
		Contents cur_contents = cur_cell->getContents();
		if (cur_contents != Contents::GOAL){
			this->path_length += 1;
			cur_cell->markOnPath();
		}
		cur_cell = cur_cell->getParent();
	}
}

//////////////////////////////////////////////////////////////////////////////
std::optional<Cell*> Maze::a_star(Maze* maze){

	PriorityQueue<double, Cell*> to_explore;
	std::map<Position, double>   explored;

	Cell* n = &maze->getCell(maze->start.row, maze->start.col);
	double g_n = n->g_n = 0.0;
	double h_n = n->h_n = maze->manhattan(n);
	double f_n = g_n + g_n;
	
	to_explore.insert(f_n, n);
	explored[n->getPosition()] = g_n;

	while (!to_explore.is_empty()){
		Entry<double, Cell*> e = to_explore.remove_min();
		n = e.value;

		if (n == &maze->getCell(maze->goal.row, maze->goal.col)){
			maze->path_found = true;
			maze->updatePath();
			return n;
		}

		maze->pushSearchLocations(n, to_explore, explored);	
	}

	return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////
std::optional<Cell*> Maze::dfs(Maze* maze){
	/*****************************************************************
	 * Performs a Depth-first-search on the maze to find the goal    *
	 * from the start.                                               *
	 *****************************************************************/
	std::optional<Cell*> return_value;
	Stack<Cell*>         search_stack;
	Cell*                cur_cell  = &maze->getCell(maze->start.row, maze->start.col);
	Cell*                goal_cell = &maze->getCell(maze->goal.row , maze->goal.col);
	std::map<int, bool> searched_index;

	maze->resetStats();

	while (cur_cell != goal_cell){
		DEBUG_MSG("In DFS Loop");
		maze->pushSearchLocations(cur_cell, search_stack, searched_index);
		if (search_stack.isEmpty()){break;}
		cur_cell = search_stack.pop();
	}

	if (cur_cell == goal_cell){
		return_value = cur_cell;
		maze->path_found = true;
		maze->updatePath();
	}

	return return_value;
}

std::optional<Cell*> Maze::bfs(Maze* maze){
	/*****************************************************************
	 * Performs a Breath-first-search on the maze to find the goal   *
	 * from the start.                                               *
	 *****************************************************************/
	std::optional<Cell*> return_value;
	Queue<Cell*>         search_queue;
	Cell*                cur_cell  = &maze->getCell(maze->start.row, maze->start.col);
	Cell*                goal_cell = &maze->getCell(maze->goal.row , maze->goal.col);
	std::map<int, bool> searched_index;

	maze->resetStats();

	while (cur_cell != goal_cell){
		DEBUG_MSG("In DFS Loop");
		maze->pushSearchLocations(cur_cell, search_queue, searched_index);
		if (search_queue.isEmpty()){break;}
		cur_cell = search_queue.pop();
	}

	if (cur_cell == goal_cell){
		return_value = cur_cell;
		maze->path_found = true;
		maze->updatePath();
	}

	return return_value;
}


void Maze::showPath(){
	/****************************************************************
	 * Prints the map if there is a path, otherwise throws          *
	 ****************************************************************/
	DEBUG_MSG("In showPath");
	if (!this->path_found){throw std::invalid_argument("No path to show");}
	std::cout<< this->toString();

}

std::string Maze::toString(){
	std::cout << "in string";
	std::string return_str; 
	for (int row_i = 0; row_i < rows; row_i++){
		return_str.append("|");
		for (int col_i = 0; col_i < cols; col_i++){
			int cell_i = (row_i * cols) + col_i;
			return_str.append(" " + grid[cell_i].toString() + " |");
		}
		return_str.append("\n");
	}
	return_str.erase(return_str.end()-1, return_str.end());
	return return_str;
}
