#include <bits/stdc++.h>
#include <stdio.h>

int main(){
    printf("--- C++ ---\n");
    int n = 2000;
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