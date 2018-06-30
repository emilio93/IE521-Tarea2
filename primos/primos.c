#include <math.h>
#include <stdio.h>
#include <sys/time.h>

// identify wether number is prime or not
int isPrime(int n) {

  // base cases
  if (n <= 1) {
    return 0;
  } else if (n <= 3) {
    return 1;
  } else if (n % 2 == 0 || n % 3 == 0) {
    return 0;
  }
  // first to check
  int i = 5;

  // check until ceil(sqrt(n))
  while (i * i <= n) {
    // check if n is divisible by multiple of i
    if (n % i == 0 || n % (i + 2) == 0) {
      return 0;
    }
    // 6 following cases already ruled out
    i = i + 6;
  }
  return 1;
}

int main(int argc, char *argv[]) {

  // prime numbers found so far
  int n = 0;

  // setup elapsed time vars
  long start, end;
  struct timeval timecheck;

  // save start time
  gettimeofday(&timecheck, NULL);
  start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;

  // get first 500 000 primes
  for (int i = 0; n < 500000; i++) {
    if (isPrime(i) > 0) {
      n++;
    }
  }

  // get time at end and get elapsed time
  gettimeofday(&timecheck, NULL);
  end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
  printf("%ld milliseconds elapsed\n", (end - start));

  return 0;
}
