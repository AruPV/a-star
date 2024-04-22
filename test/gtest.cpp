#include <cstdlib>
#include <ranges>
#include <gtest/gtest.h>
#include "../incl/cell.hpp"
#include "../incl/maze.hpp"


class CellTest : public testing::Test {
	protected:
		Cell empty_cell   = Cell(Position(0,0), Contents::EMPTY);
		Cell start_cell   = Cell(Position(0,0), Contents::START);
		Cell goal_cell    = Cell(Position(0,0), Contents::GOAL);
		Cell blocked_cell = Cell(Position(0,0), Contents::BLOCKED);
		Cell path_cell    = Cell(Position(0,0), Contents::PATH);
};

class MazeTest : public testing::Test {
	protected:
		Maze default_maze = Maze(Position(0,0), Position(9,9),  10,  10, true, 0.2);
		Maze two_two_maze = Maze(Position(0,0), Position(1,1),   2,   2, true, 0.2);
		Maze one_two_maze = Maze(Position(0,0), Position(0,1),   1,   2, 1313, 0.2);
};


//					****** CELL TESTS ******

TEST_F(CellTest, empty_cell){
	EXPECT_EQ(empty_cell.getContents(), Contents::EMPTY);
	EXPECT_EQ(empty_cell.isBlocked(), false);
	EXPECT_EQ(empty_cell.isGoal(), false);
	EXPECT_EQ(empty_cell.toString(), " ");
}

TEST_F(CellTest, start_cell){
	EXPECT_EQ(start_cell.getContents(), Contents::START);
	EXPECT_EQ(start_cell.isBlocked(), false);
	EXPECT_EQ(start_cell.isGoal(), false);
	EXPECT_EQ(start_cell.toString(), "S");
}

TEST_F(CellTest, goal_cell){
	EXPECT_EQ(goal_cell.getContents(), Contents::GOAL);
	EXPECT_EQ(goal_cell.isBlocked(), false);
	EXPECT_EQ(goal_cell.isGoal(), true);
	EXPECT_EQ(goal_cell.toString(), "G");
}

TEST_F(CellTest, blocked_cell){
	EXPECT_EQ(blocked_cell.getContents(), Contents::BLOCKED);
	EXPECT_EQ(blocked_cell.isBlocked(), true);
	EXPECT_EQ(blocked_cell.isGoal(), false);
	EXPECT_EQ(blocked_cell.toString(), "x");
}

TEST_F(CellTest, path_cell){
	EXPECT_EQ(path_cell.getContents(), Contents::PATH);
	EXPECT_EQ(path_cell.isBlocked(), false);
	EXPECT_EQ(path_cell.isGoal(), false);
	EXPECT_EQ(path_cell.toString(), "*");
}

//					****** MAZE TESTS ******
// --- Default Maze

TEST_F(MazeTest, check_maze_initialized){
	EXPECT_EQ(default_maze.getSize(),100);
}

TEST_F(MazeTest, check_all_initialized_on_default_maze){
	for (int row_i: std::views::iota(0, 10)){
		for (int col_i: std::views::iota(0, 10)){
			EXPECT_NE(default_maze.getCell(row_i, col_i).getContents(), Contents::UNINT);
		}
	}
}

TEST_F(MazeTest, string_rep_on_default_maze){
	EXPECT_EQ(default_maze.toString(), "| S |   |   |   |   |   | x | x |   |   |\n|   |   | x |   |   | x |   | x |   |   |\n|   |   |   |   | x |   |   |   |   |   |\n|   |   |   |   |   |   |   |   |   |   |\n|   |   |   |   |   |   | x |   | x |   |\n|   | x |   |   | x | x |   |   |   |   |\n|   |   |   |   | x |   | x |   |   |   |\n|   |   | x |   |   |   |   | x |   |   |\n|   |   |   |   |   |   |   |   | x |   |\n| x |   |   |   | x |   | x | x |   | G |");



	// Maze should print like this:
	//
	//| S |   |   |   |   |   | x | x |   |   |
	//|   |   | x |   |   | x |   | x |   |   |
	//|   |   |   |   | x |   |   |   |   |   |
	//|   |   |   |   |   |   |   |   |   |   |
	//|   |   |   |   |   |   | x |   | x |   |
	//|   | x |   |   | x | x |   |   |   |   |
	//|   |   |   |   | x |   | x |   |   |   |
	//|   |   | x |   |   |   |   | x |   |   |
	//|   |   |   |   |   |   |   |   | x |   |
	//| x |   |   |   | x |   | x | x |   | G |

}

