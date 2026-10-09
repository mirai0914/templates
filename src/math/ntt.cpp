#pragma once
#include <vector>
#include <algorithm>

/**
 * @brief 快速数论变换 (NTT)
 * @details 模数固定 998244353，原根 G = 3
 */
struct NTT 
{
    static constexpr int MOD = 998244353;
    const int G = 3;

    // 功能: 快速幂 a^b % MOD
    // 传参: a 底数, b 指数
    long long ksm(long long a, long long b) const {
        long long res = 1;
        a %= MOD;
        for (; b; b >>= 1, a = a * a % MOD) {
            if (b & 1) res = res * a % MOD;
        }
        return res;
    }

    // 功能: 费马小定理求逆元
    // 传参: a 待求逆元的整数
    long long inv(long long a) const {
        return ksm(a, MOD - 2);
    }

    // 功能: NTT / INTT 核心变换 (系数 <-> 点值)
    // 传参: a 系数数组 (长度需为 2^k), inv_flag 0: 正变换 / 1: 逆变换
    void transf(std::vector<int>& a, bool inv_flag) const {
        int n = a.size();
        if (n <= 1) return;

        std::vector<int> rev(n, 0);
        int bit = __builtin_ctz(n);

        for (int i = 0; i < n; ++i) {
            rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (bit - 1));
            if (i < rev[i]) std::swap(a[i], a[rev[i]]);
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
            for (int i = 0; i < n; ++i) {
                a[i] = 1LL * a[i] * inv_n % MOD;
            }
        }
    }

    // 功能: 多项式乘法 (卷积) C(x) = A(x) * B(x)
    // 传参: a, b 多项式系数向量 (下标 i 为 x^i 项系数)
    // 返回: 卷积后的系数向量 (size = deg(A) + deg(B) + 1)
    std::vector<int> mult(std::vector<int> a, std::vector<int> b) const {
        if (a.empty() || b.empty()) return {};
        int sz = a.size() + b.size() - 1;
        int n = 1;
        while (n < sz) n <<= 1;

        a.resize(n, 0);
        b.resize(n, 0);

        transf(a, 0);
        transf(b, 0);
        for (int i = 0; i < n; ++i) {
            a[i] = 1LL * a[i] * b[i] % MOD;
        }
        transf(a, 1);

        a.resize(sz);
        return a;
    }

    // 功能: 多项式快速幂 A^k(x)
    // 传参: a 底数多项式系数, k 幂次, lim 最高次数限制 (截断到 <= lim 项, 默认 -1 不截断)
    // 返回: 结果多项式系数向量
    std::vector<int> poly_pow(std::vector<int> a, long long k, int lim = -1) const {
        std::vector<int> res = {1};
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
};

/*
使用示范:
    NTT ntt;

    // 1. 多项式乘法: (1 + 2x) * (3 + 4x) = 3 + 10x + 8x^2
    std::vector<int> a = {1, 2}, b = {3, 4};
    std::vector<int> c = ntt.mult(a, b); // c 为 {3, 10, 8}

    // 2. 多项式快速幂 (带截断): (1 + x)^3 截断到最高 2 次项
    std::vector<int> p = ntt.poly_pow({1, 1}, 3, 2); // p 为 {1, 3, 3} (即 1 + 3x + 3x^2)
*/
