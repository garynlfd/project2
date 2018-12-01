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
  void GetDistances(const Cell&);

  const int rows_, cols_;
  const int battery_life_;
  Cell charging_cell_;
  Cell second_cell_;
  std::vector<Cell> path_;
  std::map<Cell, bool> cleaned_;
  std::vector<std::vector<char> > floor_;
  std::map<Cell, int> distances_;
};

FloorCleaner::FloorCleaner(int rows, int cols, int battery_life)
    : rows_(rows),
      cols_(cols),
      battery_life_(battery_life),
      charging_cell_(-1, -1),
      floor_(rows, std::vector<char>(cols))
{
    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < cols; ++j)
        {
            distances_[{i, j}] = -1;
        }
    }
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
    auto max_distance = [&]() {
        return std::max_element(distances_.begin(), distances_.end(),
                            [&](const std::pair<Cell, int> &a,
                                const std::pair<Cell, int> &b) {
                                    if(cleaned_[a.first]) return true;
                                    if(cleaned_[b.first]) return false;
                                    return a.second < b.second;
                                });
    };
    GetDistances(charging_cell_);
    second_cell_ = ShortestPath(charging_cell_, max_distance()->first)[1];
    cleaned_[second_cell_] = true;
    GetDistances(second_cell_);
    auto not_visited = [&]() -> Cell
    {
        Cell max = max_distance()->first;
        if(cleaned_[max] || distances_[max] == -1)
        {
            return Cell(-1, -1);
        }
        return max;
    };
    auto c = not_visited();
    while(c != Cell(-1, -1))
    {
        std::vector<Cell> path, return_path;
        path = ShortestPath(second_cell_, c);
        for(auto &cell : path) { cleaned_[cell] = true; }
        path_.insert(path_.end(), path.begin(), path.end());
        path_.insert(path_.end(), path.rbegin() + 1, path.rend());
        path_.emplace_back(charging_cell_);
        c = not_visited();
    }
}


void FloorCleaner::GetDistances(const Cell &source)
{
    const int dir_r[] = {1, -1, 0, 0};
    const int dir_c[] = {0, 0, 1, -1};
    for(auto &cell : distances_) { cell.second = -1; }
    distances_[source] = 0;
    for(bool done = false; !done;)
    {
        done = true;
        for(int i = 0; i < rows_; ++i)
        {
            for(int j = 0; j < cols_; ++j)
            {
                if(distances_[{i, j}] == -1) continue;
                for (int k = 0; k < 4; ++k)
                {
                    int next_r = i + dir_r[k];
                    int next_c = j + dir_c[k];
                    if((0 <= next_r && next_r < rows_) &&
                       (0 <= next_c && next_c < cols_) &&
                        floor_[next_r][next_c] == '0')
                    {
                        if (distances_[{next_r, next_c}] == -1 ||
                        distances_[{next_r, next_c}] > distances_[{i, j}] + 1)
                        {
                            distances_[{next_r, next_c}] = distances_[{i, j}] + 1;
                            done = false;
                        }
                    }
                }
            }
        }
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
            if((next_cell.first >= 0 && next_cell.first < rows_) &&
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
