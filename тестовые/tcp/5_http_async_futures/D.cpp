#include <iostream>
#include <vector>

int INF = 30000;

struct Graph {
    std::vector <std::vector <int>> edges;
    int N;
    void add_edge(int a, int b, int cost) {
        if (cost < edges[a - 1][b - 1]) {
            edges[a - 1][b - 1] = cost;
        }
    }
    void floyd() {
        for (int k = 0; k < N; k++) {
            for (int y = 0; y < N; y++) {
                for (int x = 0; x < N; x++) {
                    if (edges[y][k] < INF && edges[k][x] < INF) {
                        if (edges[y][k] + edges[k][x] < edges[y][x]) {
                            edges[y][x] = edges[y][k] + edges[k][x];
                        }
                    }
                }
            }
        }
    }
    std::vector <std::vector <int>> get_matrix() {
        floyd();
        return edges;
    }
    Graph(int n) {
        N = n;
        for (int y = 0; y < N; y++) {
            std::vector <int> line;
            for (int x = 0; x < N; x++) {
                if (x == y) {
                    line.push_back(0);
                } else {
                    line.push_back(INF);
                }
            }
            edges.push_back(line);
        }
    }
};

int main() {
    int N, M;
    std::cin >> N >> M;
    Graph graph(N);
    for (int i = 0; i < M; i++) {
        int a, b, cost;
        std::cin >> a >> b >> cost;
        graph.add_edge(a, b, cost);
    }
    for (auto line : graph.get_matrix()) {
        for (int item : line) {
            std::cout << item << ' ';
        }
        std::cout << std::endl;
    }
    return 0;
}
