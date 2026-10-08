#pragma once
#include <vector>

/**
 * @brief 线性筛素数 (欧拉筛)
 * @details 复杂度: O(N) 预处理
 *          提供质数列表、素数判定表、最小质因子 min_p
 */
struct EulerSieve {
    int n = 0;
    std::vector<int> primes;
    std::vector<int> min_p; // min_p[x] 存 x 的最小质因子 (x=1 时为 1)
    std::vector<bool> is_prime;

    EulerSieve(int max_val = 0) {
        if (max_val > 0) build(max_val);
    }

    void build(int max_val) {
        n = max_val;
        primes.clear();
        min_p.assign(n + 1, 0);
        is_prime.assign(n + 1, true);

        if (n >= 0) is_prime[0] = false;
        if (n >= 1) {
            is_prime[1] = false;
            min_p[1] = 1;
        }

        for (int i = 2; i <= n; i++) {
            if (is_prime[i]) {
                primes.push_back(i);
                min_p[i] = i;
            }
            for (int p : primes) {
                if (i * p > n) break;
                is_prime[i * p] = false;
                min_p[i * p] = p;
                if (i % p == 0) break;
            }
        }
    }
};
