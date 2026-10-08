# XCPC Algorithm Templates (个人板子库)

> 维护者: mirai_39 / mirai0914  
> 目标: CCPC / ICPC 赛站冲银，赛前代码速打 & 现场打印手册 (Team Notebook)

---

## 目录结构

```text
templates/
├── .gitignore
├── README.md
├── legacy/                  # 从原工作区迁移的原始旧版本存档
└── src/
    ├── base/                # 竞赛缺省源文件与基础宏
    │   └── initial.cpp
    ├── data_structure/      # 数据结构
    │   ├── st_table.cpp     # 稀疏表 (ST Table)
    │   └── trie.cpp         # 字典树 (Trie)
    ├── math/                # 数论与数学
    │   ├── comb.cpp         # 组合数 / 阶乘逆元 / 快速幂
    │   ├── euler_sieve.cpp  # 线性筛素数 (欧拉筛)
    │   ├── spf.cpp          # 最小质因子 / 快速质因数分解
    │   └── prime_table.cpp  # 常用质数常量表
    └── misc/                # 杂项与技巧
        └── quick_select.cpp # 快速选择
```

---

## 码风与工程规范

1. **结构体自闭环**：所有算法/数据结构均采用 `struct` 封装，提供构造函数与 `build()`，多测测试清空自闭环。
2. **现代 C++ 习惯**：
   - 树/图遍历优先使用递归 lambda：`auto dfs = [&](auto&& self, int u, int p) -> void { ... };`。
   - 纯血 `std::vector`，严禁裸写 C 风格原生数组 `int[]`（常量查找表除外）。
   - 清空容器使用 `.assign()` 或 `.clear()`，严禁使用 `memset`。
   - 拒绝多余遍历，单趟扫描（如单调栈）在单 pass 内顺手维护左右界。
3. **多测清空绝对安全**：
   - 面对多测 $T$ 大且 $\sum n$ 有界的题目，严禁绑定全局最大值域（如 $MAXV$）全量复位，必须按照 $\mathcal{O}(n)$ 痕迹定向清空，杜绝假复杂度。

---

## 本地编译建议

- **Windows 本地运行大递归/深树**：本地 MinGW 默认栈仅 2MB，深搜极易爆栈，编译时务必指定栈空间扩大参数：
  ```bash
  g++ -O2 -std=c++20 solve.cpp -Wl,--stack=268435456 -o solve.exe
  ```

---

## 赛前打印指南 (Team Notebook)

- 后续将提供一键将 `src/` 提取为紧凑双栏 LaTeX / PDF 的打印脚本，便于在现场赛直接携带纸质手册。
