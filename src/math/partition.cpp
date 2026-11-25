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
