#pragma once
#include <vector>
#include <functional>
#include <algorithm>

/**
 * @brief 稀疏表 (ST Table)
 * @details 复杂度: 预处理 O(N log N), 查询 O(1)
 *          支持任意结合律与可重复贡献运算 (默认 max)
 *          结构体自闭环封装，多测安全
 */
template <typename T, typename Op = std::function<T(const T&, const T&)>>
struct STTable {
    int n = 0;
    std::vector<int> lg;
    std::vector<std::vector<T>> st;
    Op op;

    STTable(Op operation = [](const T& a, const T& b) { return std::max(a, b); })
        : op(operation) {}

    void build(const std::vector<T>& a) {
        n = static_cast<int>(a.size());
        if (n <= 0) {
            st.clear();
            lg.clear();
            return;
        }

        lg.assign(n + 1, 0);
        for (int i = 2; i <= n; i++) {
            lg[i] = lg[i >> 1] + 1;
        }

        int k = lg[n];
        st.assign(k + 1, std::vector<T>(n));
        st[0] = a;

        for (int j = 1; j <= k; j++) {
            int step = 1 << (j - 1);
            for (int i = 0; i + (1 << j) <= n; i++) {
                st[j][i] = op(st[j - 1][i], st[j - 1][i + step]);
            }
        }
    }

    // 查询闭区间 [l, r] (0-indexed)
    T query(int l, int r) const {
        int len = r - l + 1;
        int k = lg[len];
        return op(st[k][l], st[k][r - (1 << k) + 1]);
    }
};
