#pragma once
#include <bits/stdc++.h>
using namespace std;

/**
 * =========================================================================
 * 斯特林数 (Stirling Numbers)
 * =========================================================================
 * 常用恒等式备忘 (赛场速查):
 * 1. 普通幂转下降幂: x^n = sum_{k=0}^n {n, k} * x^{underline k}
 * 2. 上升幂转普通幂: x^{overline n} = sum_{k=0}^n [n, k] * x^k
 * 3. 下降幂转普通幂: x^{underline n} = sum_{k=0}^n (-1)^{n-k} [n, k] * x^k
 * 4. 斯特林反演:
 *    f(n) = sum_{k=0}^n {n, k} g(k) <=> g(n) = sum_{k=0}^n (-1)^{n-k} [n, k] f(k)
 */

/**
 * 模块 1: O(N^2) 斯特林数打表 (适合 N <= 3000 ~ 5000)
 * 提供无符号第一类 [n, k] 与第二类 {n, k}
 */
struct StirlingTable {
    static constexpr int MOD = 998244353;
    int n = 0;
    vector<vector<int>> s1; // 第一类斯特林数 (轮换排列数)
    vector<vector<int>> s2; // 第二类斯特林数 (非空子集划分数)

    StirlingTable(int max_n = 0) {
        if (max_n >= 0) build(max_n);
    }

    // 功能: 预处理 0 ~ max_n 的第一类与第二类斯特林数
    // 传参: max_n 预处理规模
    void build(int max_n) {
        n = max_n;
        s1.assign(n + 1, vector<int>(n + 1, 0));
        s2.assign(n + 1, vector<int>(n + 1, 0));

        s1[0][0] = s2[0][0] = 1;
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= i; ++j) {
                // s1[i][j] = s1[i-1][j-1] + (i-1) * s1[i-1][j]
                s1[i][j] = (s1[i - 1][j - 1] + 1LL * (i - 1) * s1[i - 1][j]) % MOD;
                // s2[i][j] = s2[i-1][j-1] + j * s2[i-1][j]
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

/**
 * 模块 2: O(N log N) / O(N log^2 N) 快速单行求解 (适合 N <= 10^5)
 * 依赖 NTT 卷积与分治
 */
struct StirlingRow {
    static constexpr int MOD = 998244353;

    // 功能: NTT 卷积求解第二类斯特林数第 n 行 {n, 0 ... n}
    // 传参: n 行号, ntt NTT 实例
    // 返回: 系数向量 res, res[k] 为 {n, k} % MOD (复杂度 O(N log N))
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

    // 功能: 分治 NTT 求解第一类斯特林数第 n 行 [n, 0 ... n]
    // 传参: n 行号, ntt NTT 实例
    // 返回: 系数向量 res, res[k] 为 [n, k] % MOD (复杂度 O(N log^2 N))
    template <typename NTTOp>
    vector<int> row_s1(int n, const NTTOp& ntt) const {
        if (n == 0) return {1};
        vector<vector<int>> polys(n);
        for (int i = 0; i < n; ++i) {
            polys[i] = {i, 1}; // 生成多项式 (x + i)
        }
        return ntt.poly_prod(polys);
    }
};

/*
使用示范:
    // 1. 小数据打表 O(N^2):
    StirlingTable st(2000);
    int val_s1 = st.query_s1(4, 2); // [4, 2] = 11
    int val_s2 = st.query_s2(4, 2); // {4, 2] = 7

    // 2. 大数据单行求解 O(N log N):
    NTT ntt;
    StirlingRow sr;
    vector<int> r_s2 = sr.row_s2(100000, ntt); // {100000, 0 ... 100000}
    vector<int> r_s1 = sr.row_s1(100000, ntt); // [100000, 0 ... 100000]
*/
