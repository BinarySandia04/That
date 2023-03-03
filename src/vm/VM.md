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
- DEF
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
- TEST
- JUMP
- TO
- END

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

Treu tal nombre d'stacks


### MOVE

Copia el contingut d'un registre a una posició d'un stack. És de tipus `a,b`
Registre => Stack

### MOVER

Copia el contingut d'un registre a un altre. És de tipus `a,b`
Registre => Registre

### CALL

Crida a una funció guardada en `a` els paràmtres que hi ha als registres de la direcció `b` fins a la `c`

### DEF

Crida la funció predeterminada d'un objecte. Per defecte, està definida com una sortida standard per `stdout`

### ICL

Crida a una funció interna guardada en la direcció `a` amb els paràmetres que hi ha als registres de la direcció `b` (inclosa) fins a la `c` (exclosa)

### RET

Retorna un valor de la funció del registre `a`

### ADD

Operador `+`. Els operadors el que fan és operar la direcció `a` amb `b` i guarden el resultat a `c`

### SUB

Operador `-`

### MUL

Operador `*`

### DIV

Operador `/`

## TEST

Si `a` és un valor de fals, salta a `bx`

## JUMP

Salta a `bx`

## TO

Comença la creació de una funció i la penja al stack

## END

Declara el final d'una funció