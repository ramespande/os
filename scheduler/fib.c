#include <stdio.h>
#include <stdlib.h>
int fib(int n) {
    if (n<2) return n;
    else return fib(n-1)+fib(n-2);
}
int main(int argc, char *argv[]) {
    int n=atoi(argv[1]);
    int val=fib(n);
    if (n<0) printf("Invalid number\n");
    else printf("fib(%d) = %d\n", n, val);
    return 0;
}

