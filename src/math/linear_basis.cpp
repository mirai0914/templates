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
