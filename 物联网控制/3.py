import random
import copy
import networkx as nx
import matplotlib.pyplot as plt

plt.rcParams['font.sans-serif'] = ['Arial Unicode MS', 'PingFang SC', 'SimHei', 'Microsoft YaHei']
plt.rcParams['axes.unicode_minus'] = False

# 1. 定义复杂的路网图 (20个节点，包含主干道和复杂分支)
GRAPH = {
    0: {1: 4, 5: 3, 6: 7},
    1: {0: 4, 2: 5, 6: 2},
    2: {1: 5, 3: 2, 7: 4, 8: 6},
    3: {2: 2, 4: 6, 8: 3},
    4: {3: 6, 9: 5},
    5: {0: 3, 6: 4, 10: 6},
    6: {0: 7, 1: 2, 5: 4, 7: 3, 11: 5, 12: 4},
    7: {2: 4, 6: 3, 8: 4, 12: 2},
    8: {2: 6, 3: 3, 7: 4, 9: 2, 13: 5},
    9: {4: 5, 8: 2, 14: 4},
    10: {5: 6, 11: 3, 15: 5},
    11: {6: 5, 10: 3, 12: 4, 16: 3, 17: 4},
    12: {6: 4, 7: 2, 11: 4, 13: 3, 17: 6, 18: 5},
    13: {8: 5, 12: 3, 14: 2, 18: 4},
    14: {9: 4, 13: 2, 19: 7},
    15: {10: 5, 16: 4},
    16: {11: 3, 15: 4, 17: 2},
    17: {11: 4, 12: 6, 16: 2, 18: 3},
    18: {12: 5, 13: 4, 17: 3, 19: 5},
    19: {14: 7, 18: 5}
}

class PathPlanningGA:
    def __init__(self, start, target, pop_size=50, max_gen=200, pc=0.8, pm=0.2):
        self.start = start
        self.target = target
        self.pop_size = pop_size
        self.max_gen = max_gen
        self.pc = pc  # 交叉概率
        self.pm = pm  # 变异概率
        self.population = []

    def random_path(self, current, target, path):
        """深度优先随机搜索生成一条初始无环路径"""
        if current == target:
            return path
        neighbors = list(GRAPH[current].keys())
        random.shuffle(neighbors)
        for n in neighbors:
            if n not in path: 
                res = self.random_path(n, target, path + [n])
                if res: return res
        return None

    def init_population(self):
        """3. 初始路径的产生"""
        attempts = 0
        while len(self.population) < self.pop_size:
            path = self.random_path(self.start, self.target, [self.start])
            if path:
                if path not in self.population:
                    self.population.append(path)
                else:
                    if attempts > 500: # 复杂图放宽尝试次数
                        self.population.append(path)
            attempts += 1

    def calculate_distance(self, path):
        """计算路径总距离"""
        dist = 0
        for i in range(len(path) - 1):
            dist += GRAPH[path[i]][path[i+1]]
        return dist

    def fitness(self, path):
        """4. 适应度函数的确定"""
        return 1.0 / self.calculate_distance(path)

    def select(self, pop):
        """5. 复制（选择）操作：轮盘赌选择"""
        fitnesses = [self.fitness(p) for p in pop]
        total_fitness = sum(fitnesses)
        pick = random.uniform(0, total_fitness)
        current = 0
        for i, f in enumerate(fitnesses):
            current += f
            if current >= pick:
                return pop[i]
        return pop[-1]

    def crossover(self, p1, p2):
        """6. 交叉操作"""
        if random.random() > self.pc:
            return p1, p2
        common_nodes = list(set(p1[1:-1]) & set(p2[1:-1]))
        if not common_nodes:
            return p1, p2 
        
        cross_node = random.choice(common_nodes)
        idx1 = p1.index(cross_node)
        idx2 = p2.index(cross_node)
        
        child1 = p1[:idx1] + p2[idx2:]
        child2 = p2[:idx2] + p1[idx1:]
        return child1, child2

    def mutate(self, path):
        """7. 变异操作"""
        if random.random() > self.pm or len(path) <= 2:
            return path
        
        mutate_idx = random.randint(0, len(path) - 2)
        mutate_node = path[mutate_idx]
        
        new_sub_path = self.random_path(mutate_node, self.target, [mutate_node])
        if new_sub_path:
            mutated_path = path[:mutate_idx] + new_sub_path
            return mutated_path
        return path

    def remove_loops(self, path):
        """8. 删除操作：去除环路"""
        new_path = []
        for node in path:
            if node in new_path:
                idx = new_path.index(node)
                new_path = new_path[:idx]
            new_path.append(node)
        return new_path

    def run(self):
        """运行遗传算法"""
        self.init_population()
        best_path = None
        best_dist = float('inf')

        for gen in range(self.max_gen):
            new_population = []
            
            # 记录当前代最优 (保留精英)
            for p in self.population:
                dist = self.calculate_distance(p)
                if dist < best_dist:
                    best_dist = dist
                    best_path = copy.deepcopy(p)

            # 生成新一代
            while len(new_population) < self.pop_size:
                p1 = self.select(self.population)
                p2 = self.select(self.population)
                
                c1, c2 = self.crossover(p1, p2)
                
                c1 = self.mutate(c1)
                c2 = self.mutate(c2)
                
                c1 = self.remove_loops(c1)
                c2 = self.remove_loops(c2)
                
                new_population.extend([c1, c2])
                
            # 保持种群规模并保留最佳路径
            self.population = new_population[:self.pop_size-1] + [best_path]

        return best_path, best_dist

