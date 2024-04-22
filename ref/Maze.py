from __future__ import annotations

from enum import Enum
from typing import NamedTuple

import random

###########################################################
class Contents(str, Enum):
    ''' create an enumeration to define what the visual contents 
        of a Cell are; using str as a "mixin" forces all the entries
        to be strings; using an enum means no cell entry can be 
        anything other than the options here '''
    EMPTY   = " "
    START   = "◎"  # "S"
    GOAL    = "◆"  # "G"
    BLOCKED = "░"  # "X"
    PATH    = "★"  # "*"

    def __str__(self) -> str: return self.value

###########################################################
class Position(NamedTuple):
    ''' named tuple that allows us to use .row and .col rather 
        than the less-easy-to-read [0] and [1] for accessing 
        values'''
    row: int
    col: int

    def __str__(self) -> str: return f"({self.row},{self.col})"

###########################################################
class Cell:
    ''' class that allows us to use Cell as a data type -- 
        row, column, & cell contents '''
    __slots__ = ('_position', '_contents', '_parent')

    def __init__(self, row: int, col: int, contents: Contents):
        self._position: Position = Position(row, col)
        self._contents: Contents = contents
        self._parent:   Cell     = None

    def __str__(self) -> str:
        contents = "[EMPTY]" if self._contents == Contents.EMPTY else self._contents
        result = f"({self._position.row},{self._position.col}): {contents}"
        if self._parent is not None: 
            result += f"({self._parent._position.row}, {self._parent._position.col})"
        return result

    def getPosition(self) -> Position:    return self._position
    def getParent(self)   -> Cell | None: return self._parent

    def setParent(self, parent: Cell) -> None:  self._parent = parent 
    def markOnPath(self)              -> None:  self._contents = Contents.PATH
    def markAsBlocked(self)           -> None:  self._contents = Contents.BLOCKED

    def isBlocked(self) -> bool:  return self._contents == Contents.BLOCKED
    def isGoal(self)    -> bool:  return self._contents == Contents.GOAL

    def __eq__(self, other: Cell) -> bool:
        return self._position == other._position and \
               self._contents == other._contents and \
               self._parent   == other._parent
        
