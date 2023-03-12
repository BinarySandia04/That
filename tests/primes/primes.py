# Mostra el nombre 10000 primer
print("--- PYTHON ---")
n = 4000
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