# XCPC Algorithm Templates (个人板子库)

> 维护者: mirai_39 / mirai0914  
> 目标: CCPC / ICPC 赛站冲银 · 赛前速查 & Team Notebook 手册  
> 定位: 仅收录高价值、推导繁琐、难默写的算法模板

---

## 目录索引

```text
templates/
├── .gitignore
├── README.md
└── src/
    └── math/
        └── euler_sieve.cpp   # 欧拉线性筛 (包含: 基础素数筛 + 扩展积性函数筛 phi & mu)
```

---

## 码风与规范

1. **结构体自闭环**：所有算法均采用 `struct` 封装，提供构造函数与 `build(n)`，多测清空自闭环。
2. **纯血 `std::vector`**：严禁裸写 C 风格原生数组 `int[]`，严禁 `memset`，清空重置统一使用 `.assign()` 或 `.clear()`。
3. **单趟极简**：单次线性扫描顺手维护所有信息，杜绝假复杂度。

---

## 本地编译提示

Windows 本地 MinGW 默认栈仅 2MB，遇深搜或极深树结构需添加扩大栈空间参数：
```bash
g++ -O2 -std=c++20 solve.cpp -Wl,--stack=268435456 -o solve.exe
```
