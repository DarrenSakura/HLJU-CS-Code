import numpy as np
import skfuzzy as fuzz
from skfuzzy import control as ctrl
import matplotlib.pyplot as plt

plt.rcParams['font.sans-serif'] = ['Arial Unicode MS']  # Windows用户若显示方块请改回 'SimHei'
plt.rcParams['axes.unicode_minus'] = False    

# ==========================================
# 第一阶段：基础时间控制器 (重量 + 材质 -> 基础时间)
# ==========================================
weight = ctrl.Antecedent(np.arange(0, 21, 1), '衣物重量 (kg)')
material = ctrl.Antecedent(np.arange(0, 101, 1), '衣服材质 (0轻薄-100厚重)')
base_time = ctrl.Consequent(np.arange(10, 51, 1), '基础时间 (分钟)')

# 隶属度函数
weight['轻'] = fuzz.trimf(weight.universe, [0, 0, 10])
weight['中等'] = fuzz.trimf(weight.universe, [0, 10, 20])
weight['重'] = fuzz.trimf(weight.universe, [10, 20, 20])

material['轻薄(丝绸)'] = fuzz.trimf(material.universe, [0, 0, 50])
material['常规(棉麻)'] = fuzz.trimf(material.universe, [0, 50, 100])
material['厚重(牛仔)'] = fuzz.trimf(material.universe, [50, 100, 100])

base_time['短'] = fuzz.trimf(base_time.universe, [10, 10, 25])
base_time['中等'] = fuzz.trimf(base_time.universe, [10, 30, 50])
base_time['长'] = fuzz.trimf(base_time.universe, [35, 50, 50])

# 规则 (重量与材质决定基础时间)
rule_b1 = ctrl.Rule(weight['轻'] & material['轻薄(丝绸)'], base_time['短'])
rule_b2 = ctrl.Rule(weight['轻'] & material['常规(棉麻)'], base_time['短'])
rule_b3 = ctrl.Rule(weight['轻'] & material['厚重(牛仔)'], base_time['中等'])
rule_b4 = ctrl.Rule(weight['中等'] & material['轻薄(丝绸)'], base_time['短'])
rule_b5 = ctrl.Rule(weight['中等'] & material['常规(棉麻)'], base_time['中等'])
rule_b6 = ctrl.Rule(weight['中等'] & material['厚重(牛仔)'], base_time['长'])
rule_b7 = ctrl.Rule(weight['重'] & material['轻薄(丝绸)'], base_time['中等'])
rule_b8 = ctrl.Rule(weight['重'] & material['常规(棉麻)'], base_time['长'])
rule_b9 = ctrl.Rule(weight['重'] & material['厚重(牛仔)'], base_time['长'])

base_ctrl = ctrl.ControlSystem([rule_b1, rule_b2, rule_b3, rule_b4, rule_b5, rule_b6, rule_b7, rule_b8, rule_b9])
base_sim = ctrl.ControlSystemSimulation(base_ctrl)

# ==========================================
# 第二阶段：时间调整控制器 (污浊度 + 污浊类型 -> 调整时间)
# ==========================================
dirt = ctrl.Antecedent(np.arange(0, 101, 1), '污浊程度 (%)')
dirt_type = ctrl.Antecedent(np.arange(0, 101, 1), '污浊类型 (0非油性-100油性顽固)')
time_adj = ctrl.Consequent(np.arange(-15, 16, 1), '调整时间 (分钟)')

# 隶属度函数
dirt['轻微'] = fuzz.trimf(dirt.universe, [0, 0, 50])
dirt['中等'] = fuzz.trimf(dirt.universe, [0, 50, 100])
dirt['严重'] = fuzz.trimf(dirt.universe, [50, 100, 100])

dirt_type['水溶性(易洗)'] = fuzz.trimf(dirt_type.universe, [0, 0, 50])
dirt_type['混合性(常规)'] = fuzz.trimf(dirt_type.universe, [0, 50, 100])
dirt_type['油性(顽固)'] = fuzz.trimf(dirt_type.universe, [50, 100, 100])

time_adj['大幅减少'] = fuzz.trimf(time_adj.universe, [-15, -15, -5])
time_adj['微调(不变)'] = fuzz.trimf(time_adj.universe, [-5, 0, 5])
time_adj['大幅增加'] = fuzz.trimf(time_adj.universe, [5, 15, 15])

# 规则 (污渍情况决定加减时)
rule_a1 = ctrl.Rule(dirt['轻微'] & dirt_type['水溶性(易洗)'], time_adj['大幅减少'])
rule_a2 = ctrl.Rule(dirt['轻微'] & dirt_type['混合性(常规)'], time_adj['大幅减少'])
rule_a3 = ctrl.Rule(dirt['轻微'] & dirt_type['油性(顽固)'], time_adj['微调(不变)'])
rule_a4 = ctrl.Rule(dirt['中等'] & dirt_type['水溶性(易洗)'], time_adj['大幅减少'])
rule_a5 = ctrl.Rule(dirt['中等'] & dirt_type['混合性(常规)'], time_adj['微调(不变)'])
rule_a6 = ctrl.Rule(dirt['中等'] & dirt_type['油性(顽固)'], time_adj['大幅增加'])
rule_a7 = ctrl.Rule(dirt['严重'] & dirt_type['水溶性(易洗)'], time_adj['微调(不变)'])
rule_a8 = ctrl.Rule(dirt['严重'] & dirt_type['混合性(常规)'], time_adj['大幅增加'])
rule_a9 = ctrl.Rule(dirt['严重'] & dirt_type['油性(顽固)'], time_adj['大幅增加'])

