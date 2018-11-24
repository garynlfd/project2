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
        //path_.emplace_back(row, col);
    }
    if(cell_type == '0')
    {
        cleaned_[{row, col}] = false;
    }
}

void FloorCleaner::Clean()
{

}

std::vector<FloorCleaner::Cell> FloorCleaner::ShortestPath(const Cell &source, const Cell &destination)
{

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
        output << robot->TotalSteps() << std::endl;
        for(const FloorCleaner::Cell &cell : path)
            output << cell.first << " " << cell.second << std::endl;
        output.close();
        //else std::cerr << "failed to write \"final.path\"" << std::endl;
        delete robot;
        input.close();
    }
    else std::cerr << "failed to open \"floor.data\"" << std::endl;
    return 0;
}
