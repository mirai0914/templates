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