def visualize_path(graph_dict, best_path):
    """可视化路网和规划出的最优路径"""
    G = nx.Graph()
    
    # 构建 NetworkX 图
    for u, edges in graph_dict.items():
        for v, weight in edges.items():
            G.add_edge(u, v, weight=weight)
            
    # 设置节点布局 (为了每次运行图形不乱变，设置一个固定的随机种子)
    pos = nx.spring_layout(G, seed=42) 
    
    plt.figure(figsize=(8, 6))
    plt.title("遗传算法路径规划", fontsize=14)
    
    # 1. 绘制所有节点和基础边
    nx.draw_networkx_nodes(G, pos, node_color='lightblue', node_size=600, edgecolors='gray')
    nx.draw_networkx_edges(G, pos, edge_color='gray', width=1.0, alpha=0.5)
    nx.draw_networkx_labels(G, pos, font_size=12, font_weight='bold')
    
    # 2. 绘制边上的权重(距离)
    edge_labels = nx.get_edge_attributes(G, 'weight')
    nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, font_size=10)
    
    # 3. 高亮显示最优路径
    if best_path:
        # 提取路径中的边: [(0, 1), (1, 3), ...]
        path_edges = list(zip(best_path, best_path[1:]))
        
        # 绘制路径节点（绿色）
        nx.draw_networkx_nodes(G, pos, nodelist=best_path, node_color='lightgreen', node_size=600)
        # 绘制起点（黄色）和终点（红色）
        nx.draw_networkx_nodes(G, pos, nodelist=[best_path[0]], node_color='gold', node_size=700, edgecolors='black')
        nx.draw_networkx_nodes(G, pos, nodelist=[best_path[-1]], node_color='tomato', node_size=700, edgecolors='black')
        
        # 绘制路径边（红色加粗）
        nx.draw_networkx_edges(G, pos, edgelist=path_edges, edge_color='red', width=3.0)

    # 关闭坐标轴显示并展现窗口
    plt.axis('off')
    plt.tight_layout()
    plt.show()

# 仿真执行
if __name__ == "__main__":
    start_node = 0
    target_node = 19
    print(f"开始路径规划模拟: 起点 {start_node} -> 终点 {target_node}")
    
    # 运行遗传算法
    ga = PathPlanningGA(start=start_node, target=target_node, pop_size=30, max_gen=100)
    best_path, min_dist = ga.run()
    
    print("\n规划完成!")
    print(f"最优路径: {' -> '.join(map(str, best_path))}")
    print(f"最短距离: {min_dist}")
    
    # 弹出可视化窗口
    print("正在生成可视化图表...")
    visualize_path(GRAPH, best_path)