#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <unordered_set>

/**
 * @brief  Read your map from stdin
 * @note   Input format: See project description
 * @param  &grid: You should first resize the grid! Modify it inplace.
 * @retval None
 */
void read_map(std::istream &is, std::vector<std::string> &grid)
{
    size_t row = 0;
    size_t col = 0;

    std::string line;

    getline(is, line);
    std::stringstream ss(line);
    ss >> col >> row; // Get the col and row from the first line.

    grid.resize(row, std::string(col, ' ')); // Resize the grid.

    for (size_t i = 0; i < row; ++i)
    {
        getline(is, line);

        grid[i] = line;
    }
}

struct space
{
    std::pair<size_t, size_t> Player;
    std::vector<std::pair<size_t, size_t>> boxes;
    std::string route;
    std::vector<std::pair<std::pair<size_t, size_t>, bool>> targetState;
    size_t targetRight;
    bool pushBox;
};

// Find the initial position of the person and the boxes.
space findItems(std::vector<std::string> &grid)
{
    space Items;
    bool Existed = false;

    for (size_t i = 0; i < grid.size(); ++i)
    {
        for (size_t j = 0; j < grid[i].size(); ++j)
        {
            if (grid[i][j] == 'S')
            {
                if ((i > 0) && (i < grid.size() - 1) && (j > 0) && (j < grid[i].size() - 1))
                {
                    if (!Existed)
                    {
                        Items.Player = {i, j};
                        Existed = true;
                        continue;
                    }
                    // More than one starting point.
                    else
                    {
                        std::cout << "No solution!" << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << "No solution!" << std::endl;
                    std::exit(0);
                }
            }
            if (grid[i][j] == 'B')
            {
                // B is in the corner.
                if (std::pair<size_t, size_t>{i, j} == std::pair<size_t, size_t>{1, 1} || std::pair<size_t, size_t>{i, j} == std::pair<size_t, size_t>{1, grid[i].size() - 2} || std::pair<size_t, size_t>{i, j} == std::pair<size_t, size_t>{grid.size() - 2, 1} || std::pair<size_t, size_t>{i, j} == std::pair<size_t, size_t>{grid.size() - 2, grid[i].size() - 2})
                {
                    std::cout << "No solution!" << std::endl;
                    std::exit(0);
                }

                if ((i > 0) && (i < grid.size() - 1) && (j > 0) && (j < grid[i].size() - 1))
                {
                    std::pair<size_t, size_t> box = {i, j};
                    Items.boxes.push_back(box);
                    continue;
                }
                else
                {
                    std::cout << "No solution!" << std::endl;
                    std::exit(0);
                }
            }
            if (grid[i][j] == 'R')
            {
                if ((i > 0) && (i < grid.size() - 1) && (j > 0) && (j < grid[i].size() - 1))
                {
                    std::pair<size_t, size_t> box = {i, j};
                    Items.boxes.push_back(box);
                }
                else
                {
                    std::cout << "No solution!" << std::endl;
                    std::exit(0);
                }
            }
        }
    }
    // No starting point.
    if (!Existed)
    {
        std::cout << "No solution!" << std::endl;
        std::exit(0);
    }

    return Items;
}

// Find all the targets' position.
std::vector<std::pair<std::pair<size_t, size_t>, bool>> findTarget(std::vector<std::string> &grid)
{
    std::vector<std::pair<std::pair<size_t, size_t>, bool>> targets;
    for (size_t i = 0; i < grid.size(); ++i)
    {
        for (size_t j = 0; j < grid[0].size(); ++j)
        {
            if (grid[i][j] == 'T')
            {
                if ((i > 0) && (i < grid.size() - 1) && (j > 0) && (j < grid[i].size() - 1))
                {
                    std::pair<size_t, size_t> target = {i, j};
                    targets.push_back({target, false});
                }
                else
                {
                    std::cout << "No solution!" << std::endl;
                    std::exit(0);
                }
            }
            if (grid[i][j] == 'R')
            {
                if ((i > 0) && (i < grid.size() - 1) && (j > 0) && (j < grid[i].size() - 1))
                {
                    std::pair<size_t, size_t> target = {i, j};
                    targets.push_back({target, true});
                }
                else
                {
                    std::cout << "No solution!" << std::endl;
                    std::exit(0);
                }
            }
        }
    }
    return targets;
}

// Judge whether all boxes have been successfully pushed to their target positions.
bool isSolved(const space &Current)
{
    if (Current.targetRight != Current.targetState.size())
    {
        return false;
    }

    return true;
}

std::string reverseRoute(const std::string &route)
{
    std::string rRoute = route;
    std::reverse(rRoute.begin(), rRoute.end());
    for (char &c : rRoute)
    {
        if (c == 'U')
        {
            c = 'D';
        }
        else if (c == 'D')
        {
            c = 'U';
        }
        else if (c == 'L')
        {
            c = 'R';
        }
        else
        {
            c = 'L';
        }
    }

    return rRoute;
}

using State = std::tuple<size_t, size_t, std::vector<std::pair<size_t, size_t>>>;

// Change the space type to a vector of int, making it easies to store in the set.
State addStateVisited(const space &State)
{
    std::vector<std::pair<size_t, size_t>> boxesSorted = State.boxes;
    std::sort(boxesSorted.begin(), boxesSorted.end());

    return std::make_tuple(State.Player.first, State.Player.second, boxesSorted);
}

bool isValidDirection(const std::vector<std::string> &grid, const space &state, char Direction)
{
    int LR = 0;
    int UD = 0;

    UD = (Direction == 'U') ? -1 : ((Direction == 'D') ? 1 : UD);
    LR = (Direction == 'L') ? -1 : ((Direction == 'R') ? 1 : LR);

    if (!state.pushBox)
    {
        if (!state.route.empty())
        {
            // if (Direction == 'U' && state.route.back() == 2)
            if (Direction == 'U' && state.route.back() == 'D')
            {
                return false;
            }
            // if (Direction == 'D' && state.route.back() == 1)
            if (Direction == 'D' && state.route.back() == 'U')
            {
                return false;
            }
            // if (Direction == 'L' && state.route.back() == 4)
            if (Direction == 'L' && state.route.back() == 'R')
            {
                return false;
            }
            // if (Direction == 'R' && state.route.back() == 3)
            if (Direction == 'R' && state.route.back() == 'L')
            {
                return false;
            }
        }
    }

    size_t newRow = (size_t)((int)state.Player.first + UD);
    size_t newCol = (size_t)((int)state.Player.second + LR);

    // If new position is wall, invalid.
    if (grid[newRow][newCol] == '#')
    {
        return false;
    }

    // If there was a box, and above the box was wall or another box, invalid.
    // if (grid[newRow][newCol] == 'B')
    auto it = std::find(state.boxes.begin(), state.boxes.end(), std::pair<size_t, size_t>(newRow, newCol));
    if (it != state.boxes.end())
    {
        if (((int)newRow + UD <= 0) || ((int)newRow + UD >= (int)grid.size()))
        {
            return false;
        }
        if (((int)newCol + LR <= 0) || ((int)newCol + LR >= (int)grid[0].size()))
        {
            return false;
        }
        if ((grid[(size_t)((int)newRow + UD)][(size_t)((int)newCol + LR)] == '#'))
        {
            return false;
        }
        size_t newboxRow = (size_t)((int)newRow + UD);
        size_t newboxCol = (size_t)((int)newCol + LR);
        auto boxnew = std::find(state.boxes.begin(), state.boxes.end(), std::pair<size_t, size_t>(newboxRow, newboxCol));
        if (boxnew != state.boxes.end())
        {
            return false;
        }
    }

    return true;
}

bool isValidDirectionBack(const std::vector<std::string> &grid, const space &state, char Direction)
{
    int LR = 0;
    int UD = 0;

    UD = (Direction == 'U') ? -1 : ((Direction == 'D') ? 1 : UD);
    LR = (Direction == 'L') ? -1 : ((Direction == 'R') ? 1 : LR);

    // if (!state.pushBox)
    // {
    //     if (!state.route.empty())
    //     {
    //         // if (Direction == 'U' && state.route.back() == 2)
    //         if (Direction == 'U' && state.route.back() == 'D')
    //         {
    //             return false;
    //         }
    //         // if (Direction == 'D' && state.route.back() == 1)
    //         if (Direction == 'D' && state.route.back() == 'U')
    //         {
    //             return false;
    //         }
    //         // if (Direction == 'L' && state.route.back() == 4)
    //         if (Direction == 'L' && state.route.back() == 'R')
    //         {
    //             return false;
    //         }
    //         // if (Direction == 'R' && state.route.back() == 3)
    //         if (Direction == 'R' && state.route.back() == 'L')
    //         {
    //             return false;
    //         }
    //     }
    // }

    size_t newRow = (size_t)((int)state.Player.first + UD);
    size_t newCol = (size_t)((int)state.Player.second + LR);

    // If new position is wall, invalid.
    if (grid[newRow][newCol] == '#')
    {
        return false;
    }

    // If new position is box, invalid.
    auto it = std::find(state.boxes.begin(), state.boxes.end(), std::pair<size_t, size_t>(newRow, newCol));
    if (it != state.boxes.end())
    {
        return false;
    }

    return true;
}

bool isDeadLock(const std::vector<std::string> &grid, const space &state)
{
    for (auto &box : state.boxes)
    {
        // .#.
        // #B.
        // ...
        if (grid[box.first - 1][box.second] == '#' && grid[box.first][box.second - 1] == '#')
        {
            if ((grid[box.first][box.second] != 'T') && (grid[box.first][box.second] != 'R'))
            {
                return true; // This state is a deadlock.
            }
        }
        // .#.
        // .B#
        // ...
        if (grid[box.first - 1][box.second] == '#' && grid[box.first][box.second + 1] == '#')
        {
            if ((grid[box.first][box.second] != 'T') && (grid[box.first][box.second] != 'R'))
            {
                return true; // This state is a deadlock.
            }
        }
        // ...
        // #B.
        // .#.
        if (grid[box.first + 1][box.second] == '#' && grid[box.first][box.second - 1] == '#')
        {
            if ((grid[box.first][box.second] != 'T') && (grid[box.first][box.second] != 'R'))
            {
                return true; // This state is a deadlock.
            }
        }
        // ...
        // .B#
        // .#.
        if (grid[box.first + 1][box.second] == '#' && grid[box.first][box.second + 1] == '#')
        {
            if ((grid[box.first][box.second] != 'T') && (grid[box.first][box.second] != 'R'))
            {
                return true; // This state is a deadlock.
            }
        }
    }

    for (size_t i = 0; i < state.boxes.size(); ++i)
    {
        for (size_t j = i + 1; j < state.boxes.size(); ++j)
        {
            if (((grid[state.boxes[i].first][state.boxes[i].second] == 'T') || (grid[state.boxes[i].first][state.boxes[i].second] == 'R')) && ((grid[state.boxes[j].first][state.boxes[j].second] == 'T') || (grid[state.boxes[j].first][state.boxes[j].second] == 'R')))
            {
                continue;
            }
            if ((state.boxes[i].first == state.boxes[j].first) && (std::abs((int)state.boxes[i].second - (int)state.boxes[j].second) == 1))
            {
                // ## or BB
                // BB or ##
                if (((grid[state.boxes[i].first - 1][state.boxes[i].second] == '#') && (grid[state.boxes[j].first - 1][state.boxes[j].second] == '#')) || ((grid[state.boxes[i].first + 1][state.boxes[i].second] == '#') && (grid[state.boxes[j].first + 1][state.boxes[j].second] == '#')))
                {
                    return true;
                }
            }
            else if ((state.boxes[i].second == state.boxes[j].second) && (std::abs((int)state.boxes[i].first - (int)state.boxes[j].first) == 1))
            {
                // #B or B#
                // #B or B#
                if (((grid[state.boxes[i].first][state.boxes[i].second - 1] == '#') && (grid[state.boxes[j].first][state.boxes[j].second - 1] == '#')) || ((grid[state.boxes[i].first][state.boxes[i].second + 1] == '#') && (grid[state.boxes[j].first][state.boxes[j].second + 1] == '#')))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

space moveNextState(const space &state, char Direction)
{
    space Next = state;
    Next.pushBox = false;

    int LR = 0;
    int UD = 0;

    UD = (Direction == 'U') ? -1 : ((Direction == 'D') ? 1 : UD);
    LR = (Direction == 'L') ? -1 : ((Direction == 'R') ? 1 : LR);

    // Update the position of the player.
    size_t newRow = (size_t)((int)Next.Player.first + UD);
    size_t newCol = (size_t)((int)Next.Player.second + LR);

    // Detect whether a box is pushed.
    auto it = std::find(Next.boxes.begin(), Next.boxes.end(), std::pair<size_t, size_t>(newRow, newCol));
    if (it != Next.boxes.end())
    {
        size_t newboxRow = (size_t)((int)newRow + UD);
        size_t newboxCol = (size_t)((int)newCol + LR);

        for (auto &item : Next.targetState)
        {
            // If already Right position, make this back to target 'T'.
            if ((item.first == std::pair<size_t, size_t>(newRow, newCol)) && item.second == true)
            {
                item.second = false;
                Next.targetRight--;
            }
            // If the new position of the box is target, mark it as R.
            if ((item.first == std::pair<size_t, size_t>(newboxRow, newboxCol)) && item.second == false)
            {
                item.second = true;
                Next.targetRight++;
            }
        }

        (*it).first = newboxRow;
        (*it).second = newboxCol;

        Next.pushBox = true;
    }

    Next.Player.first = newRow;
    Next.Player.second = newCol;

    // int newDirect = char2Int(Direction);
    // Next.route.push_back(newDirect);

    Next.route += Direction;
    // std::cout << Next.route << std::endl;

    return Next;
}

space moveNextStateBack(const space &state, char Direction, bool pullBox)
{
    space Next = state;
    Next.pushBox = false;

    int LR = 0;
    int UD = 0;

    UD = (Direction == 'U') ? -1 : ((Direction == 'D') ? 1 : UD);
    LR = (Direction == 'L') ? -1 : ((Direction == 'R') ? 1 : LR);

    // Update the position of the player.
    size_t newRow = (size_t)((int)Next.Player.first + UD);
    size_t newCol = (size_t)((int)Next.Player.second + LR);

    size_t originBoxRow = (size_t)((int)Next.Player.first - UD);
    size_t originBoxCol = (size_t)((int)Next.Player.second - LR);

    // Detect whether a box is pulled.
    auto it = std::find(Next.boxes.begin(), Next.boxes.end(), std::pair<size_t, size_t>(originBoxRow, originBoxCol));
    if ((it != Next.boxes.end()) && pullBox)
    {
        size_t newboxRow = Next.Player.first;
        size_t newboxCol = Next.Player.second;

        for (auto &item : Next.targetState)
        {
            // If already Right position, make this back to target 'T'.
            if ((item.first == std::pair<size_t, size_t>(originBoxRow, originBoxCol)) && item.second == true)
            {
                item.second = false;
                Next.targetRight--;
            }
            // If the new position of the box is target, mark it as R.
            if ((item.first == std::pair<size_t, size_t>(newboxRow, newboxCol)) && item.second == false)
            {
                item.second = true;
                Next.targetRight++;
            }
        }

        (*it).first = newboxRow;
        (*it).second = newboxCol;

        Next.pushBox = true;
    }

    Next.Player.first = newRow;
    Next.Player.second = newCol;

    // int newDirect = char2Int(Direction);
    // Next.route.push_back(newDirect);

    Next.route += Direction;
    // std::cout << Next.route << std::endl;

    return Next;
}

class MyHash
{
public:
    // size_t operator()(const std::vector<int> &state) const
    size_t operator()(const State &state) const
    {
        size_t hash = 0;
        hash = hash * 31 + std::hash<size_t>()(std::get<0>(state));
        hash = hash * 31 + std::hash<size_t>()(std::get<1>(state));
        for (const auto &it : std::get<2>(state))
        {
            hash = hash * 31 + std::hash<size_t>()(it.first);
            hash = hash * 31 + std::hash<size_t>()(it.second);
        }
        return hash;
    }
};

space findSameState(const space &current, std::queue<space> &qEnd)
{
    size_t qSize = qEnd.size();

    for (size_t i = 0; i < qSize; ++i)
    {
        space Now = qEnd.front();
        qEnd.pop();
        qEnd.push(Now);

        if (current.Player != Now.Player)
        {
            continue;
        }

        auto box1 = current.boxes;
        auto box2 = Now.boxes;

        std::sort(box1.begin(), box1.end());
        std::sort(box2.begin(), box2.end());

        if (box1 != box2)
        {
            continue;
        }

        return Now;
    }

    space temp;
    return temp;
}

/**n
 * @brief  Solve the sokoban described by the grid
 * @note   Output string format: See project description
 * @param  &grid: The grid you read by `read_map'
 * @retval
 */
std::string solve(std::vector<std::string> &grid)
{
    // Get the position of the starting point, boxes and targets.
    space initial = findItems(grid);
    initial.targetState = findTarget(grid);
    initial.pushBox = false;
    // std::cout << Items.boxes.size() << std::endl;
    // std::cout << targets.size() << std::endl;

    initial.targetRight = 0;
    for (auto &it : initial.targetState)
    {
        if (it.second == true)
        {
            initial.targetRight++;
        }
    }

    // If there are more boxes than target positions.
    if ((initial.boxes).size() > (initial.targetState).size())
    {
        std::cout << "No solution!" << std::endl;
        std::exit(0);
    }

    size_t differ = 0;
    differ = initial.targetState.size() - initial.boxes.size();

    // Detect whether the game is completed initially.
    if (isSolved(initial))
    {
        return "";
    }

    if (isDeadLock(grid, initial))
    {
        std::cout << "No solution!" << std::endl;
        std::exit(0);
    }

    // std::set<std::vector<int>> visited;
    std::unordered_set<State, MyHash> visitedStart;
    std::unordered_set<State, MyHash> visitedEnd;
    std::queue<space> qStart;
    std::queue<space> qEnd;

    qStart.push(initial);
    visitedStart.insert(addStateVisited(initial)); // Mark the initial state as visited.

    if (differ == 0)
    {
        for (const auto &target : initial.targetState)
        {
            // Record the position (x,y) of every target.
            size_t x = target.first.first;
            size_t y = target.first.second;

            std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // UDLR.
            for (const auto &dir : directions)
            {
                size_t EndPointx = (size_t)(static_cast<int>(x) + dir.first);
                size_t EndPointy = (size_t)(static_cast<int>(y) + dir.second);

                if (grid[EndPointx][EndPointy] == '.' || grid[EndPointx][EndPointy] == 'B')
                {
                    space possibleGoal;
                    possibleGoal.Player.first = EndPointx;
                    possibleGoal.Player.second = EndPointy;
                    possibleGoal.boxes.clear();
                    possibleGoal.route = "";
                    possibleGoal.pushBox = false;
                    possibleGoal.targetState = initial.targetState;

                    for (auto &it : possibleGoal.targetState)
                    {
                        possibleGoal.boxes.push_back(it.first);
                        it.second = true;
                    }

                    possibleGoal.targetRight = possibleGoal.targetState.size();
                    if (visitedEnd.find(addStateVisited(possibleGoal)) == visitedEnd.end())
                    {
                        qEnd.push(possibleGoal);
                        visitedEnd.insert(addStateVisited(possibleGoal));
                    }
                }
            }
        }
    }
    else
    {
        for (const auto &target : initial.targetState)
        {
            for (const auto &item : initial.targetState)
            {
                if (item == target)
                {
                    continue;
                }
                // Record the position (x,y) of every target.
                size_t x = target.first.first;
                size_t y = target.first.second;

                std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // UDLR.
                for (const auto &dir : directions)
                {
                    size_t EndPointx = (size_t)(static_cast<int>(x) + dir.first);
                    size_t EndPointy = (size_t)(static_cast<int>(y) + dir.second);

                    if (grid[EndPointx][EndPointy] == '.' || grid[EndPointx][EndPointy] == 'B')
                    {
                        space possibleGoal;
                        possibleGoal.Player.first = EndPointx;
                        possibleGoal.Player.second = EndPointy;
                        possibleGoal.boxes.clear();
                        possibleGoal.route = "";
                        possibleGoal.pushBox = false;
                        possibleGoal.targetState = initial.targetState;

                        for (auto &it : possibleGoal.targetState)
                        {
                            if (it == item)
                            {
                                continue;
                            }
                            possibleGoal.boxes.push_back(it.first);
                            it.second = true;
                        }

                        possibleGoal.targetRight = possibleGoal.targetState.size();
                        if (visitedEnd.find(addStateVisited(possibleGoal)) == visitedEnd.end())
                        {
                            qEnd.push(possibleGoal);
                            visitedEnd.insert(addStateVisited(possibleGoal));
                        }
                    }
                }
            }
        }
    }

    while ((!qStart.empty()) && (!qEnd.empty()))
    {
        if ((qStart.size() < qEnd.size()))
        {
            size_t currentSize = qStart.size();
            for (size_t i = 0; i < currentSize; ++i)
            {
                space current = qStart.front();
                qStart.pop();

                for (char direction : "UDLR")
                {
                    if (isValidDirection(grid, current, direction))
                    {
                        space next = moveNextState(current, direction);
                        // Not exist yet.
                        if ((!(isDeadLock(grid, next))) && (visitedStart.find(addStateVisited(next))) == visitedStart.end())
                        {
                            qStart.push(next);
                            visitedStart.insert(addStateVisited(next));
                            // std::cout << next.route << std::endl;

                            // Whether solved.
                            auto it = visitedEnd.find(addStateVisited(next));
                            if (it != visitedEnd.end())
                            {
                                space correctBack = findSameState(next, qEnd);
                                // std::cout << next.route << std::endl;
                                // std::cout << reverseRoute(correctBack.route) << std::endl;
                                return next.route + reverseRoute(correctBack.route);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            size_t currentSize = qEnd.size();
            for (size_t i = 0; i < currentSize; ++i)
            {
                space current = qEnd.front();
                qEnd.pop();

                for (char direction : "RLDU")
                {
                    if (isValidDirectionBack(grid, current, direction))
                    {
                        space next = moveNextStateBack(current, direction, 1);
                        // Not exist yet.
                        if ((!(isDeadLock(grid, next))) && (visitedEnd.find(addStateVisited(next))) == visitedEnd.end())
                        {
                            qEnd.push(next);
                            visitedEnd.insert(addStateVisited(next));
                            // std::cout << next.route << std::endl;

                            // if (!SomeRight)
                            // {
                            //     for (auto const &item : next.boxes)
                            //     {
                            //         if (grid[item.first][item.second] == 'B')
                            //         {
                            //             SomeRight = true;
                            //         }
                            //     }
                            // }

                            // Whether solved.
                            auto it = visitedStart.find(addStateVisited(next));
                            if (it != visitedStart.end())
                            {
                                space correctForw = findSameState(next, qStart);
                                // std::cout << reverseRoute(next.route) << std::endl;
                                // std::cout << correctForw.route << std::endl;
                                return correctForw.route + reverseRoute(next.route);
                            }
                        }

                        // if (SomeRight)
                        // {
                        space next2 = moveNextStateBack(current, direction, 0);

                        if ((!(isDeadLock(grid, next2))) && (visitedEnd.find(addStateVisited(next2))) == visitedEnd.end())
                        {
                            qEnd.push(next2);
                            visitedEnd.insert(addStateVisited(next2));
                            // std::cout << next2.route << std::endl;

                            // Whether solved.
                            auto it = visitedStart.find(addStateVisited(next2));
                            if (it != visitedStart.end())
                            {
                                space correctForw = findSameState(next2, qStart);
                                // std::cout << reverseRoute(next2.route) << std::endl;
                                // std::cout << correctForw.route << std::endl;
                                return correctForw.route + reverseRoute(next2.route);
                            }
                        }
                        // }
                    }
                }
            }
        }
    }

    // std::cout << "No solution!" << std::endl;
    // std::exit(0);

    return "No solution!";
}

void movePlayer(std::vector<std::string> &grid, char move, bool &isPlayerOnTarget)
{
    int dx = 0;
    int dy = 0;

    // Help to find the new position of the player.
    if (move == 'U')
    {
        dx = -1;
    }
    else if (move == 'D')
    {
        dx = 1;
    }
    else if (move == 'L')
    {
        dy = -1;
    }
    else if (move == 'R')
    {
        dy = 1;
    }

    for (size_t i = 0; i < grid.size(); ++i)
    {
        for (size_t j = 0; j < grid[i].size(); ++j)
        {
            if (grid[i][j] == 'S') // Found the original position of the player.
            {
                int ni = (int)i + dx;
                int nj = (int)j + dy;

                if (grid[ni][nj] == '.') // If new position is path.
                {
                    // If the old position is a walkable path.
                    if (!isPlayerOnTarget)
                    {
                        grid[i][j] = '.';
                    }
                    else
                    { // If once was a target position.
                        grid[i][j] = 'T';
                        isPlayerOnTarget = false;
                    }
                }
                else if (grid[ni][nj] == 'T') // New position is target, so now player and target overlap.
                {
                    if (!isPlayerOnTarget)
                    {
                        grid[i][j] = '.';
                        isPlayerOnTarget = true;
                    }
                    else
                    {
                        grid[i][j] = 'T';
                        isPlayerOnTarget = true;
                    }
                }
                else if (grid[ni][nj] == 'B') // New position is box, so push the box.
                {
                    int nbi = ni + dx;
                    int nbj = nj + dy;

                    if (nbi < 0 || nbi >= (int)grid.size() || nbj < 0 || nbj >= (int)grid[i].size())
                    {
                        return;
                    }

                    // New position of box.
                    if (grid[nbi][nbj] == '.')
                    {
                        grid[nbi][nbj] = 'B';
                    }
                    else if (grid[nbi][nbj] == 'T')
                    {
                        grid[nbi][nbj] = 'R';
                    }

                    if (!isPlayerOnTarget)
                    {
                        grid[i][j] = '.';
                    }
                    else
                    {
                        grid[i][j] = 'T';
                        isPlayerOnTarget = false;
                    }
                }
                else if (grid[ni][nj] == 'R') // New position is box, so push the box.
                {
                    int nbi = ni + dx;
                    int nbj = nj + dy;

                    if (grid[nbi][nbj] == '.')
                    {
                        grid[nbi][nbj] = 'B';
                    }
                    else if (grid[nbi][nbj] == 'T')
                    {
                        grid[nbi][nbj] = 'R';
                    }

                    if (!isPlayerOnTarget)
                    {
                        grid[i][j] = '.';
                        isPlayerOnTarget = true;
                    }
                    else
                    {
                        grid[i][j] = 'T';
                        isPlayerOnTarget = true;
                    }
                }
                grid[ni][nj] = 'S';
                return;
            }
        }
    }
    return;
}

void writeMap(std::vector<std::string> &grid, const std::string &route, std::ostream &os)
{
    size_t row = grid.size();
    size_t col = grid[0].size();
    bool isPlayerOnTarget = false;

    // std::cout << col << std::endl;

    // Output the original grid.
    os << col << " " << row << std::endl;
    for (size_t i = 0; i < row; ++i)
    {
        os << grid[i] << std::endl;
    }
    os << std::endl;

    os << route << std::endl;
    os << std::endl;

    size_t step = 0;
    while (step < route.size())
    {
        movePlayer(grid, route[step], isPlayerOnTarget);
        for (size_t i = 0; i < row; ++i)
        {
            os << grid[i] << std::endl;
        }
        os << std::endl;

        step++;
    }

    return;
}

// For big cases, paste your answers here (i.e. replace "ans for big 1" with your answer)
// Do not remove the first element, it should be left blank.
// NOTE: Ensure the order is correct!
// Your answer should look like "UUDDLLRR..."
const std::vector<std::string> answers = {
    "__leave_this_blank__",
    "UUUUUULLLDRDLLLLLLLLUULLLLRDRRRDLLDLUDLLULDDDDLDRRRRRDLDDRRRDDDRRRDRDRRULLLRDDDLLUUURULLDLUUULURRR",
    "RUULDDLDDLLUULUUURRDLDDRRDDRRUULUULLULLDDRURRRULRDDDRDLDLLLUURRDRUUDDDLL",
    "DRURRLLUUULUURDRRRDDDRRDDLLUURRUURRUULLDDDDLLDDRRURULDDLLULLLUUULUURDRRRRLDDRRDDDLLULLLUUULURRRRDDRRDDDLLDLURRRUUULLDDUUUULLLDDDDRRDRUUURRDDDLRUUUURRUULLDLLLLRRRRDDLLUDDDLDDRUUUURRDLULDDLDDRURRURULULLDDLDRUUURRUULLDDDDLLLUUULUURDRRURDDDDULDDLLUUULURRRURDRRDDDLLULDRRRUUULLDDRDL",
    "ans for big 4",
    "RRUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULUURRDLLDDDLLLLLDDDDDLLLUURRDRUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULULLLLRRRRURRDLLLLLRRRDDDLLLLLDDDRDRRDRRULLLLDLUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULUURRDLLLLULDRRRDDDLLLLLDDDRDRRDLLRRDDLLLUUUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULUURRDLLLLDLURUL",
    "URRUULULLRRDRDDLUUDDLDLLURUURUURRDLDDRDDLLRURUULUURDDLLLDDRDRRULUURDULLLDDRDRUURULDDDRULLLLDRRURUURUULLLDDDUUURRRDDLUDDDLDLLURUURURRDLDDRDLLLURDRUUURULDDDRUU",
    "LUURRDRUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULUURRDLLLLLLRRRRDDDLLLLLDDDDDRRULDLUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULUURRDLLLLLRRRDDDLLLLLDDDRDRRDRRULLLLDLUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULUURRDLLLLDLURRRDDDLLLLLDDDDDDDRRRUURULLLDLUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULUURRDLLLLULLRDRRRDDDLLLLLDDDLLLDDRRURDRRRDDLLLUUUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULUURRDLLLLDLURUL",
    "ULDDDRDLLLDLLURRRLLUURDUURURRDLDDLLUURURDLLDDRDLDLLULUURRLURRDLLLDDRDRRULLRRRURRDLLLDLLURURRDRRULDLLDLURUUURURRDLDDRDLLLUUURURDDDUULLDDDRRLLDLLURRUUULLDRDUURRRDDLLDDLLURURRRUULLLLDLDLDRUURURRDLDRLDDRUULUURRRDDLRDL",
    "RDDLLLDDLLURDRRRDRUUULLLDLDRRLUURRDRDDLULLULLUUUUURRDDDDDLDRRRURDUUUULUURDDDDLDDLLULLUUUURURDDDDDLDRRLUURRDDLLULLUURLUURRDDDDLDRRR",
    "LLLULUURDLDDDRDRRRRUUUULLLLULDRDDDURRRU",
    "ans for big 11",
    "ans for big 12",
    "LLLUUURRRUUUUURDRRDDDDLDLULUUURRURDDUUURDDDDDUURRDL",
    "RRUUURRLLUULURRRRRRRRRRLLLLLDDDDLUUUDLUURRRRRLLLLLLURRRRRLLLLURRRRDRUDDRUU",
    "ans for big 15"};

// Don't modify this.
std::string print_answer(int index)
{
    if (index < 1 || index >= (int)answers.size())
    {
        return "invalid";
    }
    return answers[(size_t)index];
}
