#pragma once
#include <vector>

/**
 * @brief 最小质因子 (SPF) 与快速质因数分解
 * @details 预处理 O(N)，单次质因数分解 O(log x)
 */
struct SPF {
    int n = 0;
    std::vector<int> min_p;
    std::vector<int> primes;

    SPF(int max_val = 0) {
        if (max_val > 0) build(max_val);
    }

    void build(int max_val) {
        n = max_val;
        min_p.assign(n + 1, 0);
        primes.clear();

        if (n >= 1) min_p[1] = 1;

        for (int i = 2; i <= n; i++) {
            if (min_p[i] == 0) {
                min_p[i] = i;
                primes.push_back(i);
            }
            for (int p : primes) {
                if (i * p > n) break;
                min_p[i * p] = p;
                if (i % p == 0) break;
            }
        }
    }

    // 分解质因数: 返回各质因子 (包含重数)，如 12 -> {2, 2, 3}
    std::vector<int> get_factors(int x) const {
        std::vector<int> res;
        while (x > 1) {
            int p = min_p[x];
            res.push_back(p);
            x /= p;
        }
        return res;
    }

    // 分解质因数: 返回 {质数, 指数} 形式，如 12 -> {{2, 2}, {3, 1}}
    std::vector<std::pair<int, int>> get_factor_pairs(int x) const {
        std::vector<std::pair<int, int>> res;
        while (x > 1) {
            int p = min_p[x];
            int cnt = 0;
            while (x % p == 0) {
                cnt++;
                x /= p;
            }
            res.emplace_back(p, cnt);
        }
        return res;
    }
};
