#pragma once
#include <bits/stdc++.h>
using namespace std;

/**
 * =========================================================================
 * 主席树 (可持久化权值线段树 - Persistent Segment Tree)
 * =========================================================================
 * 适用场景: 静态区间第 k 小 / 区间值域统计 / 树上路径第 k 小 (结合 LCA 差分)
 * 复杂度:
 *   - 空间复杂度: O(N log M) 动态开点 (M 为离散化后值域大小)
 *   - 时间复杂度: 单次单点插入 O(log M), 单次区间查询 O(log M), 整树构建 O(N log N)
 * 特性: 内置离散化与前缀版本链，纯血 vector 动态开点，多测自闭环绝对安全。
 */
struct PersistentSegTree {
    struct Node {
        int lc = 0, rc = 0, cnt = 0;
    };

    int n = 0, m = 0;
    vector<int> roots;
    vector<Node> tr;
    vector<int> vals; // 离散化去重原值表

    PersistentSegTree() {}
    PersistentSegTree(const vector<int>& a) {
        build(a);
    }

    // 功能: 在历史版本 p 基础上，在值域位置 pos 增加 1 生成新节点
    // 传参: p 历史版本节点编号, l, r 当前值域区间, pos 待插入值域下标 (1 <= pos <= m)
    // 返回: 新开节点编号
    int insert(int p, int l, int r, int pos) {
        int u = tr.size();
        tr.push_back(tr[p]);
        tr[u].cnt++;
        if (l == r) return u;
        int mid = l + (r - l) / 2;
        if (pos <= mid) {
            tr[u].lc = insert(tr[p].lc, l, mid, pos);
        } else {
            tr[u].rc = insert(tr[p].rc, mid + 1, r, pos);
        }
        return u;
    }

    // 功能: 在两版本 u (右) 与 v (左) 间做差，二分查找第 k 小位置 (1-indexed)
    // 传参: u 右端点根编号, v 左端点前驱根编号, l, r 当前值域, k 名次 (1 <= k <= cnt)
    // 返回: 离散化值域下标 (1 <= res <= m)
    int query(int u, int v, int l, int r, int k) const {
        if (l == r) return l;
        int left_cnt = tr[tr[u].lc].cnt - tr[tr[v].lc].cnt;
        int mid = l + (r - l) / 2;
        if (k <= left_cnt) {
            return query(tr[u].lc, tr[v].lc, l, mid, k);
        } else {
            return query(tr[u].rc, tr[v].rc, mid + 1, r, k - left_cnt);
        }
    }

    // 功能: 用原数组一键构建前缀主席树 (下标 1 ~ a.size()-1)
    // 传参: a 原始数组向量 (1-indexed)
    void build(const vector<int>& a) {
        n = a.size() - 1;
        roots.assign(n + 1, 0);
        tr.clear();
        tr.reserve(n * 20 + 5);
        tr.emplace_back(); // 0 号空节点 (lc=0, rc=0, cnt=0)

        // 离散化
        vals.assign(a.begin() + 1, a.end());
        sort(vals.begin(), vals.end());
        vals.erase(unique(vals.begin(), vals.end()), vals.end());
        m = vals.size();

        for (int i = 1; i <= n; ++i) {
            roots[i] = insert(roots[i - 1], get_pos(a[i]));
        }
    }

    // --- 一行小封装入口 ---
    int insert(int p, int pos) { return insert(p, 1, m, pos); }
    int get_pos(int x) const { return lower_bound(vals.begin(), vals.end(), x) - vals.begin() + 1; }
    int query_kth(int l, int r, int k) const { return vals[query(roots[r], roots[l - 1], 1, m, k) - 1]; }

    // =========================================================================
    // 可选扩展 1: 区间值域计数 / 二维数点 (Count / Rank)
    // =========================================================================
    // 功能: 查询区间 [l, r] 内值域在 [ql, qr] 范围的元素个数
    int query_count(int u, int v, int l, int r, int ql, int qr) const {
        if (ql > r || qr < l || ql > qr) return 0;
        if (ql <= l && r <= qr) return tr[u].cnt - tr[v].cnt;
        int mid = l + (r - l) / 2;
        return query_count(tr[u].lc, tr[v].lc, l, mid, ql, qr) +
               query_count(tr[u].rc, tr[v].rc, mid + 1, r, ql, qr);
    }
    // 一行小封装: 区间 <= x 的元素个数
    int count_le(int l, int r, int x) const {
        int qr = upper_bound(vals.begin(), vals.end(), x) - vals.begin();
        return query_count(roots[r], roots[l - 1], 1, m, 1, qr);
    }
    // 一行小封装: 区间值在 [x, y] 范围内的元素个数
    int count_range(int l, int r, int x, int y) const { return count_le(l, r, y) - count_le(l, r, x - 1); }

    // =========================================================================
    // 可选扩展 2: 树上路径第 k 小 (Tree Path Kth)
    // =========================================================================
    // 功能: 树上前缀差分 (u + v - lca - fa_lca) 二分第 k 小
    int query_tree(int u, int v, int lca, int fa_lca, int l, int r, int k) const {
        if (l == r) return l;
        int left_cnt = tr[tr[u].lc].cnt + tr[tr[v].lc].cnt - tr[tr[lca].lc].cnt - tr[tr[fa_lca].lc].cnt;
        int mid = l + (r - l) / 2;
        if (k <= left_cnt) return query_tree(tr[u].lc, tr[v].lc, tr[lca].lc, tr[fa_lca].lc, l, mid, k);
        else return query_tree(tr[u].rc, tr[v].rc, tr[lca].rc, tr[fa_lca].rc, mid + 1, r, k - left_cnt);
    }
    // 一行小封装: 树上路径第 k 小原值 (建树时 roots[u] = insert(roots[fa], get_pos(val[u])))
    int query_path_kth(int u, int v, int lca, int fa_lca, int k) const {
        return vals[query_tree(roots[u], roots[v], roots[lca], roots[fa_lca], 1, m, k) - 1];
    }
};

/*
使用示范:
    // a 必须是 1-indexed (a[0] 占位)
    vector<int> a = {0, 3, -1, 5, 2, 4, 2, 5, 10, -5};

    // 1. 初始化并构建前缀主席树
    PersistentSegTree pst(a);

    // 2. 一行小封装: 查询区间 [2, 6] 第 3 小的值
    int val = pst.query_kth(2, 6, 3);

    // 3. 可选扩展 1: 查询区间 [2, 6] 内 <= 4 的元素个数
    int cnt = pst.count_le(2, 6, 4);

    // 4. 可选扩展 2: 树上路径第 k 小 (需配合树剖/倍增求 LCA)
    // int ans = pst.query_path_kth(u, v, lca, fa[lca], k);
*/
