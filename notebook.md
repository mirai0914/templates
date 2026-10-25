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

- [1. 欧拉线性筛 (素数 + 积性函数 phi & mu)](#1-欧拉线性筛-素数--积性函数-phi--mu)
- [2. 快速数论变换 (NTT 多项式乘法 / 快速幂 / 分治连乘)](#2-快速数论变换-ntt-多项式乘法--快速幂--分治连乘)
- [3. 分拆数 (五边形数定理 + 根号分治 DP)](#3-分拆数-五边形数定理--根号分治-dp)
- [4. 斯特林数 (O(N^2) 递推 + NTT 单行求解)](#4-斯特林数-on2-递推--ntt-单行求解)
- [5. 珂朵莉树 (ODT - Old Driver Tree / Chtholly Tree)](#5-珂朵莉树-odt---old-driver-tree--chtholly-tree)

---

## 1. 欧拉线性筛 (素数 + 积性函数 phi & mu)

> **公式备忘**:  
> - $\phi(p) = p - 1$；若 $i \pmod p == 0$ 则 $\phi(i \cdot p) = \phi(i) \cdot p$；否则 $\phi(i \cdot p) = \phi(i) \cdot (p - 1)$  
> - $\mu(1) = 1, \mu(p) = -1$；若 $i \pmod p == 0$ 则 $\mu(i \cdot p) = 0$；否则 $\mu(i \cdot p) = -\mu(i)$

```cpp
#include <bits/stdc++.h>
using namespace std;

// 模块 1: 基础欧拉筛 (素数)
struct EulerSieve {
    int n = 0;
    vector<int> prm;
    vector<bool> isp; // isp[x] == false 表示 x 为质数

    EulerSieve(int max_n = 0) { if (max_n > 0) build(max_n); }
    void build(int max_n) {
        n = max_n; prm.clear(); isp.assign(n + 1, false);
        if (n >= 0) isp[0] = true;
        if (n >= 1) isp[1] = true;
        for (int i = 2; i <= n; i++) {
            if (!isp[i]) prm.push_back(i);
            for (int p : prm) {
                if (1LL * i * p > n) break;
                isp[i * p] = true;
                if (i % p == 0) break;
            }
        }
    }
    bool is_prime(int x) const { return x >= 2 && x <= n && !isp[x]; }
};

// 模块 2: 扩展欧拉筛 (素数 + 欧拉函数 phi + 莫比乌斯函数 mu)
struct MultiplicativeSieve {
    int n = 0;
    vector<int> prm, phi, mu;
    vector<bool> isp;

    MultiplicativeSieve(int max_n = 0) { if (max_n > 0) build(max_n); }
    void build(int max_n) {
        n = max_n; prm.clear();
        isp.assign(n + 1, false);
        phi.assign(n + 1, 0); mu.assign(n + 1, 0);
        if (n >= 0) isp[0] = true;
        if (n >= 1) { isp[1] = true; phi[1] = 1; mu[1] = 1; }
        for (int i = 2; i <= n; i++) {
            if (!isp[i]) {
                prm.push_back(i);
                phi[i] = i - 1; mu[i] = -1;
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
};
```

---

## 2. 快速数论变换 (NTT 多项式乘法 / 快速幂 / 分治连乘)

> **特性**: 模数 $MOD = 998244353$，原根 $G = 3$  
> - `mult`: $\mathcal{O}((N + M) \log(N + M))$  
> - `poly_pow`: $\mathcal{O}(L \log L \log k)$  
> - `poly_prod`: $\mathcal{O}(N \log^2 N)$

```cpp
#include <bits/stdc++.h>
using namespace std;

struct NTT {
    static constexpr int MOD = 998244353;
    const int G = 3;

    long long ksm(long long a, long long b) const {
        long long res = 1; a %= MOD;
        for (; b; b >>= 1, a = a * a % MOD) if (b & 1) res = res * a % MOD;
        return res;
    }
    long long inv(long long a) const { return ksm(a, MOD - 2); }

    void transf(vector<int>& a, bool inv_flag) const {
        int n = a.size(); if (n <= 1) return;
        vector<int> rev(n, 0);
        int bit = __builtin_ctz(n);
        for (int i = 0; i < n; ++i) {
            rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (bit - 1));
            if (i < rev[i]) swap(a[i], a[rev[i]]);
        }
        for (int len = 2; len <= n; len <<= 1) {
            long long wlen = ksm(G, (MOD - 1) / len);
            if (inv_flag) wlen = inv(wlen);
            int half = len >> 1;
            for (int i = 0; i < n; i += len) {
                long long w = 1;
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
            long long inv_n = inv(n);
            for (int i = 0; i < n; ++i) a[i] = 1LL * a[i] * inv_n % MOD;
        }
    }

    // 多项式乘法
    vector<int> mult(vector<int> a, vector<int> b) const {
        if (a.empty() || b.empty()) return {};
        int sz = a.size() + b.size() - 1, n = 1;
        while (n < sz) n <<= 1;
        a.resize(n, 0); b.resize(n, 0);
        transf(a, 0); transf(b, 0);
        for (int i = 0; i < n; ++i) a[i] = 1LL * a[i] * b[i] % MOD;
        transf(a, 1); a.resize(sz);
        return a;
    }

    // 截断多项式快速幂
    vector<int> poly_pow(vector<int> a, long long k, int lim = -1) const {
        vector<int> res = {1};
        while (k > 0) {
            if (k & 1) {
                res = mult(res, a);
                if (lim != -1 && (int)res.size() > lim + 1) res.resize(lim + 1);
            }
            a = mult(a, a);
            if (lim != -1 && (int)a.size() > lim + 1) a.resize(lim + 1);
            k >>= 1;
        }
        return res;
    }

    // 分治 NTT 求 n 个多项式连乘
    vector<int> poly_prod(const vector<vector<int>>& polys, int lim = -1) const {
        if (polys.empty()) return {1};
        auto dfs = [&](auto&& self, int l, int r) -> vector<int> {
            if (l == r) {
                auto res = polys[l];
                if (lim != -1 && (int)res.size() > lim + 1) res.resize(lim + 1);
                return res;
            }
            int mid = l + (r - l) / 2;
            auto left = self(self, l, mid);
            auto right = self(self, mid + 1, r);
            auto res = mult(left, right);
            if (lim != -1 && (int)res.size() > lim + 1) res.resize(lim + 1);
            return res;
        };
        return dfs(dfs, 0, (int)polys.size() - 1);
    }
};
```

---

## 3. 分拆数 (五边形数定理 + 根号分治 DP)

> **公式备忘**:  
> - 广义五边形数: $g_k = \frac{k(3k \mp 1)}{2}$，符号规律: `+ + - - + + - -`  
> - 根号分治: 阈值 $B = \lfloor\sqrt{n}\rfloor$。$\le B$ 完全背包；$> B$ 整体 $+1$ 与新增 $B+1$ 滚动 DP。

```cpp
#include <bits/stdc++.h>
using namespace std;

// 模块 1: 五边形数定理 (打表前 N 项, O(N sqrt(N)))
struct PartitionPentagonal {
    static constexpr int MOD = 998244353;
    int n = 0;
    vector<int> p;

    PartitionPentagonal(int max_n = 0) { if (max_n >= 0) build(max_n); }
    void build(int max_n) {
        n = max_n; p.assign(n + 1, 0); p[0] = 1;
        vector<pair<int, int>> pent;
        for (int k = 1;; ++k) {
            long long g1 = 1LL * k * (3LL * k - 1) / 2;
            long long g2 = 1LL * k * (3LL * k + 1) / 2;
            int sign = (k & 1) ? 1 : -1;
            if (g1 <= n) pent.emplace_back((int)g1, sign);
            if (g2 <= n) pent.emplace_back((int)g2, sign);
            if (g1 > n && g2 > n) break;
        }
        for (int i = 1; i <= n; ++i) {
            long long val = 0;
            for (const auto& [g, sign] : pent) {
                if (g > i) break;
                val += (sign == 1 ? p[i - g] : -p[i - g]);
            }
            val %= MOD; if (val < 0) val += MOD;
            p[i] = (int)val;
        }
    }
    int query(int x) const { return (x < 0 || x > n) ? 0 : p[x]; }
};

// 模块 2: 根号分治 DP (单点求解与带限制分拆模型, O(N sqrt(N)))
struct PartitionSqrt {
    static constexpr int MOD = 998244353;
    int calc(int n) const {
        if (n < 0) return 0;
        if (n <= 1) return 1;
        int B = sqrt(n);
        vector<int> f(n + 1, 0); f[0] = 1;
        for (int v = 1; v <= B; ++v)
            for (int j = v; j <= n; ++j) {
                f[j] += f[j - v];
                if (f[j] >= MOD) f[j] -= MOD;
            }
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
        long long ans = 0;
        for (int k = 0; k <= n; ++k)
            ans = (ans + 1LL * f[k] * h[n - k]) % MOD;
        return (int)ans;
    }
};
```

---

## 4. 斯特林数 (O(N^2) 递推 + NTT 单行求解)

> **公式备忘**:  
> - 普通幂转下降幂: $x^n = \sum_{k=0}^n \left\{\begin{matrix} n \\ k \end{matrix}\right\} x^{\underline{k}}$  
> - 上升幂转普通幂: $x^{\overline{n}} = \sum_{k=0}^n \left[\begin{matrix} n \\ k \end{matrix}\right] x^k = \prod_{i=0}^{n-1} (x + i)$  
> - 下降幂转普通幂: $x^{\underline{n}} = \sum_{k=0}^n (-1)^{n-k} \left[\begin{matrix} n \\ k \end{matrix}\right] x^k$  
> - 卷积容斥: $\left\{\begin{matrix} n \\ k \end{matrix}\right\} = \sum_{j=0}^k \frac{(-1)^{k-j}}{(k-j)!} \frac{j^n}{j!}$

```cpp
#include <bits/stdc++.h>
using namespace std;

// 模块 1: O(N^2) 斯特林数打表 (适合 N <= 3000 ~ 5000)
struct StirlingTable {
    static constexpr int MOD = 998244353;
    int n = 0;
    vector<vector<int>> s1, s2;

    StirlingTable(int max_n = 0) { if (max_n >= 0) build(max_n); }
    void build(int max_n) {
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
    int query_s1(int x, int y) const { return (x < 0 || y < 0 || y > x || x > n) ? 0 : s1[x][y]; }
    int query_s2(int x, int y) const { return (x < 0 || y < 0 || y > x || x > n) ? 0 : s2[x][y]; }
};

// 模块 2: O(N log N) / O(N log^2 N) 快速单行求解 (适合 N <= 10^5)
struct StirlingRow {
    static constexpr int MOD = 998244353;

    // NTT 卷积求第二类斯特林数整行 O(N log N)
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

    // 分治 NTT 求第一类斯特林数整行 O(N log^2 N)
    template <typename NTTOp>
    vector<int> row_s1(int n, const NTTOp& ntt) const {
        if (n == 0) return {1};
        vector<vector<int>> polys(n);
        for (int i = 0; i < n; ++i) polys[i] = {i, 1}; // (x + i)
        return ntt.poly_prod(polys);
    }
};
```

---

## 5. 珂朵莉树 (ODT - Old Driver Tree / Chtholly Tree)

> **核心避坑准则**:  
> 所有区间操作涉及双端点 split 时，必须【**先 split(r + 1)，再 split(l)**】！否则先获得的左端点迭代器可能在右端点 split 执行 erase 时失效导致 RE！  
> 结构: `set<array<int, 3>>`，每个节点为 `{l, r, v}`。在含随机区间推平 assign 的数据下，块数期望为 $\mathcal{O}(\log \log n)$。

```cpp
#include <bits/stdc++.h>
using namespace std;

struct ODT {
    set<array<int, 3>> s; // {l, r, v}

    ODT() {}
    ODT(int n, int dft_v = 0) { build(n, dft_v); }
    void clear() { s.clear(); }
    void build(int n, int dft_v = 0) {
        s.clear(); s.insert({1, n, dft_v});
    }
    void build(const vector<int>& a) {
        s.clear(); int n = a.size() - 1;
        for (int i = 1; i <= n; ++i) s.insert({i, i, a[i]});
    }

    // 切分包含 x 的区间为 [l, x-1] 与 [x, r]
    auto split(int x) {
        auto it = prev(s.lower_bound({x + 1, 0, 0}));
        if ((*it)[0] == x) return it;
        auto [l, r, v] = *it;
        s.erase(it);
        s.insert({l, x - 1, v});
        return s.insert({x, r, v}).first;
    }

    // 区间推平赋值 [l, r] = v (必须先 r+1 再 l)
    void assign(int l, int r, int v) {
        auto itr = split(r + 1), itl = split(l);
        s.erase(itl, itr);
        s.insert({l, r, v});
    }

    // 区间加 [l, r] += v
    void add(int l, int r, int v) {
        auto itr = split(r + 1), itl = split(l);
        vector<array<int, 3>> tmp;
        for (auto it = itl; it != itr; ++it)
            tmp.push_back({(*it)[0], (*it)[1], (*it)[2] + v});
        s.erase(itl, itr);
        for (auto& node : tmp) s.insert(node);
    }

    // 区间第 k 小 (1-indexed)
    int kth(int l, int r, int k) {
        auto itr = split(r + 1), itl = split(l);
        vector<pair<int, int>> a;
        for (auto it = itl; it != itr; ++it)
            a.emplace_back((*it)[2], (*it)[1] - (*it)[0] + 1);
        sort(a.begin(), a.end());
        for (const auto& [val, len] : a) {
            k -= len;
            if (k <= 0) return val;
        }
        return -1;
    }

    // 区间 p 次方和 sum_{i=l}^r (a[i]^p) % mod
    int sum_pow(int l, int r, int p, int mod) {
        auto itr = split(r + 1), itl = split(l);
        auto ksm = [&](long long base, int exp) -> long long {
            long long res = 1 % mod; base %= mod;
            while (exp > 0) {
                if (exp & 1) res = (__int128_t)res * base % mod;
                base = (__int128_t)base * base % mod; exp >>= 1;
            }
            return res;
        };
        long long ans = 0;
        for (auto it = itl; it != itr; ++it) {
            long long len = (*it)[1] - (*it)[0] + 1;
            ans = (ans + len * ksm((*it)[2], p)) % mod;
        }
        return (int)ans;
    }
};
```
