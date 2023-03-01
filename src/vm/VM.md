# Com funciona la màquina virtual

Val la idea es que primer el que fa es llegir
un nombre determinat de constants (un enter).

Després, en aquestes constants es decideix el tipus,
i també pot ser una funció també.

Al final, hi ha el codi del main, que té les següents
instruccions:

# Memòria
Hi ha tres llocs de memòria:

- Espai constant (Memòria fixa, mida fixada pel programa)
- Registres (Memòria variable, mida fixada pel programa)
- Stack (Memòria variable, mida variable pel programa)

# Instruccions

Hi ha les següents:

- LOAD
- LOADC
- PUSH
- CLOSE
- MOVE
- CALL
- ICL
- RET
- ADD
- SUB
- MUL
- DIV
- EQ
- LT
- GT
- LEQ
- GEQ
- JUMP

TODO: Cal afegir més operadors per les instruccions i tal

### LOAD
Carrega una cosa del stack al registre
Stack => Registre

### LOADC
Carrega una constant al registre
Constant => Registre

### PUSH

A, Bx
PUSH el que fa és afegir al stack unes constants carregada al registre.
Registre => Stack

### CLOSE

Treu tots els stacks afegits per l'últim push

### MOVE

Copia el contingut d'un registre a un altre. És de tipus `a,b`
Registre <=> Registre

### CALL

Crida a una funció guardada en `a` els paràmtres que hi ha als registres de la direcció `b` fins a la `c`

### ICL

Crida a una funció interna guardada en la direcció `a` amb els paràmetres que hi ha als registres de la direcció `b` (inclosa) fins a la `c` (exclosa)

### RET

Retorna un valor de la funció suposo

### ADD

Operador `+`. Els operadors el que fan és operar la direcció `a` amb `b` i guarden el resultat a `c`

### SUB

Operador `-`

### MUL

Operador `*`

### DIV

Operador `/`

## JUMP

Si `a` és un valor de veritat, salta a `bx`