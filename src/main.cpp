// Stuff regarding chrono: 
// 		https://en.cppreference.com/w/cpp/chrono/duration
// And about timepoint substraction & steady_clock::now()
// 		https://en.cppreference.com/w/cpp/chrono/steady_clock/now

#include "../incl/maze.hpp"
#include <iostream>
#include <chrono>
#include <random>

typedef std::chrono::duration<double> Duration;
typedef std::chrono::microseconds us;
int TRIALS = 30;

class Stats {
	/*************************************************************************
	 * Used to keep track of the statistic for a particular benchmark        * 
	 * being run                                                             *
	 *************************************************************************/

	public:
		Duration total_duration =  Duration::zero();
		double  total_pushes       = 0;
		double  total_path_length  = 0;
		double  unreachable_count  = 0;

		void print(){
			auto average_duration = total_duration/TRIALS;
			std::cout
				<< "        "
				<< "Average Duration    : "
				//Casting this leads to inaccuracy.
				<< std::chrono::duration_cast<us>(average_duration).count()
				<< "ms"
				<< "\n        "
				<< "Average Pushes      : "
				<< total_pushes/TRIALS
				<< "\n        "
				<< "Average Path Length : "
				<< total_path_length/(TRIALS - unreachable_count)
				<< "\n";
		}

		void update(Maze maze, Duration duration){
			if (maze.path_found){
				total_duration    += duration;
				total_pushes      += maze.push_count;
				total_path_length += maze.path_length;
			} else {
				unreachable_count += 1;
			}
		}
};


void benchmarkAlgorithm(
		int                    rows, 
		int                    cols,
		float                  proportion){;
	/*************************************************************************
	 * Benchmarks a path-finding algorithm. The function is to be passed as  *
	 * a pointer and must be a static member function (or a non-member       *
	 * function),                                                            *
	 *************************************************************************/

	Stats dfs_stats;
	Stats bfs_stats;
	Stats a_stats;

	for (int i: std::views::iota(0,TRIALS)){
		std::mt19937 rng(i);

		int start_row = ((i * rng()) % (rows-1)); 		
		int   end_row = ((i * rng()) % (rows-1)); 		
		int start_col = ((i * rng()) % (cols-1)); 		
		int   end_col = ((i * rng()) % (cols-1)); 		

		if (start_row == end_row){start_row = (start_row + 1) % (rows-1);}
		if (start_col == end_col){start_col = (start_col + 1) % (cols-1);}

		Position start_pos(start_row, start_col);
		Position end_pos(end_row, end_col);
		
		std::cout << "Start Trial \n";
		Maze maze = Maze(start_pos, end_pos, rows, cols, i, proportion);

		maze.resetStats();

		auto start       = std::chrono::steady_clock::now();
		Maze::bfs(&maze);
		auto end         = std::chrono::steady_clock::now();
		auto it_duration = end-start;
		bfs_stats.update(maze, it_duration);
		std::cout << "bfs version: \n" << maze.toString() << "\n";

		maze.resetStats();

		start       = std::chrono::steady_clock::now();
		Maze::dfs(&maze);
		end         = std::chrono::steady_clock::now();
		it_duration = end-start;
		dfs_stats.update(maze, it_duration);
		std::cout << "dfs version: \n" << maze.toString() << "\n";

		maze.resetStats();

		start       = std::chrono::steady_clock::now();
		Maze::a_star(&maze);
		end         = std::chrono::steady_clock::now();
		it_duration = end-start;
		a_stats.update(maze, it_duration);
		std::cout << "a_star version: \n" << maze.toString() << "\n";

		maze.resetStats();
	}
	std::cout << "DFS Benchmark: \n";
	dfs_stats.print();
	std::cout << "BFS Benchmark: \n";
	bfs_stats.print();
	std::cout << "A Star Benchmark: \n";
	a_stats.print();
}


int main(){

	std::cout << "Average maze instantiation time in microseconds: "; {

		auto total_duration    = std::chrono::duration<double>::zero();
		const auto start       = std::chrono::steady_clock::now();

		for (int i = 0; i<TRIALS; i++){
			Maze maze(Position(5,5), Position (13,12), 20, 20, i, .2);
		}

		const auto end         = std::chrono::steady_clock::now();
		const auto it_duration =  end-start;

		total_duration += it_duration;

		auto average_duration = total_duration/TRIALS;
		std::cout << std::chrono::duration_cast<us>(average_duration).count() << "\n";
	}

	benchmarkAlgorithm(30, 30, .25);

}
