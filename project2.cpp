#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <set>
#include <map>
#include <string>

class FloorCleaner {
 public:
  using Cell = std::pair<int, int>;

  FloorCleaner(int, int, int);

  void SetCell(int, int, char);
  void Clean();

  std::size_t TotalSteps() const { return path_.size(); }
  std::vector<Cell> CleaningPath() const { return path_; }

 private:
  std::vector<Cell> ShortestPath(const Cell&, const Cell&);

  const int rows_, cols_;
  const int battery_life_;
  Cell charging_cell_;
  std::vector<Cell> path_;
  std::map<Cell, bool> cleaned_;
  std::vector<std::vector<char> > floor_;
  std::vector<std::vector<Cell> > shortest_paths_;
};

FloorCleaner::FloorCleaner(int rows, int cols, int battery_life)
    : rows_(rows),
      cols_(cols),
      battery_life_(battery_life),
      charging_cell_(-1, -1),
      floor_(rows, std::vector<char>(cols)) {
}

void FloorCleaner::SetCell(int row, int col, char cell_type)
{
    floor_[row][col] = cell_type;
    if(cell_type == 'R')
    {
        charging_cell_.first = row;
        charging_cell_.second = col;
    }
    if(cell_type == '0')
    {
        cleaned_[{row, col}] = false;
    }
}

void FloorCleaner::Clean()
{
    shortest_paths_.emplace_back(ShortestPath(charging_cell_, cleaned_.begin()->first));
    Cell source2 = shortest_paths_[0][1];
    shortest_paths_[0].erase(shortest_paths_[0].begin());
    for(const auto &cell : cleaned_)
    {
        if(cell == *cleaned_.begin()) continue;
        shortest_paths_.emplace_back(ShortestPath(source2, cell.first));
    }
    std::sort(shortest_paths_.begin(), shortest_paths_.end(),
        [](const std::vector<Cell> &a, const std::vector<Cell> &b) {
            return a.size() > b.size();
        });
    auto not_visited = [&]() {
        return std::find_if(shortest_paths_.begin(), shortest_paths_.end(),
            [&](const std::vector<Cell> &path) {
                return !cleaned_[path.back()];
            });
    };
    auto shortest_path = not_visited();
    while(shortest_path != shortest_paths_.end())
    {
        if(shortest_path->size() * 2 > battery_life_)
        {
            std::cerr << "run out of battery" << std::endl;
            throw std::exception();
        }
        path_.insert(path_.end(), shortest_path->begin(), shortest_path->end());
        path_.insert(path_.end(), shortest_path->rbegin() + 1, shortest_path->rend());
        for (auto &cell : *shortest_path)
        {
            cleaned_[cell] = true;
        }
        path_.emplace_back(charging_cell_);
        shortest_path = not_visited();
    }
}

std::vector<FloorCleaner::Cell> FloorCleaner::ShortestPath(const Cell &source, const Cell &destination)
{
    std::vector<Cell> path;
    std::queue<std::vector<Cell> > queue({{source}});
    std::set<Cell> visited({source});
    int dir_r[] = {1, -1, 0, 0};
    int dir_c[] = {0, 0, 1, -1};
    while (!queue.empty())
    {
        path = queue.front();
        queue.pop();
        Cell cell = path.back();
        if(cell == destination) return path;
        for(int i = 0; i < 4; ++i)
        {
            Cell next_cell(cell.first + dir_r[i], cell.second + dir_c[i]);
            if ((next_cell.first >= 0 && next_cell.first < rows_) &&
            (next_cell.second >= 0 && next_cell.second < cols_) &&
            floor_[next_cell.first][next_cell.second] == '0' &&
            visited.find(next_cell) == visited.end())
            {
                path.emplace_back(next_cell);
                queue.emplace(path); //queue may have many paths in the same time
                path.pop_back();
                visited.emplace(next_cell);
            }
         }
    }
    return std::vector<Cell>();  // path not found
}

int main(int argc, const char *argv[]) {
    std::string file = "./";
    std::string studentID = argv[1];
    std::string fileinput = file + studentID + "/floor.data";
    std::string fileoutput = file + studentID + "/final.path";
    std::ifstream input;
    std::ofstream output;
    input.open(fileinput.c_str());
    if(input)
    {
        int rows, cols, battery_life;
        input >> rows >> cols >> battery_life;
        FloorCleaner *robot = new FloorCleaner(rows, cols, battery_life);
        for(int i = 0; i < rows; ++i)
        {
            for(int j = 0; j < cols; ++j)
            {
                char cell_type;
                input >> cell_type;
                robot->SetCell(i, j, cell_type);
            }
        }
        robot->Clean();
        output.open(fileoutput.c_str());
        std::vector<FloorCleaner::Cell> path = robot->CleaningPath();
        output << robot->TotalSteps() << std::endl;
        for(const FloorCleaner::Cell &cell : path)
            output << cell.first << " " << cell.second << std::endl;
        output.close();
        delete robot;
        input.close();
    }
    else std::cerr << "failed to open \"floor.data\"" << std::endl;
    return 0;
}