###########################################################
class Maze:
    __slots__ = ('_num_rows', '_num_cols', '_start', '_goal', '_grid')

    def __init__(self, num_rows: int = 10, num_cols: int = 10, \
                       start: Position = Position(0,0), \
                       goal:  Position = Position(9,9), \
                       proportion_blocked: float = 0.2, \
                       debug: bool = False) -> None:
        ''' Maze initializer method
        Parameters:
            num_rows:           number of rows in the grid
            num_cols:           number of columns in the grid
            start:              Position object indicating (row,col) of the start cell
            goal:               Position object indicating (row,col) of the goal cell
            proportion_blocked: proportion of cells to be blocked (between 0.0 and 1.0)
            debug:              whether to use one of the Maze examples from slides
        Raises:
            TypeError if proportion_blocked is not float
            TypeError if start of goal is not Position
            ValueError if proportion_blocked is outside (0,1)
        '''
        if not isinstance(proportion_blocked, float):
            raise TypeError("proportion_blocked argument must be a float")
        if proportion_blocked < 0 or proportion_blocked > 1:
            raise ValueError("proportion_blocked argument must be a float b/w 0 and 1")
        if not isinstance(start, Position) or not isinstance(goal, Position):
            raise TypeError("start and goal must both be Position objects")

        if debug:  # set up 6X5 maze example from course slides
            num_rows = 6; num_cols = 5;
            start = Position(5, 0)
            goal  = Position(0, 4)

        self._num_rows: int  = num_rows
        self._num_cols: int  = num_cols

        # set up the start and goal Cell objects
        self._start: Cell = Cell(start.row, start.col, Contents.START)
        self._goal:  Cell = Cell(goal.row,  goal.col,  Contents.GOAL)

        # create a 2D list of Cell objects, all initially empty
        self._grid: list[ list[Cell] ] = \
            [ [Cell(r,c, Contents.EMPTY) for c in range(num_cols)] \
              for r in range(num_rows) ]

        # overwrite the appropriate locations with the start and goal Cells
        self._grid[start.row][start.col] = self._start
        self._grid[goal.row][goal.col]   = self._goal

        if debug:
            # for example from slides
            blocked_cells = [(1,1), (1,3), (2,4), (3,2), (5,1), (5,3), (5,4)]
            for pos in blocked_cells:
                p = Position(*pos)  # expand the pos tuple & pass to Position
                self._grid[p.row][p.col].markAsBlocked()
                #self._grid[pos[0]][pos[1]]._contents = Contents.BLOCKED
        else:
            # put blocks at random spots in the grid, using given proportion
            options: list[Cell] = [cell for row in self._grid for cell in row]  # 1D version of our 2D list
            options.remove(self._start)
            options.remove(self._goal)
            how_many: int = round((num_rows * num_cols - 2) * proportion_blocked)
            blocked: list[Cell] = random.sample(options, k = how_many)
            #print(f"type of blocked[0] = {type(blocked[0])}") 
            #print(f"blocked[0] = {blocked[0]}") 
            for cell in blocked:
                cell._contents.markAsBlocked()

    def showPath(self, goal: Cell) -> None:
        path = []
        cell = goal
        while cell._parent is not None:
            path.append(cell)
            cell = cell._parent
        path.append(cell)  # should be the start
        assert(cell == self._start)

        path.reverse()  # reverse the path list

        for cell in path:
            if cell not in [self._start, self._goal]:
                cell.markOnPath()

        print(self)  # print the newly marked-up maze


    def getStart(self) -> Cell: return self._start
    def getGoal(self)  -> Cell: return self._goal

    def __str__(self) -> str:
        ''' creates a str version of the Maze, showing contents, with cells
            delimited by vertical pipes
        Returns:
            a str representation of the Maze
        '''
        maze_str = ""
        for row in self._grid:  # row : list[Cell]
            maze_str += "|" + "|".join([cell._contents for cell in row]) + "|\n"
        return maze_str[:-1]  # remove the final \n

    def getSearchLocations(self, cell: Cell) -> list[Cell]:
        ''' returns a list of Cell objects of valid search locations
            (i.e., in the Maze, not already searched, not block) in
            the directions N,S,W,E of the given cell
        Parameters:
            cell: the current cell searching from
        Returns:
            a list of Cell objects of valid Cells to explore
        '''
        pass

    def dfs(self) -> Cell | None:
        ''' implements depth-first search (using a Stack)
        Returns:
            a Cell object corresponding to the goal, if a valid path exists 
            from the start to the goal;  or None, if no path exists
        '''
        pass



###########################################################
def main() -> None:
    m = Maze(debug = True)
    print(m)

    # hack the default (from slides) maze to
    # set up parents so we can reverse engineer
    # a path from start to goal


    #m._grid[0][4]._parent = m._grid[0][3]
    m._goal._parent       = m._grid[0][3]
    m._grid[0][3]._parent = m._grid[0][2]
    m._grid[0][2]._parent = m._grid[0][1]
    m._grid[0][1]._parent = m._grid[0][0]
    m._grid[0][0]._parent = m._grid[1][0]
    m._grid[1][0]._parent = m._grid[2][0]
    m._grid[2][0]._parent = m._grid[3][0]
    m._grid[3][0]._parent = m._grid[4][0]
    m._grid[4][0]._parent = m._start

    print()

    m.showPath(m._goal)


    #random.seed(8675309)
    #m = Maze(proportion_blocked = 0.1)
    #print(m)

    '''
    start = Cell(0,0, Contents.START)
    goal  = Cell(9,9, Contents.GOAL)
    empty = Cell(4,4, Contents.EMPTY)
    block = Cell(2,2, Contents.BLOCKED)
    path  = Cell(9,8, Contents.PATH)

    for c in [start,goal,empty,block,path]:
        print(c)
    print('-' * 40)

    print(f"before: {empty}")
    empty.markOnPath()
    print(f"after:  {empty}")


    print('-' * 40)

    for c in Contents:
        print(c)
    print('-' * 40)

    p1 = Position(3,4)
    p2 = Position(9,8)
    p3 = (7,7)

    print(f"row for p1 is: {p1.row}")
    print(f"col for p1 is: {p1.col}")
    print(f"row for p3 is: {p3[0]}")
    print(f"col for p3 is: {p3[1]}")
    print(f"p2 is: {p2}")
    '''


if __name__ == "__main__":
    main()
