# Mostra el nombre 10000 primer
n = 2000
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
    i += 1;
    if divisible == 0:
        t += 1;
print(i-1);