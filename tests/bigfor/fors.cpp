#include <stdio.h>

int main(){
    int i = 0;
    for(i = 0; i < 50000000; i++);
    printf("%d\n", i);
}