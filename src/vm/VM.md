# Com funciona la màquina virtual

Val la idea es que primer el que fa es llegir
un nombre determinat de constants (un enter).

Després, en aquestes constants es decideix el tipus,
i també pot ser una funció també.

Al final, hi ha el codi del main, que té les següents
instruccions:

# Instruccions

Hi ha les següents:

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

### PUSH

PUSH el que fa és afegir al stack una constant determinada.

També hauria de poder afegir-hi directament alguna dada per anar més ràpid i tal suposo?

La idea es que llegeix el tipus `abx` on abx doncs és l'adreça relativa de la constant

### CLOSE

Treu tots els registers afegits per l'últim push

### MOVE
Copia el contingut d'un registre a un altre. És de tipus `a,b`

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