#pragma once
#include <bits/stdc++.h>
using namespace std;

/**
 * =========================================================================
 * 模块 1: 五边形数定理 (Pentagonal Number Theorem) 求分拆数
 * =========================================================================
 * 适用场景: 一次性预处理前 N 项分拆数 p(0 ... N)
 * 复杂度: 时间 O(N sqrt(N)), 空间 O(N)
 * 递推公式:
 *   p(n) = sum_{k >= 1} (-1)^{k-1} [ p(n - k(3k-1)/2) + p(n - k(3k+1)/2) ]
 *   符号规律: + + - - + + - - ... (奇正偶负)
 */
struct PartitionPentagonal {
    static constexpr int MOD = 998244353;
    int n = 0;
    vector<int> p; // p[i] 为整数 i 的分拆数

    PartitionPentagonal(int max_n = 0) {
        if (max_n >= 0) build(max_n);
    }

    // 功能: 预处理 0 ~ max_n 的分拆数
    // 传参: max_n 预处理上限
    void build(int max_n) {
        n = max_n;
        p.assign(n + 1, 0);
        p[0] = 1;

        // 预生成广义五边形数: k*(3k-1)/2 与 k*(3k+1)/2
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
                if (sign == 1) {
                    val += p[i - g];
                } else {
                    val -= p[i - g];
                }
            }
            val %= MOD;
            if (val < 0) val += MOD;
            p[i] = (int)val;
        }
    }

    // 功能: 查询 p(x)
    // 传参: x 目标整数
    // 返回: p(x) % MOD (越界返回 0)
    int query(int x) const {
        if (x < 0 || x > n) return 0;
        return p[x];
    }
};

/**
 * =========================================================================
 * 模块 2: 根号分治 DP 求分拆数
 * =========================================================================
 * 适用场景: 单点求 p(n) % MOD, 或作为带部件大小/数量限制的分拆数模型模板
 * 复杂度: 时间 O(N sqrt(N)), 空间 O(N)
 * 思路: 阈值 B = sqrt(n)
 *   1. 小数部分 (<= B): 完全背包
 *   2. 大数部分 (> B): 设状态 g[c][j] 为恰有 c 个 > B 的数和为 j
 *      转移: g[c][j] = g[c][j - c] (整体+1) + g[c - 1][j - (B + 1)] (新增一个 B+1)
 *   3. 合并: sum_{k=0}^n f[k] * h[n - k]
 */
struct PartitionSqrt {
    static constexpr int MOD = 998244353;

    // 功能: 单点计算整数 n 的分拆数 p(n) % MOD
    // 传参: n 目标正整数
    // 返回: p(n) % MOD
    int calc(int n) const {
        if (n < 0) return 0;
        if (n <= 1) return 1;

        int B = sqrt(n);

        // 1. 小数部分 (<= B) 完全背包
        vector<int> f(n + 1, 0);
        f[0] = 1;
        for (int v = 1; v <= B; ++v) {
            for (int j = v; j <= n; ++j) {
                f[j] += f[j - v];
                if (f[j] >= MOD) f[j] -= MOD;
            }
        }

        // 2. 大数部分 (> B) 滚动 DP: 最多有 n / (B + 1) 个数
        vector<int> prev(n + 1, 0), cur(n + 1, 0);
        prev[0] = 1;
        vector<int> h(n + 1, 0);
        h[0] = 1; // c = 0

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
        long long ans = 0;
        for (int k = 0; k <= n; ++k) {
            ans = (ans + 1LL * f[k] * h[n - k]) % MOD;
        }
        return (int)ans;
    }
};

/*
使用示范:
    // 1. 五边形数定理 (求前 N 项，如求 0 ~ 100000 的所有分拆数)
    PartitionPentagonal pp(100000);
    int p5 = pp.query(5); // p(5) = 7 (5, 4+1, 3+2, 3+1+1, 2+2+1, 2+1+1+1, 1+1+1+1+1)

    // 2. 根号分治 DP (单点计算 p(n) 或套用其大数加一/扩项 DP 思想)
    PartitionSqrt ps;
    int p100 = ps.calc(100); // 190569291 % 998244353
*/