adj_ctrl = ctrl.ControlSystem([rule_a1, rule_a2, rule_a3, rule_a4, rule_a5, rule_a6, rule_a7, rule_a8, rule_a9])
adj_sim = ctrl.ControlSystemSimulation(adj_ctrl)

# ==========================================
# 交互与计算
# ==========================================
print("--- 欢迎使用 双阶模糊控制 全自动洗衣机系统 ---")
try:
    test_w = float(input("1. 请输入衣物重量 (0 - 20 kg): "))
    test_m = float(input("2. 请输入衣服材质 (0丝绸轻薄 - 100牛仔厚重): "))
    test_d = float(input("3. 请输入污浊程度 (0 - 100 %): "))
    test_dt = float(input("4. 请输入污浊类型 (0非油性 - 100油性顽固): "))
except ValueError:
    print("输入错误，采用默认值测试...")
    test_w, test_m, test_d, test_dt = 10, 50, 80, 80

# 第一阶段计算
base_sim.input['衣物重量 (kg)'] = np.clip(test_w, 0, 20)
base_sim.input['衣服材质 (0轻薄-100厚重)'] = np.clip(test_m, 0, 100)
base_sim.compute()
c_base = base_sim.output['基础时间 (分钟)']

# 第二阶段计算
adj_sim.input['污浊程度 (%)'] = np.clip(test_d, 0, 100)
adj_sim.input['污浊类型 (0非油性-100油性顽固)'] = np.clip(test_dt, 0, 100)
adj_sim.compute()
c_adj = adj_sim.output['调整时间 (分钟)']

final_time = max(c_base + c_adj, 5) # 保证总时间不低于5分钟

print("\n" + "="*40)
print(f"【第一阶段】根据 重量({test_w}kg) 和 材质({test_m}) -> 估算基础时间: {c_base:.2f} 分钟")
print(f"【第二阶段】根据 污浊度({test_d}%) 和 类型({test_dt}) -> 智能调整时间: {c_adj:+.2f} 分钟")
print(f"【最终结论】精确洗涤时间: {final_time:.2f} 分钟")
print("="*40 + "\n正在生成双阶段3D控制曲面图...")

# ==========================================
# 可视化 (双 3D 曲面图)
# ==========================================
fig = plt.figure(figsize=(16, 7))

# 图1：基础时间曲面
ax1 = fig.add_subplot(121, projection='3d')
x_w, y_m = np.meshgrid(np.arange(0, 21, 1), np.arange(0, 101, 5))
z_base = np.zeros_like(x_w, dtype=float)
for i in range(x_w.shape[0]):
    for j in range(x_w.shape[1]):
        base_sim.input['衣物重量 (kg)'] = x_w[i, j]
        base_sim.input['衣服材质 (0轻薄-100厚重)'] = y_m[i, j]
        base_sim.compute()
        z_base[i, j] = base_sim.output['基础时间 (分钟)']
ax1.plot_surface(x_w, y_m, z_base, cmap='Blues', alpha=0.8)
ax1.scatter(test_w, test_m, c_base, color='red', s=100, label=f'基础时间点: {c_base:.1f}分')
ax1.set_title('第一阶段: 基础时间策略面')
ax1.set_xlabel('重量 (kg)'), ax1.set_ylabel('材质'), ax1.set_zlabel('时间 (分)')
ax1.legend()

# 图2：调整时间曲面
ax2 = fig.add_subplot(122, projection='3d')
x_d, y_dt = np.meshgrid(np.arange(0, 101, 5), np.arange(0, 101, 5))
z_adj = np.zeros_like(x_d, dtype=float)
for i in range(x_d.shape[0]):
    for j in range(x_d.shape[1]):
        adj_sim.input['污浊程度 (%)'] = x_d[i, j]
        adj_sim.input['污浊类型 (0非油性-100油性顽固)'] = y_dt[i, j]
        adj_sim.compute()
        z_adj[i, j] = adj_sim.output['调整时间 (分钟)']
ax2.plot_surface(x_d, y_dt, z_adj, cmap='Reds', alpha=0.8)
ax2.scatter(test_d, test_dt, c_adj, color='black', s=100, label=f'调整时间点: {c_adj:+.1f}分')
ax2.set_title('���二阶段: 时间微调策略面')
ax2.set_xlabel('污浊度 (%)'), ax2.set_ylabel('污浊类型'), ax2.set_zlabel('加减时间 (分)')
ax2.legend()

plt.tight_layout()
plt.show()