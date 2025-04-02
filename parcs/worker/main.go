package main

import (
	"log"
	"math"
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

type Worker struct {
	*parcs.Service
}

func (w *Worker) Run() {
	start := time.Now() // <--- запам'ятовуємо час ще до Recv()

	log.Println("Waiting for task...")

	var task Task
	if err := w.Recv(&task); err != nil {
		log.Fatalf("Error receiving task: %v", err)
	}

	log.Printf("Task received: Low=%d, High=%d, BasePrimes=%d", task.Low, task.High, len(task.BasePrimes))

	size := task.High - task.Low + 1
	isPrime := make([]bool, size)
	for i := range isPrime {
		isPrime[i] = true
	}

	for _, p := range task.BasePrimes {
		start := int(math.Max(float64(p*p), float64((task.Low+p-1)/p)*float64(p)))
		for j := start; j <= task.High; j += p {
			isPrime[j-task.Low] = false
		}
	}

	primes := []int{}
	for i := task.Low; i <= task.High; i++ {
		if i >= 2 && isPrime[i-task.Low] {
			primes = append(primes, i)
		}
	}

	execTime := time.Since(start).Seconds()

	log.Printf("Worker found %d primes in %.6f seconds", len(primes), execTime)

	if err := w.Send(Result{Primes: primes, Time: execTime}); err != nil {
		log.Fatalf("Error sending result: %v", err)
	}
}

func main() {
	parcs.Exec(&Worker{parcs.DefaultService()})
}
