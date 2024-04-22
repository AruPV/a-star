#ifndef MAZE_CPP
#define MAZE_CPP
#include <vector>
#include <optional>
#include <map>
#include "cell.hpp"
#include "stack.hpp"
#include "queue.hpp"
#include "../incl/priority-queue.hpp"

class Maze {
	private:

		Position          start;
		Position          goal;
		std::vector<Cell> grid;		//Vector allows size definition at runtime.
		size_t            rows;
		size_t            cols;
		

		// The queues and stacks are pointer instantiated because vectors cannot
		// be instantiated with references.
		void pushSearchLocations(
			Cell*                cell, 
			Stack<Cell*>&        search_stack, 
			std::map<int, bool>& searched_index
		);

		// Queue overload for BFS
		void pushSearchLocations(
			Cell*                cell, 
			Queue<Cell*>&        search_queue, 
			std::map<int, bool>& searched_index
		);

		void pushSearchLocations(
			Cell*                         cell, 
			PriorityQueue<double, Cell*>& search_queue, 
			std::map<Position, double>&   searched_index
		);

		void resetStats();
		void updatePath();

	public:

		bool              path_found = false;
		int               path_length;
		int               push_count;

		Maze(
			Position 	start_pos           = Position(0,0),
			Position 	goal_pos            = Position(9,9),
			size_t		rows                = 10,
			size_t		cols                = 10,
			int	        debug_seed          = -1,
			float		blocked_proportion  = 0.2
		);

		void   showPath();
		Cell&  getCell(int row, int col);
		size_t getRows();
		size_t getCols();
		size_t getSize();
		double manhattan(Cell* n);

		// These are pointers instead of references because you cannot have 
		// optional references in c++.
		static std::optional<Cell*> dfs(Maze* maze);
		static std::optional<Cell*> bfs(Maze* maze);
		static std::optional<Cell*> a_star(Maze* maze);

		std::string toString();
};



#endif
