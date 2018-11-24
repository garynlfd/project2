#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <queue>
using namespace std;
const int MAX_row = 100;
const int MAX_col = 100;
const int INF = 1000000;
int steps = 0;
int ans_x[MAX_row];
int ans_y[MAX_col];
int rows, cols;
typedef pair<int, int> P;
char maze[MAX_row][MAX_col + 1];
int Rx, Ry; ///location of R
int d[MAX_row][MAX_col];///the distance of R to the point
int dx[4] = { 1,0,-1,0 }, dy[4] = { 0,1,0,-1 }; ///the displacement

void bfs()
{
	queue<P> Q;
	for (int i = 0; i < rows; i++)
    {
		for (int j = 0; j < cols; j++)
        {
			d[i][j] = INF;
        }
    }
	Q.push(P(Rx, Ry));
	d[Rx][Ry] = 0; ///start from R, distance = 0

	while (Q.size())
	{
		P p = Q.front();
		Q.pop();
		if(maze[p.first+1][p.second] != '0' &&
           maze[p.first-1][p.second] != '0' &&
           maze[p.first][p.second+1] != '0' &&
           maze[p.first][p.second-1] != '0') continue;

		for (int i = 0; i < 4; i++)
		{
			int nx = p.first + dx[i];
			int ny = p.second + dy[i];
			if (0 <= nx&&nx < rows
				&& 0 <= ny&&ny < cols
				&&maze[nx][ny] == '0')
			{
				Q.push(P(nx, ny));
				maze[nx][ny] = '2';
				steps++;
				ans_x[steps] = nx;
				ans_y[steps] = ny;
				d[nx][ny] = d[p.first][p.second] + 1;
			}
		}
	}
}

int main(int argc, char *argv[])
{
    string file = "./";
    string studentID = argv[1];
    string fileinput = file + studentID + "/floor.data";
    string fileoutput = file + studentID + "/final.path";
    ifstream input;
    ofstream output;
    input.open(fileinput.c_str());
    if(input)
    {
        input >> rows >> cols;

        while(!input.eof())
        {
            for(int i = 0; i < rows; ++i)
            {
                for(int j = 0; j < cols; ++j)
                {
                    input >> maze[i][j];
                }
            }
            int eat;
            input >> eat;
        }
        input.close();

        for (int i = 0; i < rows; i++)
        {
		    for (int j = 0; j < cols; j++)
		    {
			    if (maze[i][j] == 'R')
			    {
				    Rx = i;
				    Ry = j;
			    }
		    }
        }
	    bfs();
        output.open(fileoutput.c_str());
        output << steps << endl;
        for(int i = 1; i <= steps; ++i)
        {
            if(i == steps) output << ans_x[i] << " " << ans_y[i];
            else output << ans_x[i] << " " << ans_y[i] << endl;
        }
        output.close();
        }
    return 0;
}
