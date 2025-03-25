#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace std::chrono;

void sieve_openmp(int n) {
    vector<bool> is_prime(n + 1, true);
    is_prime[0] = is_prime[1] = false;

    for (int p = 2; p * p <= n; ++p) {
        if (is_prime[p]) {
            #pragma omp parallel for schedule(static)
            for (int multiple = p * p; multiple <= n; multiple += p)
                is_prime[multiple] = false;
        }
    }
}

int main() {
    int threads = omp_get_max_threads();
    if (const char* env = getenv("OMP_NUM_THREADS"))
        threads = atoi(env);
    cout << "Running OpenMP with " << threads << " threads\n";

    vector<int> values = {100000, 1000000, 10000000, 100000000};

    for (int n : values) {
        cout << "n = " << n << "..." << endl;
        auto start = high_resolution_clock::now();
        sieve_openmp(n);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        cout << "Execution time: " << duration.count() << " ms\n\n";
    }

    return 0;
}