TEST_F(MazeTest, get_search_locations_on_default_maze){
	EXPECT_EQ(default_maze.getCell(0, 0).getContents(), Contents::START );
	EXPECT_EQ(default_maze.getCell(9, 9).getContents(), Contents::GOAL );
}

TEST_F(MazeTest, check_all_positions_on_default_maze){
	for (int cur_row: std::views::iota(0, 10)){
		for (int cur_col: std::views::iota(0, 10)){
			Position cell_pos = default_maze.getCell(cur_row, cur_col).getPosition(); 
			Position expected_pos = Position(cur_row,cur_col);
			EXPECT_EQ(cell_pos.row,expected_pos.row);
			EXPECT_EQ(cell_pos.col,expected_pos.col);
		}
	}
}

TEST_F(MazeTest, dfs_on_default_maze){
	EXPECT_EQ(Maze::dfs(&default_maze).has_value(), true);
	EXPECT_EQ(default_maze.getCell(0,0).getParent(), nullptr);
}


TEST_F(MazeTest, check_dfs_path_on_default_maze){
	EXPECT_EQ(Maze::dfs(&default_maze).has_value(), true);
	default_maze.showPath();
	EXPECT_EQ(default_maze.toString(), "| S | * | * | * |   |   | x | x |   |   |\n|   |   | x | * |   | x |   | x |   |   |\n| * | * | * | * | x |   |   |   |   |   |\n| * |   |   |   |   | * | * | * | * | * |\n| * | * | * | * | * | * | x |   | x | * |\n|   | x |   |   | x | x |   |   | * | * |\n|   |   |   |   | x |   | x |   | * |   |\n|   |   | x |   |   |   |   | x | * | * |\n|   |   |   |   |   |   |   |   | x | * |\n| x |   |   |   | x |   | x | x |   | G |");

	// Maze should print like this:
	//
	//| S | * | * | * |   |   | x | x |   |   |
	//|   |   | x | * |   | x |   | x |   |   |
	//| * | * | * | * | x |   |   |   |   |   |
	//| * |   |   |   |   | * | * | * | * | * |
	//| * | * | * | * | * | * | x |   | x | * |
	//|   | x |   |   | x | x |   |   | * | * |
	//|   |   |   |   | x |   | x |   | * |   |
	//|   |   | x |   |   |   |   | x | * | * |
	//|   |   |   |   |   |   |   |   | x | * |
	//| x |   |   |   | x |   | x | x |   | G |

	EXPECT_NE(default_maze.getCell(9,9).getParent(), nullptr);
}

TEST_F(MazeTest, bfs_on_default_maze){
	EXPECT_EQ(Maze::bfs(&default_maze).has_value(), true);
	EXPECT_EQ(default_maze.getCell(0,0).getParent(), nullptr);
}

TEST_F(MazeTest, check_bfs_path_on_default_maze){
	EXPECT_EQ(Maze::bfs(&default_maze).has_value(), true);
	default_maze.showPath();
	EXPECT_EQ(default_maze.toString(), "| S |   |   |   |   |   | x | x |   |   |\n| * |   | x |   |   | x |   | x |   |   |\n| * |   |   |   | x |   |   |   |   |   |\n| * | * | * | * | * | * | * | * |   |   |\n|   |   |   |   |   |   | x | * | x |   |\n|   | x |   |   | x | x |   | * |   |   |\n|   |   |   |   | x |   | x | * | * |   |\n|   |   | x |   |   |   |   | x | * | * |\n|   |   |   |   |   |   |   |   | x | * |\n| x |   |   |   | x |   | x | x |   | G |");

	// Maze should print like this:
	//
	//| S |   |   |   |   |   | x | x |   |   |
	//| * |   | x |   |   | x |   | x |   |   |
	//| * |   |   |   | x |   |   |   |   |   |
	//| * | * | * | * | * | * | * | * |   |   |
	//|   |   |   |   |   |   | x | * | x |   |
	//|   | x |   |   | x | x |   | * |   |   |
	//|   |   |   |   | x |   | x | * | * |   |
	//|   |   | x |   |   |   |   | x | * | * |
	//|   |   |   |   |   |   |   |   | x | * |
	//| x |   |   |   | x |   | x | x |   | G |

	EXPECT_NE(default_maze.getCell(9,9).getParent(), nullptr);
}

//Illegal mazes

