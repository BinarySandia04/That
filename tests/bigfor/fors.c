#include <stdio.h>

int main(){
    int i = 0;
    for(i = 0; i < 50000000; i++);
    printf("%d\n", i);
}

/*
print("--- PYTHON ---")
n = 5000
t = 0
i = 2
while(t < n):
    divisible = 0
    j = 2
    while j < i:
        if i % j == 0:
            divisible = 1;
            break
        j += 1
    # "Miau " + t;
    if divisible == 0:
        # i és divisible
        if t == (n - 1):
            print(i);
        t += 1;
    i += 1;
*/