# XCPC Team Notebook · 个人板子手册

> **维护者**: `mirai_39` / `mirai0914`  
> **目标**: CCPC / ICPC 赛站冲银  
> **原则**: 纯血 `vector` · `struct` 自闭环封装 · 杜绝裸数组与 `memset` · 纯正竞赛风  

---

## 赛场常备编译命令 (Windows 本地深搜防爆栈)

```bash
g++ -O2 -std=c++20 solve.cpp -Wl,--stack=268435456 -o solve.exe
```

缺省快速 I/O:
```cpp
cin.tie(nullptr)->sync_with_stdio(false);
```

---

## 目录索引

- [1. 欧拉线性筛 (素数筛 + phi & mu)](#1-欧拉线性筛-素数筛--phi--mu)
- [2. 快速数论变换 (NTT 卷积 / 快速幂 / 分治连乘)](#2-快速数论变换-ntt-卷积--快速幂--分治连乘)
- [3. 分拆数 (五边形数定理 + 根号分治 DP)](#3-分拆数-五边形数定理--根号分治-dp)
- [4. 斯特林数 (O(N^2) 递推打表 + NTT 单行求解)](#4-斯特林数-on2-递推打表--ntt-单行求解)
- [5. 珂朵莉树 (ODT - Old Driver Tree)](#5-珂朵莉树-odt---old-driver-tree)
- [6. 主席树 (可持久化权值线段树 / 静态 & 树上第 k 小)](#6-主席树-可持久化权值线段树--静态--树上第-k-小)
- [7. 扩展中国剩余定理 (EXCRT - 不互质同余方程组)](#7-扩展中国剩余定理-excrt---不互质同余方程组)
- [8. 欧拉降幂 (扩展欧拉定理) 与单点大数欧拉函数](#8-欧拉降幂-扩展欧拉定理-与单点大数欧拉函数)
- [9. 线性基 (Linear Basis & 前缀区间线性基)](#9-线性基-linear-basis--前缀区间线性基)
- [10. PBDS (平衡树 ordered_set / 防卡哈希 / 可并配对堆)](#10-pbds-平衡树-ordered-set--防卡哈希--可并配对堆)

---

## 1. 欧拉线性筛 (素数筛 + phi & mu)

> **核心公式与备忘**:  
> - $\phi(p) = p - 1$；若 $i \bmod p = 0$ 则 $\phi(ip) = \phi(i) \cdot p$；否则 $\phi(ip) = \phi(i) \cdot (p - 1)$  
> - $\mu(1) = 1, \mu(p) = -1$；若 $i \bmod p = 0$ 则 $\mu(ip) = 0$；否则 $\mu(ip) = -\mu(i)$  

```cpp
#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/**
 * 欧拉线性筛 (基础素数筛 + 积性函数 phi & mu)
 * 复杂度: 时间 O(N), 空间 O(N)
 */

// 模块 1: 基础欧拉筛 (素数线性筛)
struct EulerSieve {
    int n = 0;
    vector<int> prm;
    vector<bool> isp; // isp[x] == false 表示 x 是质数

    EulerSieve(int max_n = 0) {
        if (max_n > 0) build(max_n);
    }

    // 功能: 线性筛预处理 1 ~ max_n 内所有素数
    // 传参: max_n 预处理上界
    void build(int max_n) {
        if (max_n < 0) return;
        n = max_n;
        prm.clear();
        isp.assign(n + 1, false);

        if (n >= 0) isp[0] = true;
        if (n >= 1) isp[1] = true;

        for (int i = 2; i <= n; ++i) {
            if (!isp[i]) prm.push_back(i);
            for (int p : prm) {
                if (1LL * i * p > n) break;
                isp[i * p] = true;
                if (i % p == 0) break;
            }
        }
    }

    // 功能: O(1) 判定 x 是否为质数
    bool is_prime(int x) const {
        if (x < 2 || x > n) return false;
        return !isp[x];
    }
};

// 模块 2: 扩展欧拉筛 (素数 + 欧拉函数 phi + 莫比乌斯函数 mu)
struct MultiplicativeSieve {
    int n = 0;
    vector<int> prm;
    vector<bool> isp;
    vector<int> phi, mu;

    MultiplicativeSieve(int max_n = 0) {
        if (max_n > 0) build(max_n);
    }

    // 功能: 线性筛同时预处理素数表、phi[1..n] 与 mu[1..n]
    // 传参: max_n 预处理上界
    void build(int max_n) {
        if (max_n < 0) return;
        n = max_n;
        prm.clear();
        isp.assign(n + 1, false);
        phi.assign(n + 1, 0);
        mu.assign(n + 1, 0);

        if (n >= 0) isp[0] = true;
        if (n >= 1) {
            isp[1] = true;
            phi[1] = mu[1] = 1;
        }

        for (int i = 2; i <= n; ++i) {
            if (!isp[i]) {
                prm.push_back(i);
                phi[i] = i - 1;
                mu[i] = -1;
            }
            for (int p : prm) {
                if (1LL * i * p > n) break;
                isp[i * p] = true;
                if (i % p == 0) {
                    phi[i * p] = phi[i] * p;
                    mu[i * p] = 0;
                    break;
                } else {
                    phi[i * p] = phi[i] * (p - 1);
                    mu[i * p] = -mu[i];
                }
            }
        }
    }

    // 功能: O(1) 判定 x 是否为质数
    bool is_prime(int x) const {
        if (x < 2 || x > n) return false;
        return !isp[x];
    }
};

/*
使用示范:
    EulerSieve es(1000000);
    bool ok = es.is_prime(998244353);

    MultiplicativeSieve ms(1000000);
    int p10 = ms.phi[10]; // 4
    int m6 = ms.mu[6];    // 1
*/
```

---

## 2. 快速数论变换 (NTT 卷积 / 快速幂 / 分治连乘)

> **核心公式与备忘**:  
> - 模数固定 $998244353$，原根 $G = 3$  
> - `mult`: $\mathcal{O}(N \log N)$；`poly_pow`: $\mathcal{O}(L \log L \log k)$；`poly_prod`: $\mathcal{O}(N \log^2 N)$  

```cpp
#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/**
 * 快速数论变换 (NTT - 模数 998244353, 原根 G = 3)
 * 复杂度: mult O(N log N), poly_pow O(L log L log k), poly_prod O(N log^2 N)
 */
struct NTT {
    const int MOD = 998244353;
    const int G = 3;

    // 功能: 快速幂 a^b % MOD
    // 传参: a 底数, b 指数
    ll ksm(ll a, ll b) const {
        ll res = 1;
        a = (a % MOD + MOD) % MOD;
        for (; b > 0; b >>= 1, a = a * a % MOD) {
            if (b & 1) res = res * a % MOD;
        }
        return res;
    }

    // 功能: 费马小定理求逆元
    // 传参: a 待求逆元的整数
    ll inv(ll a) const {
        return ksm(a, MOD - 2);
    }

    // 功能: NTT / INTT 核心变换 (系数 <-> 点值)
    // 传参: a 系数数组 (长度需为 2^k), inv_flag false: 正变换 / true: 逆变换
    void transf(vector<int>& a, bool inv_flag) const {
        int n = a.size();
        if (n <= 1) return;

        vector<int> rev(n, 0);
        int bit = __builtin_ctz(n);
        for (int i = 0; i < n; ++i) {
            rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (bit - 1));
            if (i < rev[i]) swap(a[i], a[rev[i]]);
        }

        for (int len = 2; len <= n; len <<= 1) {
            ll wlen = ksm(G, (MOD - 1) / len);
            if (inv_flag) wlen = inv(wlen);
            int half = len >> 1;

            for (int i = 0; i < n; i += len) {
                ll w = 1;
                for (int j = 0; j < half; ++j) {
                    int u = a[i + j];
                    int v = 1LL * a[i + j + half] * w % MOD;
                    a[i + j] = (u + v >= MOD ? u + v - MOD : u + v);
                    a[i + j + half] = (u - v < 0 ? u - v + MOD : u - v);
                    w = w * wlen % MOD;
                }
            }
        }

        if (inv_flag) {
            ll inv_n = inv(n);
            for (int i = 0; i < n; ++i) {
                a[i] = 1LL * a[i] * inv_n % MOD;
            }
        }
    }

    // 功能: 多项式乘法 (卷积) C(x) = A(x) * B(x)
    // 传参: a, b 多项式系数向量 (下标 i 为 x^i 项系数)
    // 返回: 卷积后的系数向量 (size = deg(A) + deg(B) + 1)
    vector<int> mult(vector<int> a, vector<int> b) const {
        if (a.empty() || b.empty()) return {};
        int sz = a.size() + b.size() - 1;
        int n = 1;
        while (n < sz) n <<= 1;

        a.resize(n, 0);
        b.resize(n, 0);
        transf(a, false);
        transf(b, false);
        for (int i = 0; i < n; ++i) {
            a[i] = 1LL * a[i] * b[i] % MOD;
        }
        transf(a, true);

        a.resize(sz);
        return a;
    }

    // 功能: 多项式快速幂 A^k(x)
    // 传参: a 底数多项式系数, k 幂次, lim 最高次数限制 (截断到 <= lim 项, 默认 -1 不截断)
    // 返回: 结果多项式系数向量
    vector<int> poly_pow(vector<int> a, ll k, int lim = -1) const {
        vector<int> res = {1};
        if (lim != -1 && (int)a.size() > lim + 1) a.resize(lim + 1);
        while (k > 0) {
            if (k & 1) {
                res = mult(res, a);
                if (lim != -1 && (int)res.size() > lim + 1) res.resize(lim + 1);
            }
            if (k > 1) {
                a = mult(a, a);
                if (lim != -1 && (int)a.size() > lim + 1) a.resize(lim + 1);
            }
            k >>= 1;
        }
        return res;
    }

    // 功能: 分治 NTT 求 n 个多项式连乘 F_1(x) * ... * F_n(x)
    // 传参: polys 待连乘的多项式集合, lim 最高次数限制 (默认 -1 不截断)
    // 返回: 连乘结果多项式系数向量
    vector<int> poly_prod(const vector<vector<int>>& polys, int lim = -1) const {
        if (polys.empty()) return {1};

        auto dfs = [&](auto&& self, int l, int r) -> vector<int> {
            if (l == r) {
                auto res = polys[l];
                if (lim != -1 && (int)res.size() > lim + 1) res.resize(lim + 1);
                return res;
            }
            int mid = l + (r - l) / 2;
            auto res = mult(self(self, l, mid), self(self, mid + 1, r));
            if (lim != -1 && (int)res.size() > lim + 1) res.resize(lim + 1);
            return res;
        };

        return dfs(dfs, 0, (int)polys.size() - 1);
    }
};

/*
使用示范:
    NTT ntt;
    vector<int> c = ntt.mult({1, 2}, {3, 4});                // {3, 10, 8}
    vector<int> p = ntt.poly_pow({1, 1}, 3, 2);              // {1, 3, 3}
    vector<int> prod = ntt.poly_prod({{1, 1}, {1, 2}, {1, 3}}); // {1, 6, 11, 6}
*/
```

---

## 3. 分拆数 (五边形数定理 + 根号分治 DP)

> **核心公式与备忘**:  
> - **广义五边形数**: $g_k = \frac{k(3k \pm 1)}{2}$，符号规律 $+ + - - + + - -$  
> - **根号分治 DP**: $\le B$ 完全背包；$> B$ 维护恰有 $c$ 个数之和为 $j$（整体 $+1$ 或新增 $B+1$）  

```cpp
#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/**
 * 分拆数 (五边形数定理预处理 + 根号分治 DP 单点求值)
 * 复杂度: 时间 O(N sqrt(N)), 空间 O(N)
 */

// 模块 1: 五边形数定理求 0 ~ N 全量分拆数
struct PartitionPentagonal {
    const int MOD = 998244353;
    int n = 0;
    vector<int> p; // p[i] 为整数 i 的分拆数

    PartitionPentagonal(int max_n = 0) {
        if (max_n >= 0) build(max_n);
    }

    // 功能: 预处理 0 ~ max_n 的分拆数
    // 传参: max_n 预处理上限
    void build(int max_n) {
        if (max_n < 0) return;
        n = max_n;
        p.assign(n + 1, 0);
        p[0] = 1;

        vector<array<int, 2>> pent; // 广义五边形数 {pos, sign}
        for (int k = 1;; ++k) {
            ll g1 = 1LL * k * (3LL * k - 1) / 2;
            ll g2 = 1LL * k * (3LL * k + 1) / 2;
            int sign = (k & 1) ? 1 : -1;
            if (g1 <= n) pent.push_back({(int)g1, sign});
            if (g2 <= n) pent.push_back({(int)g2, sign});
            if (g1 > n && g2 > n) break;
        }

        for (int i = 1; i <= n; ++i) {
            ll val = 0;
            for (const auto& [g, sign] : pent) {
                if (g > i) break;
                val += sign * p[i - g];
            }
            val %= MOD;
            if (val < 0) val += MOD;
            p[i] = (int)val;
        }
    }

    // 功能: O(1) 查询 p(x) % MOD (越界返回 0)
    int query(int x) const {
        if (x < 0 || x > n) return 0;
        return p[x];
    }
};

// 模块 2: 根号分治 DP (单点求 p(n) 或魔改带限制分拆数)
struct PartitionSqrt {
    const int MOD = 998244353;

    // 功能: 单点计算整数 n 的分拆数 p(n) % MOD
    // 传参: n 目标非负整数
    // 返回: p(n) % MOD
    int calc(int n) const {
        if (n < 0) return 0;
        if (n <= 1) return 1;

        int B = sqrt(n);
        while (1LL * (B + 1) * (B + 1) <= n) ++B;
        while (1LL * B * B > n) --B;

        // 1. 小数部分 (<= B): 完全背包
        vector<int> f(n + 1, 0);
        f[0] = 1;
        for (int v = 1; v <= B; ++v) {
            for (int j = v; j <= n; ++j) {
                f[j] += f[j - v];
                if (f[j] >= MOD) f[j] -= MOD;
            }
        }

        // 2. 大数部分 (> B): 滚动 DP, 最多有 n / (B + 1) 个数
        vector<int> prev(n + 1, 0), cur(n + 1, 0), h(n + 1, 0);
        prev[0] = h[0] = 1;

        int max_c = n / (B + 1);
        for (int c = 1; c <= max_c; ++c) {
            cur.assign(n + 1, 0);
            for (int j = c * (B + 1); j <= n; ++j) {
                int ways = prev[j - (B + 1)] + cur[j - c];
                if (ways >= MOD) ways -= MOD;
                cur[j] = ways;
                h[j] += ways;
                if (h[j] >= MOD) h[j] -= MOD;
            }
            prev = move(cur);
        }

        // 3. 点积合并
        ll ans = 0;
        for (int k = 0; k <= n; ++k) {
            ans = (ans + 1LL * f[k] * h[n - k]) % MOD;
        }
        return (int)ans;
    }
};

/*
使用示范:
    PartitionPentagonal pp(100000);
    int p5 = pp.query(5);   // 7

    PartitionSqrt ps;
    int p100 = ps.calc(100); // 190569291
*/
```

---

## 4. 斯特林数 (O(N^2) 递推打表 + NTT 单行求解)

> **核心公式与备忘**:  
> - 普通幂转下降幂: $x^n = \sum_{k=0}^n \{n, k\} x^{\underline{k}}$；上升幂: $x^{\overline{n}} = \sum_{k=0}^n [n, k] x^k$  
> - 第二类卷积通项: $\{n, k\} = \sum_{j=0}^k \frac{(-1)^{k-j}}{(k-j)!} \cdot \frac{j^n}{j!}$  

```cpp
#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/**
 * 斯特林数 (O(N^2) 递推打表 + NTT 单行求解)
 * 常用恒等式:
 *   x^n = sum {n, k} x^{underline k}
 *   x^{overline n} = sum [n, k] x^k
 */

// 模块 1: O(N^2) 斯特林数打表 (适合 N <= 5000)
struct StirlingTable {
    const int MOD = 998244353;
    int n = 0;
    vector<vector<int>> s1, s2; // s1: 第一类 [n, k], s2: 第二类 {n, k}

    StirlingTable(int max_n = 0) {
        if (max_n >= 0) build(max_n);
    }

    // 功能: 预处理 0 ~ max_n 的第一类与第二类斯特林数
    // 传参: max_n 预处理规模
    void build(int max_n) {
        if (max_n < 0) return;
        n = max_n;
        s1.assign(n + 1, vector<int>(n + 1, 0));
        s2.assign(n + 1, vector<int>(n + 1, 0));

        s1[0][0] = s2[0][0] = 1;
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= i; ++j) {
                s1[i][j] = (s1[i - 1][j - 1] + 1LL * (i - 1) * s1[i - 1][j]) % MOD;
                s2[i][j] = (s2[i - 1][j - 1] + 1LL * j * s2[i - 1][j]) % MOD;
            }
        }
    }

    // 功能: 查询第一类斯特林数 [x, y]
    int query_s1(int x, int y) const {
        if (x < 0 || y < 0 || y > x || x > n) return 0;
        return s1[x][y];
    }

    // 功能: 查询第二类斯特林数 {x, y}
    int query_s2(int x, int y) const {
        if (x < 0 || y < 0 || y > x || x > n) return 0;
        return s2[x][y];
    }
};

// 模块 2: 快速单行求解 (适合 N <= 10^5, 依赖 NTT)
struct StirlingRow {
    const int MOD = 998244353;

    // 功能: NTT 卷积求解第二类斯特林数第 n 行 {n, 0 ... n} (O(N log N))
    // 传参: n 行号, ntt NTT 实例
    template <typename NTTOp>
    vector<int> row_s2(int n, const NTTOp& ntt) const {
        if (n == 0) return {1};

        vector<int> fac(n + 1, 1), ifac(n + 1, 1);
        for (int i = 1; i <= n; ++i) fac[i] = 1LL * fac[i - 1] * i % MOD;
        ifac[n] = ntt.inv(fac[n]);
        for (int i = n; i >= 1; --i) ifac[i - 1] = 1LL * ifac[i] * i % MOD;

        vector<int> a(n + 1), b(n + 1);
        for (int i = 0; i <= n; ++i) {
            a[i] = (i & 1) ? (MOD - ifac[i]) % MOD : ifac[i];
            b[i] = 1LL * ntt.ksm(i, n) * ifac[i] % MOD;
        }

        auto c = ntt.mult(a, b);
        c.resize(n + 1);
        return c;
    }

    // 功能: 分治 NTT 求解第一类斯特林数第 n 行 [n, 0 ... n] (O(N log^2 N))
    // 传参: n 行号, ntt NTT 实例
    template <typename NTTOp>
    vector<int> row_s1(int n, const NTTOp& ntt) const {
        if (n == 0) return {1};
        vector<vector<int>> polys(n);
        for (int i = 0; i < n; ++i) polys[i] = {i, 1};
        return ntt.poly_prod(polys);
    }
};

/*
使用示范:
    StirlingTable st(2000);
    int v1 = st.query_s1(4, 2); // [4, 2] = 11
    int v2 = st.query_s2(4, 2); // {4, 2} = 7

    NTT ntt; StirlingRow sr;
    vector<int> r2 = sr.row_s2(100000, ntt);
    vector<int> r1 = sr.row_s1(100000, ntt);
*/
```

---

## 5. 珂朵莉树 (ODT - Old Driver Tree)

> **核心公式与备忘**:  
> - **核心避坑**: 双端点操作必须 **先 `split(r + 1)`，再 `split(l)`**！  
> - 存储结构 `set<array<ll, 3>>`，随机 `assign` 下块数期望 $\mathcal{O}(\log \log n)$  

```cpp
#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/**
 * 珂朵莉树 (ODT - Old Driver Tree)
 * 结构: set<array<ll, 3>>, 节点为 {l, r, v} (区间值支持 long long)
 * 核心避坑: 双端点切分必须【先 split(r + 1)，再 split(l)】，防止迭代器失效!
 */
struct ODT {
    set<array<ll, 3>> s;

    ODT(int n, ll v = 0) {
        s.insert({1, n, v});
    }

    // 功能: 切分包含位置 x 的区间为 [l, x-1] 和 [x, r]
    // 传参: x 切分点 (将成为右区间左端点；若超出维护右界则返回 s.end())
    auto split(int x) {
        auto it = s.lower_bound({(ll)x, -1LL, LLONG_MIN});
        if (it != s.end() && (*it)[0] == x) return it;
        if (it == s.begin()) return s.end();
        --it;
        if ((*it)[1] < x) return s.end();
        auto [l, r, v] = *it;
        s.erase(it);
        s.insert({l, x - 1, v});
        return s.insert({x, r, v}).first;
    }

    // 功能: 区间推平赋值 [l, r] = v (维持复杂度核心操作)
    void assign(int l, int r, ll v) {
        auto itr = split(r + 1);
        auto itl = split(l);
        s.erase(itl, itr);
        s.insert({l, r, v});
    }

    // 功能: 区间加 [l, r] += v
    void add(int l, int r, ll v) {
        auto itr = split(r + 1);
        auto itl = split(l);
        vector<array<ll, 3>> tmp;
        for (auto it = itl; it != itr; ++it) {
            tmp.push_back({(*it)[0], (*it)[1], (*it)[2] + v});
        }
        s.erase(itl, itr);
        for (const auto& node : tmp) s.insert(node);
    }

    // 功能: 查询区间 [l, r] 第 k 小值 (1-indexed)
    ll kth(int l, int r, int k) {
        auto itr = split(r + 1);
        auto itl = split(l);
        vector<array<ll, 2>> a;
        for (auto it = itl; it != itr; ++it) {
            a.push_back({(*it)[2], (*it)[1] - (*it)[0] + 1});
        }
        sort(a.begin(), a.end());
        for (const auto& [val, len] : a) {
            k -= len;
            if (k <= 0) return val;
        }
        return -1;
    }

    // 功能: 查询区间 [l, r] 元素 p 次方和 sum_{i=l}^r (a[i]^p) % mod
    ll sum_pow(int l, int r, int p, ll mod) {
        auto itr = split(r + 1);
        auto itl = split(l);
        auto ksm = [&](ll base, int exp) -> ll {
            ll res = 1 % mod;
            base = (base % mod + mod) % mod;
            while (exp > 0) {
                if (exp & 1) res = (__int128_t)res * base % mod;
                base = (__int128_t)base * base % mod;
                exp >>= 1;
            }
            return res;
        };
        ll ans = 0;
        for (auto it = itl; it != itr; ++it) {
            ll len = (*it)[1] - (*it)[0] + 1;
            ans = (ans + len % mod * ksm((*it)[2], p)) % mod;
        }
        return ans;
    }
};

/*
使用示范:
    ODT odt(100000, 0);
    odt.add(1, 10, 5);
    odt.assign(3, 8, 7);
    ll val = odt.kth(1, 10, 4);
    ll sum2 = odt.sum_pow(1, 10, 2, 1000000007LL);
*/
```

---

## 6. 主席树 (可持久化权值线段树 / 静态 & 树上第 k 小)

> **核心公式与备忘**:  
> - 前缀版本差分 `roots[r] - roots[l - 1]`，动态开点空间 $\mathcal{O}(N \log M)$，支持 `int` / `ll`。  
> - 扩展：单趟区间值域计数 `count_range`、树上路径第 $k$ 小 `query_path_kth`。  

```cpp
#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/**
 * 主席树 (可持久化权值线段树 - Persistent Segment Tree)
 * 复杂度: 空间 O(N log M) 动态开点, 单次插/查 O(log M), 建树 O(N log N)
 * 特性: 支持 int / ll 输入数组 (1-indexed), 内置离散化与多测自闭环清空。
 */
struct PersistentSegTree {
    struct Node {
        int lc = 0, rc = 0, cnt = 0;
    };

    int n = 0, m = 0;
    vector<int> roots;
    vector<Node> tr;
    vector<ll> vals; // 离散化去重原值表

    PersistentSegTree() {}
    template <typename T>
    PersistentSegTree(const vector<T>& a) {
        build(a);
    }

    // 功能: 在历史版本 p 基础上，值域位置 pos 计数 +1 生成新版本
    // 传参: p 旧版本根编号, l, r 当前值域, pos 值域下标 (1 <= pos <= m)
    int insert(int p, int l, int r, int pos) {
        int u = tr.size();
        tr.push_back(tr[p]);
        tr[u].cnt++;
        if (l == r) return u;
        int mid = l + (r - l) / 2;
        if (pos <= mid) tr[u].lc = insert(tr[p].lc, l, mid, pos);
        else tr[u].rc = insert(tr[p].rc, mid + 1, r, pos);
        return u;
    }

    // 功能: 版本差分二分第 k 小值域下标 (u 为右界版本, v 为左界前驱版本)
    int query(int u, int v, int l, int r, int k) const {
        if (l == r) return l;
        int left_cnt = tr[tr[u].lc].cnt - tr[tr[v].lc].cnt;
        int mid = l + (r - l) / 2;
        if (k <= left_cnt) return query(tr[u].lc, tr[v].lc, l, mid, k);
        return query(tr[u].rc, tr[v].rc, mid + 1, r, k - left_cnt);
    }

    // 功能: 一键构建前缀主席树 (a 须为 1-indexed)
    template <typename T>
    void build(const vector<T>& a) {
        if ((int)a.size() <= 1) {
            n = m = 0;
            roots.assign(1, 0);
            tr.assign(1, Node{});
            vals.clear();
            return;
        }
        n = a.size() - 1;
        roots.assign(n + 1, 0);
        tr.clear();
        tr.reserve(n * 20 + 5);
        tr.emplace_back(); // 0 号空节点

        vals.assign(a.begin() + 1, a.end());
        sort(vals.begin(), vals.end());
        vals.erase(unique(vals.begin(), vals.end()), vals.end());
        m = vals.size();

        for (int i = 1; i <= n; ++i) {
            roots[i] = insert(roots[i - 1], get_pos(a[i]));
        }
    }

    // --- 一行小封装入口 ---
    int insert(int p, int pos) { return insert(p, 1, m, pos); }
    int get_pos(ll x) const { return lower_bound(vals.begin(), vals.end(), x) - vals.begin() + 1; }
    ll query_kth(int l, int r, int k) const { return vals[query(roots[r], roots[l - 1], 1, m, k) - 1]; }

    // --- 可选扩展 1: 区间值域计数 / 二维数点 ---
    int query_count(int u, int v, int l, int r, int ql, int qr) const {
        if (ql > r || qr < l || ql > qr) return 0;
        if (ql <= l && r <= qr) return tr[u].cnt - tr[v].cnt;
        int mid = l + (r - l) / 2;
        return query_count(tr[u].lc, tr[v].lc, l, mid, ql, qr) +
               query_count(tr[u].rc, tr[v].rc, mid + 1, r, ql, qr);
    }
    int count_le(int l, int r, ll x) const {
        int qr = upper_bound(vals.begin(), vals.end(), x) - vals.begin();
        return query_count(roots[r], roots[l - 1], 1, m, 1, qr);
    }
    int count_range(int l, int r, ll x, ll y) const {
        if (x > y) return 0;
        int ql = lower_bound(vals.begin(), vals.end(), x) - vals.begin() + 1;
        int qr = upper_bound(vals.begin(), vals.end(), y) - vals.begin();
        return query_count(roots[r], roots[l - 1], 1, m, ql, qr);
    }

    // --- 可选扩展 2: 树上路径第 k 小 (u + v - lca - fa_lca 差分) ---
    int query_tree(int u, int v, int lca, int fa_lca, int l, int r, int k) const {
        if (l == r) return l;
        int left_cnt = tr[tr[u].lc].cnt + tr[tr[v].lc].cnt - tr[tr[lca].lc].cnt - tr[tr[fa_lca].lc].cnt;
        int mid = l + (r - l) / 2;
        if (k <= left_cnt) return query_tree(tr[u].lc, tr[v].lc, tr[lca].lc, tr[fa_lca].lc, l, mid, k);
        return query_tree(tr[u].rc, tr[v].rc, tr[lca].rc, tr[fa_lca].rc, mid + 1, r, k - left_cnt);
    }
    ll query_path_kth(int u, int v, int lca, int fa_lca, int k) const {
        return vals[query_tree(roots[u], roots[v], roots[lca], roots[fa_lca], 1, m, k) - 1];
    }
};

/*
使用示范:
    vector<int> a = {0, 3, -1, 5, 2, 4, 2, 5, 10, -5}; // 1-indexed
    PersistentSegTree pst(a);
    ll val = pst.query_kth(2, 6, 3);
    int cnt = pst.count_range(2, 6, 2, 4);
*/
```

---

## 7. 扩展中国剩余定理 (EXCRT - 不互质同余方程组)

> **核心公式与备忘**:  
> - 联立 $ans + t \cdot M \equiv r \pmod m$，无解条件 $(r - ans) \bmod g \neq 0$  
> - 中间乘法与取模全程使用 `i128` (`__int128_t`) 杜绝 64 位溢出。  

```cpp
#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using i128 = __int128_t;

/**
 * 扩展中国剩余定理 (EXCRT - 求解模数不互质同余方程组 x = r_i mod m_i)
 * 复杂度: 单次合并 O(log m_i), 总计 O(n log(lcm)), 空间 O(1)
 * 核心推导: ans + t*M = r (mod m) => t*M = r - ans (mod m), 全程 i128 防溢出!
 */
struct EXCRT {
    ll ans = 0;    // 当前最小非负解
    ll M = 1;      // 当前公共模数 lcm
    bool ok = true;

    EXCRT() {}

    // 功能: 清空状态 (多测安全)
    void clear() {
        ans = 0;
        M = 1;
        ok = true;
    }

    // 功能: 扩展欧几里得求 a*x + b*y = gcd(a, b)
    ll exgcd(ll a, ll b, ll& x, ll& y) const {
        if (!b) {
            x = 1; y = 0;
            return a;
        }
        ll g = exgcd(b, a % b, y, x);
        y -= a / b * x;
        return g;
    }

    // 功能: 动态并入一个同余方程 x = r (mod m)
    // 返回: 并入后方程组是否有解
    bool add(ll r, ll m) {
        if (!ok) return false;
        r = (r % m + m) % m;

        ll t, y;
        ll g = exgcd(M, m, t, y);
        i128 diff = (i128)r - ans;
        if (diff % g != 0) {
            ok = false;
            return false;
        }

        ll mod_step = m / g;
        i128 k = (diff / g) % mod_step;
        t = ((i128)t * k % mod_step + mod_step) % mod_step;

        i128 new_M = (i128)M * mod_step;
        ans = (ll)(((i128)ans + (i128)t * M) % new_M);
        M = (ll)new_M;
        return true;
    }

    // 功能: 一次性求解方程组 x = r[i] (mod m[i])
    // 返回: {最小非负解 x, lcm 模数 M} (无解返回 {-1, -1})
    array<ll, 2> solve(const vector<ll>& r, const vector<ll>& m) {
        clear();
        for (int i = 0; i < (int)r.size(); ++i) {
            if (!add(r[i], m[i])) return {-1, -1};
        }
        return {ans, M};
    }
};

/*
使用示范:
    EXCRT excrt;
    auto [ans, M] = excrt.solve({2, 3, 2}, {3, 5, 7}); // ans = 23, M = 105
*/
```

---

## 8. 欧拉降幂 (扩展欧拉定理) 与单点大数欧拉函数

> **核心公式与备忘**:  
> - **单点欧拉函数**: $\mathcal{O}(\sqrt{n})$ 试除质因数，$\phi(n) = n \prod_{p \mid n} \frac{p-1}{p}$  
> - **扩展欧拉定理**: $b < \phi(m)$ 直接算；$b \ge \phi(m)$ 取 $a^{(b \bmod \phi(m)) + \phi(m)} \bmod m$  

```cpp
#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using i128 = __int128_t;

/**
 * 欧拉降幂 (扩展欧拉定理) 与 O(sqrt(N)) 单点大数欧拉函数
 * 扩展欧拉定理 (无需 gcd(a, m) = 1):
 *   a^b = a^b % m                         (b < phi(m))
 *   a^b = a^( (b % phi(m)) + phi(m) ) % m  (b >= phi(m))
 */
struct EulerPower {
    // 功能: O(sqrt(n)) 试除法求单个大数的欧拉函数 phi(n)
    // 传参: n 目标正整数 (支持至 1e14~1e18)
    ll get_phi(ll n) const {
        ll ans = n;
        for (ll i = 2; i * i <= n; ++i) {
            if (n % i == 0) {
                ans = ans / i * (i - 1);
                while (n % i == 0) n /= i;
            }
        }
        if (n > 1) ans = ans / n * (n - 1);
        return ans;
    }

    // 功能: 快速幂 a^b % m (i128 防溢出, 支持负底数)
    ll ksm(ll a, ll b, ll m) const {
        ll res = 1 % m;
        a = (a % m + m) % m;
        while (b > 0) {
            if (b & 1) res = (i128)res * a % m;
            if (b > 1) a = (i128)a * a % m;
            b >>= 1;
        }
        return res;
    }

    // 功能: 扩展欧拉定理求 a^b % m (b 为大数字符串)
    // 传参: a 底数, b 指数字符串, m 模数
    ll pow_str(ll a, const string& b, ll m) const {
        if (m == 1) return 0;
        ll p = get_phi(m);
        i128 exp = 0;
        bool ge = false;
        for (char c : b) {
            exp = exp * 10 + (c - '0');
            if (exp >= p) {
                ge = true;
                exp %= p;
            }
        }
        if (ge) exp += p;
        return ksm(a, (ll)exp, m);
    }

    // 功能: 扩展欧拉定理求 a^b % m (b 为数值类型)
    ll pow_num(ll a, ll b, ll m) const {
        if (m == 1) return 0;
        ll p = get_phi(m);
        if (b >= p) b = (b % p) + p;
        return ksm(a, b, m);
    }

    // --- 可选扩展: 幂塔递归降幂 (计算 a[l] ^ a[l+1] ^ ... ^ a[r] % m) ---
    ll ksm_ge(ll a, ll b, ll m) const {
        ll res = 1;
        bool flag = (1 >= m);
        if (res >= m) res %= m;
        if (a >= m) { flag = true; a %= m; }
        while (b > 0) {
            if (b & 1) {
                i128 t = (i128)res * a;
                if (t >= m) flag = true;
                res = t % m;
            }
            if (b > 1) {
                i128 t = (i128)a * a;
                if (t >= m) flag = true;
                a = t % m;
            }
            b >>= 1;
        }
        return res + (flag ? m : 0);
    }

    // 功能: 递归计算幂塔有效降幂值 (对 m 取模即得最终答案)
    ll power_tower(const vector<ll>& a, int l, int r, ll m) const {
        if (l > r || m == 1) return 1 >= m ? 1 % m + m : 1;
        if (l == r) return a[l] >= m ? a[l] % m + m : a[l];
        ll p = get_phi(m);
        ll exp = power_tower(a, l + 1, r, p);
        return ksm_ge(a[l], exp, m);
    }

    // 一行小封装: 幂塔求模
    ll query_tower(const vector<ll>& a, int l, int r, ll m) const {
        return power_tower(a, l, r, m) % m;
    }
};

/*
使用示范:
    EulerPower ep;
    ll phi_val = ep.get_phi(1000000007LL);
    ll ans1 = ep.pow_str(2, "1000000000000000000000000000000", 1000000007);
    ll ans2 = ep.query_tower({2, 3, 2}, 0, 2, 1000); // 512
*/
```

---

## 9. 线性基 (Linear Basis & 前缀区间线性基)

> **核心公式与备忘**:  
> - 秩为 $r$ 张成 $2^r$ 种值；存在线性相关则 $0$ 占位第 $1$ 小。`rebuild()` 为行最简形（RREF）。  
> - **前缀线性基**: 贪心保留更靠右的基下标 `pos[i]`，单次 $\mathcal{O}(B)$ 在线求区间 $[l, r]$ 最大异或和。  

```cpp
#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/**
 * 线性基 (Linear Basis / XOR Basis & 前缀线性基)
 * 复杂度: 单次插入/查询 O(B), 规范化 O(B^2), 合并 O(B^2)
 * 注意: 非空基大小为 r 则可张成 2^r 种值；若线性相关或含 0，则第 1 小异或和为 0。
 */
struct LinearBasis {
    int B = 60;                     // 最高有效位数 (默认支持 0 ~ 2^60-1)
    vector<ll> d, p;                // d[i]: 主元在第 i 位的基; p: RREF 规范化基
    int cnt = 0;                    // 当前基大小 (秩)
    bool has_zero = false;          // 空间是否可表示 0
    bool rebuilt = false;           // 是否已规范化

    LinearBasis(int b = 60) : B(b), d(b, 0) {}

    // 功能: 重置清空线性基 (多测安全)
    void clear() {
        d.assign(B, 0);
        p.clear();
        cnt = 0;
        has_zero = false;
        rebuilt = false;
    }

    // 功能: 插入元素 x (与已有基线性无关返回 true)
    bool insert(ll x) {
        rebuilt = false;
        if (x == 0) {
            has_zero = true;
            return false;
        }
        for (int i = B - 1; i >= 0; --i) {
            if ((x >> i) & 1) {
                if (!d[i]) {
                    d[i] = x;
                    cnt++;
                    return true;
                }
                x ^= d[i];
            }
        }
        has_zero = true;
        return false;
    }

    // 功能: 判断 x 能否被当前线性基异或表出
    bool check(ll x) const {
        for (int i = B - 1; i >= 0; --i) {
            if ((x >> i) & 1) {
                if (!d[i]) return false;
                x ^= d[i];
            }
        }
        return true;
    }

    // 功能: 查询与 base 异或的最大值 (默认查空间最大异或和)
    ll query_max(ll base = 0) const {
        ll ans = base;
        for (int i = B - 1; i >= 0; --i) {
            if ((ans ^ d[i]) > ans) ans ^= d[i];
        }
        return ans;
    }

    // 功能: 查询张成空间最小异或和 (allow_zero 控制是否允许返回 0)
    ll query_min(bool allow_zero = false) const {
        if (allow_zero && has_zero) return 0;
        for (int i = 0; i < B; ++i) {
            if (d[i]) return d[i];
        }
        return 0;
    }

    // 功能: 高斯消元规范化 (行最简形 RREF, 用于查询第 k 小)
    void rebuild() {
        p.clear();
        for (int i = 0; i < B; ++i) {
            if (d[i]) {
                for (int j = i + 1; j < B; ++j) {
                    if ((d[j] >> i) & 1) d[j] ^= d[i];
                }
            }
        }
        for (int i = 0; i < B; ++i) {
            if (d[i]) p.push_back(d[i]);
        }
        rebuilt = true;
    }

    // 功能: 查询非空子集第 k 小异或和 (1-indexed; 越界或 k<=0 返回 -1)
    ll query_kth(ll k) {
        if (k <= 0) return -1;
        if (!rebuilt) rebuild();
        if (has_zero) --k;
        if (k == 0) return 0;
        if ((int)p.size() < 63 && (unsigned long long)k >= (1ULL << p.size())) return -1;
        ll ans = 0;
        for (int i = 0; i < (int)p.size(); ++i) {
            if ((k >> i) & 1) ans ^= p[i];
        }
        return ans;
    }

    // 功能: 将另一个线性基 other 合并入当前基
    void merge(const LinearBasis& other) {
        for (int i = other.B - 1; i >= 0; --i) {
            if (other.d[i]) insert(other.d[i]);
        }
        if (other.has_zero) has_zero = true;
    }
};

// --- 可选扩展: 前缀线性基 (静态区间 [l, r] 最大异或和在线查询) ---
struct PrefixLinearBasis {
    int B = 60;
    vector<ll> d;
    vector<int> pos; // 基向量来自原数组的最大下标 (1-indexed)

    PrefixLinearBasis(int b = 60) : B(b), d(b, 0), pos(b, 0) {}

    void clear() {
        d.assign(B, 0);
        pos.assign(B, 0);
    }

    // 功能: 在上一前缀基础上插入下标 id 的元素 x
    void insert(ll x, int id) {
        for (int i = B - 1; i >= 0; --i) {
            if ((x >> i) & 1) {
                if (!d[i]) {
                    d[i] = x;
                    pos[i] = id;
                    return;
                }
                if (pos[i] < id) {
                    swap(pos[i], id);
                    swap(d[i], x);
                }
                x ^= d[i];
            }
        }
    }

    // 功能: 在 pre[r] 上查询区间 [l, r] 子集最大异或和
    ll query_max(int l, ll base = 0) const {
        ll ans = base;
        for (int i = B - 1; i >= 0; --i) {
            if (pos[i] >= l && (ans ^ d[i]) > ans) ans ^= d[i];
        }
        return ans;
    }
};

/*
使用示范:
    LinearBasis lb(60);
    for (ll x : a) lb.insert(x);
    ll mx = lb.query_max(), kth = lb.query_kth(k);

    vector<PrefixLinearBasis> pre(n + 1, PrefixLinearBasis(60));
    for (int i = 1; i <= n; ++i) { pre[i] = pre[i - 1]; pre[i].insert(a[i], i); }
    ll ans = pre[r].query_max(l);
*/
```

---

## 10. PBDS (平衡树 ordered_set / 防卡哈希 / 可并配对堆)

> **核心公式与备忘**:  
> - `order_of_key(x)`: 严格小于 $x$ 的个数；`find_by_order(k)`: 0-indexed 第 $k$ 小迭代器。  
> - **严禁传 `less_equal<T>`**；哈希防 CF Hack 必须配 `custom_hash`；配对堆 `join` 后原堆置空。  

```cpp
#pragma once
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/pb_ds/hash_policy.hpp>
#include <ext/pb_ds/priority_queue.hpp>

using namespace std;
using namespace __gnu_pbds;

using ll = long long;

/**
 * PBDS (GCC 扩展库常用容器: 名次树 / 防卡哈希 / 可并配对堆)
 * 注意:
 *   1. order_of_key(x): 严格小于 x 的个数; find_by_order(k): 0-indexed 第 k 小迭代器
 *   2. 多重集严禁写 less_equal<T> (会导致 lower_bound/erase 异常), 存 array<T, 2>{val, ++uid}
 *   3. h1.join(h2): O(1) 合并可并堆, h2 会被置空; modify/erase 支持 point_iterator
 */

// 1. 平衡树 (Order Statistics Tree)
template <typename T>
using ordered_set = tree<
    T,
    null_type,
    less<T>,
    rb_tree_tag,
    tree_order_statistics_node_update
>;

// 2. 防卡哈希表 (gp_hash_table + splitmix64 运行期随机盐)
struct custom_hash {
    size_t operator()(uint64_t x) const {
        static const uint64_t R = chrono::steady_clock::now().time_since_epoch().count();
        x += R + 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }
};

template <typename K, typename V>
using safe_map = gp_hash_table<K, V, custom_hash>;

template <typename K>
using safe_set = gp_hash_table<K, null_type, custom_hash>;

// 3. 可并配对堆 (默认大根堆)
template <typename T, typename Compare = less<T>>
using heap = __gnu_pbds::priority_queue<T, Compare, pairing_heap_tag>;

/*
基础用法示范:
    ordered_set<int> tr;
    tr.insert(10); tr.insert(20);
    int rk = tr.order_of_key(20);   // 1 (严格小于 20 的个数)
    int v = *tr.find_by_order(0);   // 10 (0-indexed 第 0 小)
    tr.erase(tr.lower_bound(20));

    safe_map<ll, int> mp;
    mp[123456789LL] = 1;

    heap<int> h1, h2;
    auto it = h1.push(10); h1.modify(it, 30);
    h2.push(40); h1.join(h2);       // h1.top() == 40
*/
```
