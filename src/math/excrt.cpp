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
