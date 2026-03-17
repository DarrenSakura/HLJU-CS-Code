#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>

using namespace std;

// 图类
class Graph {
public:
    int V;  // 顶点数
    vector<vector<int>> adj;  // 邻接表

    // 构造函数
    Graph(int V) {
        this->V = V;
        adj.resize(V);
    }

    // 添加边
    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);  // 无向图
    }

    // 广度优先搜索求最短路径
    vector<int> bfs(int start, int end) {
        vector<int> dist(V, INT_MAX);  // 距离数组，初始化为无穷大
        vector<int> parent(V, -1);  // 用于存储路径
        queue<int> q;

        dist[start] = 0;
        q.push(start);

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            // 遍历u的邻接节点
            for (int v : adj[u]) {
                if (dist[v] == INT_MAX) {  // 如果v没有被访问过
                    dist[v] = dist[u] + 1;  // 更新距离
                    parent[v] = u;  // 记录父节点
                    q.push(v);
                    // 找到目标节点，提前终止
                    if (v == end) {
                        return parent;  // 返回路径的父节点数组
                    }
                }
            }
        }

        return parent;  // 如果没有路径返回父节点数组
    }

    // 输出从start到end的路径
    void printPath(int start, int end, vector<int>& parent) {
        if (parent[end] == -1) {
            cout << "从 " << start << " 到 " << end << " 没有路径！" << endl;
            return;
        }

        vector<int> path;
        for (int v = end; v != -1; v = parent[v]) {
            path.push_back(v);
        }

        // 反转路径，使其从start到end
        reverse(path.begin(), path.end());  // 调用 reverse 函数

        cout << "从 " << start << " 到 " << end << " 的最短路径是：";
        for (int v : path) {
            cout << v << " ";
        }
        cout << endl;
    }
};

int main() {
    int V, E;  // 顶点数和边数
    cout << "请输入图的顶点数：";
    cin >> V;
    cout << "请输入图的边数：";
    cin >> E;

    Graph g(V);

    cout << "请输入每条边的信息（每条边由两个整数u和v表示，表示u和v之间有一条边）：" << endl;
    for (int i = 0; i < E; ++i) {
        int u, v;
        cin >> u >> v;
        g.addEdge(u, v);
    }

    int start, end;
    cout << "请输入起始顶点：";
    cin >> start;
    cout << "请输入目标顶点：";
    cin >> end;

    // 调用BFS来求解最短路径
    vector<int> parent = g.bfs(start, end);

    // 输出最短路径
    g.printPath(start, end, parent);

    return 0;
}
