#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using i128 = __int128_t;

/**
 * =========================================================================
 * 欧拉降幂 (扩展欧拉定理) 与单点大数欧拉函数
 * =========================================================================
 * 适用场景:
 *   1. O(sqrt(N)) 试除法单点求大数欧拉函数 phi(n)
 *   2. 扩展欧拉定理求 a^b % m (指数 b 为长字符串或极大整数)
 *   3. 可选扩展: 递归求解超高幂塔 a_1 ^ a_2 ^ ... ^ a_k % m
 * 
 * 核心公式备忘 (赛场速查):
 *   1. 欧拉函数: phi(n) = n * prod_{p | n} (p - 1) / p
 *   2. 扩展欧拉定理 (无需满足 gcd(a, m) = 1):
 *      a^b = a^b % m                       (b < phi(m))
 *      a^b = a^( (b % phi(m)) + phi(m) ) % m  (b >= phi(m))
 */
struct EulerPower {
    // 功能: O(sqrt(n)) 试除法求单个大数的欧拉函数 phi(n)
    // 传参: n 目标正整数 (支持至 1e14~1e18)
    // 返回: phi(n)
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

    // 功能: 快速幂 a^b % m (防溢出采用 i128)
    ll ksm(ll a, ll b, ll m) const {
        ll res = 1 % m;
        a %= m;
        while (b > 0) {
            if (b & 1) res = (i128)res * a % m;
            a = (i128)a * a % m;
            b >>= 1;
        }
        return res;
    }

    // 功能: 扩展欧拉定理求 a^b % m (b 为大数字符串)
    // 传参: a 底数, b 指数字符串, m 模数
    // 返回: a^b % m
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
    // 传参: a 底数, b 整数指数, m 模数
    // 返回: a^b % m
    ll pow_num(ll a, ll b, ll m) const {
        if (m == 1) return 0;
        ll p = get_phi(m);
        if (b >= p) b = (b % p) + p;
        return ksm(a, b, m);
    }

    // =========================================================================
    // 可选扩展: 幂塔递归降幂 (计算 a[l] ^ a[l+1] ^ ... ^ a[r] % m)
    // =========================================================================
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

    // 功能: 递归求解区间幂塔 a[l] ^ a[l+1] ^ ... ^ a[r] % m
    // 返回: 降幂后的有效值 (最终结果对 m 取模即可)
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

    // 1. O(sqrt(n)) 单点求大数欧拉函数:
    ll phi_val = ep.get_phi(1000000007LL); // 1000000006

    // 2. 指数为大数字符串的欧拉降幂:
    // 计算 2 ^ (10^100) % 1e9+7
    ll ans1 = ep.pow_str(2, "1000000000000000000000000000000", 1000000007);

    // 3. 幂塔降幂: 2 ^ 3 ^ 2 % 1000 = 2 ^ 9 % 1000 = 512
    vector<ll> tower = {2, 3, 2};
    ll ans2 = ep.query_tower(tower, 0, 2, 1000); // 512
*/
