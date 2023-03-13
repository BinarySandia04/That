#include <stdio.h>

int main(){
    printf("--- C ---\n");
    int n = 5000;
    int t = 0;
    int i = 2;
    while(t < n){
        int divisible = 0;
        int j = 2;
        while(j < i){
            if(i % j == 0){
                divisible = 1;
                break;
            }
            j++;
        }
        if(divisible == 0){
            if(t == (n-1)) printf("%d\n", i);
            t++;
        }
        i++;
    }

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