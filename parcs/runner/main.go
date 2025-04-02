package main

import (
	"fmt"
	"log"
	"math"
	"os"
	"sort"
	"strconv"
	"time"

	"github.com/lionell/parcs/go/parcs"
)

type Task struct {
	Low        int
	High       int
	BasePrimes []int
}

type Result struct {
	Primes []int
	Time   float64
}

const workerImage = "hayashi97m/parcs-lab-worker-go:latest"

type Runner struct {
	*parcs.Runner
}

func getInputs() (int, int) {
	nStr := os.Getenv("N")
	n, err := strconv.Atoi(nStr)
	if err != nil {
		panic(err)
	}
	wStr := os.Getenv("WORKERS")
	w, err := strconv.Atoi(wStr)
	if err != nil {
		panic(err)
	}
	return n, w
}

func basePrimes(limit int) []int {
	isPrime := make([]bool, limit+1)
	for i := 2; i <= limit; i++ {
		isPrime[i] = true
	}
	for i := 2; i*i <= limit; i++ {
		if isPrime[i] {
			for j := i * i; j <= limit; j += i {
				isPrime[j] = false
			}
		}
	}
	primes := []int{}
	for i := 2; i <= limit; i++ {
		if isPrime[i] {
			primes = append(primes, i)
		}
	}
	return primes
}

func (r *Runner) Run() {
	fmt.Println("Welcome to PARCS-Go!")
	fmt.Println("Running your program...")

	n, workers := getInputs()
	fmt.Printf("Received n=%d, workers=%d\n", n, workers)

	start := time.Now()

	base := basePrimes(int(math.Sqrt(float64(n))))
	fmt.Printf("Base primes (up to √n): %d primes\n", len(base))

	chunkSize := (n - 1) / workers
	tasks := make([]*parcs.Task, workers)

	for i := 0; i < workers; i++ {
		low := 2 + i*chunkSize
		high := low + chunkSize - 1
		if i == workers-1 {
			high = n
		}

		task := Task{
			Low:        low,
			High:       high,
			BasePrimes: base,
		}

		t, err := r.Engine.Start(workerImage)
		if err != nil {
			log.Fatalf("Failed to start worker %d: %v", i, err)
		}
		tasks[i] = t
		_ = t.Send(task)
	}

	allPrimes := []int{}
	for i, t := range tasks {
		var res Result
		if err := t.Recv(&res); err != nil {
			log.Fatalf("Failed to receive from worker %d: %v", i, err)
		}
		fmt.Printf("Worker %d result: %d primes, time taken: %.6fs\n", i, len(res.Primes), res.Time)
		allPrimes = append(allPrimes, res.Primes...)
	}

	sort.Ints(allPrimes)

	fmt.Println("Calculating final result")

	sum := 0
	for _, p := range allPrimes {
		sum += p
	}
	fmt.Printf("Total primes found: %d\n", len(allPrimes))
	fmt.Printf("Sum of all primes up to %d: %d\n", n, sum)

	duration := time.Since(start).Seconds()
	fmt.Printf("Total execution time: %.6fs\n", duration)
	fmt.Println("Bye!")
}

func main() {
	parcs.Exec(&Runner{parcs.DefaultRunner()})
}
