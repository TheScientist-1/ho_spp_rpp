#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

using namespace std;
using namespace std::chrono;

void sieve_serial(int n) {
    vector<bool> is_prime(n + 1, true);
    is_prime[0] = is_prime[1] = false;

    for (int p = 2; p * p <= n; ++p) {
        if (is_prime[p]) {
            for (int multiple = p * p; multiple <= n; multiple += p)
                is_prime[multiple] = false;
        }
    }

    // Uncomment for debug: print number of primes
    // int count = count_if(is_prime.begin(), is_prime.end(), [](bool x) { return x; });
    // cout << "Total primes: " << count << endl;
}

int main() {
    vector<int> values = {100000, 1000000, 10000000, 100000000};

    for (int n : values) {
        cout << "n = " << n << "..." << endl;
        auto start = high_resolution_clock::now();
        sieve_serial(n);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        cout << "Execution time: " << duration.count() << " ms\n\n";
    }

    return 0;
}
