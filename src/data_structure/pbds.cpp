#pragma once
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/pb_ds/hash_policy.hpp>
#include <ext/pb_ds/priority_queue.hpp>

using namespace std;
using namespace __gnu_pbds;

using ll = long long;

/**
 * PBDS (GCC 扩展库常用容器: 名次树 / 防卡哈希 / 可并配对堆)
 * 注意:
 *   1. order_of_key(x): 严格小于 x 的个数; find_by_order(k): 0-indexed 第 k 小迭代器
 *   2. 多重集严禁写 less_equal<T> (会导致 lower_bound/erase 异常), 存 array<T, 2>{val, ++uid}
 *   3. h1.join(h2): O(1) 合并可并堆, h2 会被置空; modify/erase 支持 point_iterator
 */

// 1. 平衡树 (Order Statistics Tree)
template <typename T>
using ordered_set = tree<
    T,
    null_type,
    less<T>,
    rb_tree_tag,
    tree_order_statistics_node_update
>;

// 2. 防卡哈希表 (gp_hash_table + splitmix64 运行期随机盐)
struct custom_hash {
    size_t operator()(uint64_t x) const {
        static const uint64_t R = chrono::steady_clock::now().time_since_epoch().count();
        x += R + 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }
};

template <typename K, typename V>
using safe_map = gp_hash_table<K, V, custom_hash>;

template <typename K>
using safe_set = gp_hash_table<K, null_type, custom_hash>;

// 3. 可并配对堆 (默认大根堆)
template <typename T, typename Compare = less<T>>
using heap = __gnu_pbds::priority_queue<T, Compare, pairing_heap_tag>;

/*
基础用法示范:
    ordered_set<int> tr;
    tr.insert(10); tr.insert(20);
    int rk = tr.order_of_key(20);   // 1 (严格小于 20 的个数)
    int v = *tr.find_by_order(0);   // 10 (0-indexed 第 0 小)
    tr.erase(tr.lower_bound(20));

    safe_map<ll, int> mp;
    mp[123456789LL] = 1;

    heap<int> h1, h2;
    auto it = h1.push(10); h1.modify(it, 30);
    h2.push(40); h1.join(h2);       // h1.top() == 40
*/
