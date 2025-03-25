#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>

using namespace std;

vector<int> generate_small_primes(int limit) {
    vector<bool> is_prime(limit + 1, true);
    is_prime[0] = is_prime[1] = false;
    for (int p = 2; p * p <= limit; ++p) {
        if (is_prime[p]) {
            for (int multiple = p * p; multiple <= limit; multiple += p)
                is_prime[multiple] = false;
        }
    }

    vector<int> primes;
    for (int i = 2; i <= limit; ++i)
        if (is_prime[i]) primes.push_back(i);

    return primes;
}

int count_primes_in_range(int low, int high, const vector<int>& small_primes) {
    int size = high - low + 1;
    vector<bool> is_prime(size, true);

    for (int p : small_primes) {
        int start = max(p * p, ((low + p - 1) / p) * p);
        for (int multiple = start; multiple <= high; multiple += p)
            is_prime[multiple - low] = false;
    }

    int count = 0;
    for (int i = 0; i < size; ++i)
        if (is_prime[i] && (low + i >= 2)) ++count;

    return count;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector<int> values = {100000, 1000000, 10000000, 100000000};

    for (int n : values) {
        if (rank == 0)
            cout << "n = " << n << "..." << endl;

        MPI_Barrier(MPI_COMM_WORLD);
        double start = MPI_Wtime();

        int root = static_cast<int>(sqrt(n));
        vector<int> small_primes;

        if (rank == 0)
            small_primes = generate_small_primes(root);

        int small_size;
        if (rank == 0) small_size = small_primes.size();
        MPI_Bcast(&small_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (rank != 0) small_primes.resize(small_size);
        MPI_Bcast(small_primes.data(), small_size, MPI_INT, 0, MPI_COMM_WORLD);

        int range_size = (n - root) / size;
        int low = root + 1 + rank * range_size;
        int high = (rank == size - 1) ? n : low + range_size - 1;

        int local_count = count_primes_in_range(low, high, small_primes);
        if (rank == 0)
            local_count += small_primes.size();

        int global_count;
        MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        double end = MPI_Wtime();

        if (rank == 0) {
            cout << "Total primes up to " << n << ": " << global_count << endl;
            cout << "Execution time: " << (end - start) * 1000 << " ms\n\n";
        }
    }

    MPI_Finalize();
    return 0;
}