TEST_F (MazeTest, initialize_invalid_size_maze){
	EXPECT_THROW(Maze(Position(0,0),Position(0,0), 1, 1, true, 0.2), std::invalid_argument);
}

TEST_F (MazeTest, start_and_end_same){
	EXPECT_THROW(Maze(Position(0,0),Position(0,0), 2, 2, true, 0.2), std::invalid_argument);
}

TEST_F (MazeTest, start_row_outside_bounds){
	EXPECT_THROW(Maze(Position(-1,0),Position(0,0), 2, 2, true, 0.2), std::invalid_argument);
}

TEST_F (MazeTest, start_col_outside_bounds){
	EXPECT_THROW(Maze(Position(1,-1),Position(0,0), 2, 2, true, 0.2), std::invalid_argument);
}

TEST_F (MazeTest, goal_row_outside_bounds){
	EXPECT_THROW(Maze(Position(0,0),Position(-1,0), 2, 2, true, 0.2), std::invalid_argument);
}

TEST_F (MazeTest, goal_col_outside_bounds){
	EXPECT_THROW(Maze(Position(0,0),Position(0,-1), 2, 2, true, 0.2), std::invalid_argument);
}

// --- 1x2 Maze

TEST_F(MazeTest, check_all_initialized_on_one_by_two_maze){
	for (int row_i: std::views::iota(0, 1)){
		for (int col_i: std::views::iota(0, 2)){
			EXPECT_NE(one_two_maze.getCell(row_i, col_i).getContents(), Contents::UNINT);
		}
	}
}

TEST_F(MazeTest, string_rep_on_one_by_two_maze){
	EXPECT_EQ(one_two_maze.toString(), "| S | G |");

	// Maze should print like this:
	//
	//| S | G | 
}

TEST_F(MazeTest, get_search_locations_on_one_by_two_maze){
	EXPECT_EQ(one_two_maze.getCell(0, 0).getContents(), Contents::START );
	EXPECT_EQ(one_two_maze.getCell(0, 1).getContents(), Contents::GOAL );
}

TEST_F(MazeTest, check_all_positions_on_one_by_two_maze){
	for (int cur_row: std::views::iota(0, 1)){
		for (int cur_col: std::views::iota(0, 2)){
			Position cell_pos = one_two_maze.getCell(cur_row, cur_col).getPosition(); 
			Position expected_pos = Position(cur_row,cur_col);
			EXPECT_EQ(cell_pos.row,expected_pos.row);
			EXPECT_EQ(cell_pos.col,expected_pos.col);
		}
	}
}

TEST_F(MazeTest, dfs_on_one_by_two_maze){
	EXPECT_EQ(Maze::dfs(&default_maze).has_value(), true);
	EXPECT_EQ(one_two_maze.getCell(0,0).getParent(), nullptr);
}

TEST_F(MazeTest, a_star_on_default_maze){
	EXPECT_EQ(Maze::a_star(&default_maze).has_value(), true);
	EXPECT_EQ(default_maze.getCell(0,0).getParent(), nullptr);
}

TEST_F(MazeTest, check_a_star_path_on_default_maze){
	EXPECT_EQ(Maze::a_star(&default_maze).has_value(), true);
	default_maze.showPath();
	EXPECT_EQ(default_maze.toString(), "| S |   |   |   |   |   | x | x |   |   |\n| * | * | x |   |   | x |   | x |   |   |\n|   | * | * | * | x |   |   |   |   |   |\n|   |   |   | * | * | * | * | * |   |   |\n|   |   |   |   |   |   | x | * | x |   |\n|   | x |   |   | x | x |   | * | * | * |\n|   |   |   |   | x |   | x |   |   | * |\n|   |   | x |   |   |   |   | x |   | * |\n|   |   |   |   |   |   |   |   | x | * |\n| x |   |   |   | x |   | x | x |   | G |");

	// Maze should print like this:
	//
	//| S |   |   |   |   |   | x | x |   |   |
	//| * | * | x |   |   | x |   | x |   |   |
	//|   | * | * | * | x |   |   |   |   |   |
	//|   |   |   | * | * | * | * | * |   |   |
	//|   |   |   |   |   |   | x | * | x |   |
	//|   | x |   |   | x | x |   | * | * | * |
	//|   |   |   |   | x |   | x |   |   | * |
	//|   |   | x |   |   |   |   | x |   | * |
	//|   |   |   |   |   |   |   |   | x | * |
	//| x |   |   |   | x |   | x | x |   | G |

	EXPECT_NE(default_maze.getCell(9,9).getParent(), nullptr);
}
