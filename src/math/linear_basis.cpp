#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/**
 * =========================================================================
 * 线性基 (Linear Basis / XOR Basis)
 * =========================================================================
 * 适用场景:
 *   1. 维护异或空间基底，O(B) 动态插入、查询异或最大值/最小值/存在性
 *   2. 高斯消元规范化后 O(B) 查询张成空间的第 k 小异或和
 *   3. 两个线性基 O(B^2) 合并
 *   4. 可选扩展: 前缀线性基 (Positional / Prefix Basis) 在线维护区间 [L, R] 异或最大值
 * 
 * 核心性质备忘 (赛场速查):
 *   1. 向量空间: 若非空基大小为 r，则能异或张成 2^r 种不同的值。
 *   2. 第 k 小查询 (1-indexed):
 *      若插入过程中有元素被异或为 0 (或原数组含 0)，说明 0 可以被表示 (此时 0 为第 1 小，k 需减 1)。
 *      规范化 (RREF) 后每一位主元互不影响，按 k 的二进制拆分贪心拼接即可。
 *   3. 区间查询 (前缀线性基):
 *      维护 pos[i] 表示基向量来自原数组的最大下标；贪心将越靠右的元素保留在基中。
 */
struct LinearBasis {
    int B = 60;             // 最高有效位数 (默认支持 0 ~ 2^60-1)
    vector<ll> d, p;           // 基向量数组: d[i] 的最高位必为第 i 位 / 规范化后的基向量 (用于求第 k 小)
    int cnt = 0;            // 线性基当前的大小 (秩)
    bool has_zero = 0, rebuilt = 0;  // 张成空间是否包含 0 / 是否已规范化

    LinearBasis(int b = 60) : B(b), d(b, 0) {}

    // 功能: 重置清空线性基 (多测安全)
    void clear() {
        d.assign(B, 0);
        p.clear();
        cnt = 0;
        has_zero = false;
        rebuilt = false;
    }

    // 功能: 插入一个元素 x
    // 传参: x 待插入的正整数
    // 返回: 是否成功插入 (与已有基线性无关则返回 true)
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
    // 传参: x 目标整数
    // 返回: 若能表出返回 true
    bool check(ll x) const {
        for (int i = B - 1; i >= 0; --i) {
            if ((x >> i) & 1) {
                if (!d[i]) return false;
                x ^= d[i];
            }
        }
        return true;
    }

    // 功能: 查询当前异或空间与 base 异或的最大值
    // 传参: base 基础初值 (默认 0，即查空间最大值)
    // 返回: 最大异或和
    ll query_max(ll base = 0) const {
        ll ans = base;
        for (int i = B - 1; i >= 0; --i) {
            if ((ans ^ d[i]) > ans) {
                ans ^= d[i];
            }
        }
        return ans;
    }

    // 功能: 查询张成空间中的最小非零异或和
    // 传参: allow_zero 是否允许返回 0 (若原集合线性相关且允许 0 则返回 0)
    // 返回: 最小值
    ll query_min(bool allow_zero = false) const {
        if (allow_zero && has_zero) return 0;
        for (int i = 0; i < B; ++i) {
            if (d[i]) return d[i];
        }
        return 0;
    }

    // 功能: 行化简规范化 (高斯消元使每个基向量的主元位在其它向量中为 0)
    void rebuild() {
        p.clear();
        for (int i = 0; i < B; ++i) {
            if (d[i]) {
                for (int j = i + 1; j < B; ++j) {
                    if ((d[j] >> i) & 1) {
                        d[j] ^= d[i];
                    }
                }
            }
        }
        for (int i = 0; i < B; ++i) {
            if (d[i]) p.push_back(d[i]);
        }
        rebuilt = true;
    }

    // 功能: 查询非空子集异或和中的第 k 小值 (1-indexed)
    // 传参: k 排名 (若原集合含 0 或线性相关，则 0 占位 k=1)
    // 返回: 第 k 小异或值；若 k 超出范围返回 -1
    ll query_kth(ll k) {
        if (!rebuilt) rebuild();
        if (has_zero) --k;
        if (k == 0) return 0;
        if (p.size() < 62 && k >= (1ULL << p.size())) return -1;
        ll ans = 0;
        for (int i = 0; i < (int)p.size(); ++i) {
            if ((k >> i) & 1) {
                ans ^= p[i];
            }
        }
        return ans;
    }

    // 功能: 将另一个线性基 other 合并入当前线性基
    // 传参: other 另一个线性基
    void merge(const LinearBasis& other) {
        for (int i = other.B - 1; i >= 0; --i) {
            if (other.d[i]) insert(other.d[i]);
        }
        if (other.has_zero) has_zero = true;
    }
};

// =============================================================================
// 可选扩展: 前缀线性基 (区间异或最大值查询)
// =============================================================================
struct PrefixLinearBasis {
    int B = 60;
    vector<ll> d;
    vector<int> pos; // pos[i] 表示基向量 d[i] 对应的原数组下标 (尽量靠右)

    PrefixLinearBasis(int b = 60) : B(b), d(b, 0), pos(b, 0) {}

    // 功能: 清空当前基
    void clear() {
        d.assign(B, 0);
        pos.assign(B, 0);
    }

    // 功能: 插入下标为 id 的元素 x (需在上一前缀基的基础上插入)
    // 传参: x 待插入的数值, id 元素在原数组中的下标 (1-indexed)
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

    // 功能: 查询区间 [l, r] 的子集异或最大值 (当前结构体应为 pre[r])
    // 传参: l 区间左端点, base 初始基底
    // 返回: [l, r] 内子集最大异或和
    ll query_max(int l, ll base = 0) const {
        ll ans = base;
        for (int i = B - 1; i >= 0; --i) {
            if (pos[i] >= l && (ans ^ d[i]) > ans) {
                ans ^= d[i];
            }
        }
        return ans;
    }
};

/*
使用示范:
    // 1. 基础线性基:
    LinearBasis lb(60);
    for (ll x : a) lb.insert(x);
    ll max_xor = lb.query_max();          // 最大异或和
    ll kth = lb.query_kth(k);              // 第 k 小异或和
    bool can = lb.check(val);              // 能否异或出 val

    // 2. 前缀线性基 (静态区间 [l, r] 异或最大值):
    vector<PrefixLinearBasis> pre(n + 1, PrefixLinearBasis(60));
    for (int i = 1; i <= n; ++i) {
        pre[i] = pre[i - 1];
        pre[i].insert(a[i], i);
    }
    ll ans = pre[r].query_max(l);          // O(B) 查询区间 [l, r] 最大异或和
*/
