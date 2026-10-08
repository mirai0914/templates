#pragma once
#include <vector>
#include <algorithm>

/**
 * @brief 快速选择算法 (Quick Select)
 * @details 复杂度: 期望 O(N)，用于在未完全排序数组中寻找第 k 小元素 (0-indexed)
 * @note 实际竞赛中推荐优先使用 STL 经过高度优化的 std::nth_element:
 *       std::nth_element(a.begin(), a.begin() + k, a.end());
 *       结果存放在 a[k]
 */
template <typename T>
T quick_select(std::vector<T>& a, int l, int r, int k) {
    if (l >= r) return a[l];

    int i = l - 1, j = r + 1;
    T pivot = a[l + (r - l) / 2];

    while (i < j) {
        do { i++; } while (a[i] < pivot);
        do { j--; } while (a[j] > pivot);
        if (i < j) std::swap(a[i], a[j]);
    }

    if (k <= j) return quick_select(a, l, j, k);
    return quick_select(a, j + 1, r, k);
}
