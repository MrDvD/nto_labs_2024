#include <iostream>
#include <vector>
#include <algorithm>

long INF = 1e9;

struct Tuple {
    int x, y;
    Tuple(int X, int Y) {
        x = X; y = Y;
    }
};

struct Cell {
    std::vector <Tuple> adjacent;
    bool visited = false;
    long min = INF;
    Tuple prev = Tuple(-1, -1);
    Cell(int N, int x, int y) {
        if (x + 1 < N && y + 2 < N) {
            adjacent.push_back(Tuple(x + 1, y + 2));
        }
        if (x + 1 < N && y - 2 >= 0) {
            adjacent.push_back(Tuple(x + 1, y - 2));
        }
        if (x + 2 < N && y + 1 < N) {
            adjacent.push_back(Tuple(x + 2, y + 1));
        }
        if (x + 2 < N && y - 1 >= 0) {
            adjacent.push_back(Tuple(x + 2, y - 1));
        }
        if (x - 1 >= 0 && y + 2 < N) {
            adjacent.push_back(Tuple(x - 1, y + 2));
        }
        if (x - 1 >= 0 && y - 2 >= 0) {
            adjacent.push_back(Tuple(x - 1, y - 2));
        }
        if (x - 2 >= 0 && y + 1 < N) {
            adjacent.push_back(Tuple(x - 2, y + 1));
        }
        if (x - 2 >= 0 && y - 1 >= 0) {
            adjacent.push_back(Tuple(x - 2, y - 1));
        }
    }
};

struct Graph {
    int N;
    std::vector <std::vector <Cell>> board;
    void dijkstra(int xs, int ys) {
        // base
        board[ys][xs].min = 0;
        while (true) {
            // minimization
            int xv = -1, yv = -1;
            for (int y = 0; y < N; y++) {
                for (int x = 0; x < N; x++) {
                    if (!board[y][x].visited && (xv == -1 || board[y][x].min < board[yv][xv].min)) {
                        xv = x; yv = y;
                    }
                }
            }
            if (xv == -1) {
                break;
            }
            // relaxation
            board[yv][xv].visited = true;
            for (Tuple n : board[yv][xv].adjacent) {
                long curr = board[yv][xv].min + 1;
                if (curr < board[n.y][n.x].min) {
                    board[n.y][n.x].min = curr;
                    board[n.y][n.x].prev = Tuple(xv, yv);
                }
            }
        }
    }
    std::vector <Tuple> get_path(int x1, int y1, int x2, int y2) {
        dijkstra(x1, y1);
        std::vector <Tuple> path = {Tuple(x2, y2)};
        while (board[y2][x2].prev.x != -1) {
            path.push_back(board[y2][x2].prev);
            int lx = x2, ly = y2;
            x2 = board[ly][lx].prev.x;
            y2 = board[ly][lx].prev.y;
        }
        return path;
    }
    Graph(int n) {
        N = n;
        for (int y = 0; y < N; y++) {
            std::vector <Cell> line;
            for (int x = 0; x < N; x++) {
                line.push_back(Cell(N, x, y));
            }
            board.push_back(line);
        }
    }
};

int main() {
    int N, x1, y1, x2, y2;
    std::cin >> N >> x1 >> y1 >> x2 >> y2;
    Graph graph(N);
    std::vector <Tuple> path = graph.get_path(x1 - 1, y1 - 1, x2 - 1, y2 - 1);
    std::cout << path.size() - 1 << std::endl;
    std::reverse(path.begin(), path.end());
    for (Tuple t : path) {
        std::cout << t.x + 1 << ' ' << t.y + 1 << std::endl;
    }
    return 0;
}
