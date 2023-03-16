<br />
 <p align="center">
  <img src="logo.png" alt="Logo" width="80" height="80">

  <h1 align="center">That</h1>
    <p align="center">
    Un llenguatge de programació programable que no et complica la vida
    <br />
    <a href="https://git.aranroig.com/BinarySandia04/that/issues">Informa d'un error</a>
</p>
<p align="center">
  <a href="https://git.aranroig.com/BinarySandia04/that">Pàgina web del repositori</a>
</p>

## Coses
Això encara està en obres. Acabaré el README quan tingui temps. Com m'agrada fer amb tots els projectes que mai acabo, aqui poso una llista amb coses per fer:

## Roadmap

Aquesta llista pot canviar en un futur. La idea es com més o menys seràn les versions (m'estic flipant molt amb això)

### Fase de desenvolupament (pre v1.0)
- [X] Acabar de completar l'assamblador (v0.3)
- [X] Acabar de completar la màquina virtual (v0.4)
- [ ] Solucionar bugs i optimitzar (v0.5)
- [ ] Aconseguir un transcompilador a C++ (v0.6)
- [ ] Fer que sigui un llenguatge totalment estable i bàsic (v0.7)
- [ ] Poder importar llibreries (v0.8)
- [ ] Fer documentació (v0.9)

### Futur
- [ ] Fer una versió estable del llenguatge i una petita documentació (v1.0)
- [ ] Acabar la API per incorporar noves llibreries i gestor de paquets (v2.0)
- [ ] Completar les llibreries estandard per That (v3.0)
- [ ] Multithreading i altres coses molt a futur (v4.0)

## Raons per las quals That hauria d'existir

En un futur m'agradaria que That tingui les següents "features" (molt guais):

- That és un llenguatge interpretat que pot ser transcompilat a C/C++. La idea es que té per una banda una màquina virtual integrada que s'utilitza per defecte, i també en cas que es desitji, es pot transcompilar un codi escrit amb That a C/C++ com a un JIT, de manera que si es vol es pot obtenir el rendiment d'un llenguatge compilat mantenint simplicitat. Entre les funcionalitats de That hi ha:
- Suport per classes. En principi el mateix que a C++ pero ja veurem
- That serà un llenguatge amb precisió numèrica pràcticament infinita utilitzant GMP per defecte. El programador podrà inserir la precisió (en bytes) que necessita de coma flotant. Això és útil per a aplicacions matemàtiques i de no haver de fer estructures més complicades o l'ús de llibreries externes per tal de poder emmagatzemar nombres massa grans. Això pot ser útil per exemple, poder calcular el nombre pi amb una precisió de 300 xifres decimals, per exemple.
- That serà un llenguatge de programació que s'allunyi del paradigma de la programació orientada a objectes, pero sense allunyar-se massa per tal de que sigui cómode i funcional, pero alhora que sigui bastant optimitzat, (ja veurem com).
- That també tindrà un gestor de paquets invisible. La idea principal és que amb una línia (per exemple, `import "algo/algunacosa"`) el programa ja instal·li automàticament els paquets necessaris per funcionar en temps d'execució. A part també vindrà amb una utilitat de terminal per tal de poder gestionar de forma usual els paquets instal·lats. Com que també necessita dependre de CMake, es pot fer que aquests paquets es compilin en temps real o que ja vinguin compilats i l'interpret els pugui cridar. Ja veurem també, pero no ho veig gaire complicat.
- That disposarà d'una gran llibreria estàndard per tal de ser molt ràpid a l'hora de crear aplicacions multimèdia visuals lo més ràpid possible, similar a Processing. En principi vull implementar ImGui i Vulkan en un futur molt llunyà. També hauria de poder compilar in situ aquestes llibreries amb GCC/G++ i doncs que en pugui accedir i es pugui comunicar amb That. També estaria bastant bé fer que no només es limiti a C/C++, sino que també es pugui comunicar amb funcions base d'altres llenguatges de més baix nivell, com Rust per exemple.
- A la llibreria estàndard haurien d'haver-hi funcions modernes com poder fer peticions HTTP, escriure en fitxers, processament d'arxius JSON, manipulació d'imatges, vídeo, entre d'altres.
- That ha de ser un llenguatge bastant fàcil d'incorporar en altres entorns fora de l'execució autònoma.
- That també serà molt modificable i programable, de manera que es pugui modificar cada operació de forma senzilla (com C++) disposant del màxim nombre de símbols i personalització possible
- A part, també el compilador de That pot triar una opció Debug i una final, on el nom ja indica tot, la idea es que la final no tingui la informació per debugar i sigui molt més lleujera i fàcil d'executar
- També hauria de permetre implementar de la forma més senzilla possible (si existeix) la possibilitat de fer execucions paral·leles. (I si es pot amb la GPU també)
## Exemple de programa amb That

```
# That programming language test.

# Això és un comentari

##
Aquest comentari es llarg
Amb That l'execució d'un programa es realitza de la següent manera: primer
es fa una llegida sequencial i es declaren variables i funcions en ordre, i també
s'executen instruccions, i després (si existeix) es crida a una funció main. Es com
el punt d'entrada de Python, bastant similar. Aquesta linea per exemple mostra un 2
##

2;

# Importar alguna cosa
import 'math';

# Importar alguna cosa a un objecte (moduls suposo?)
module Math = import 'math';
# Math.sin(3); # Mostra ~0.14112

# Per declarar variables cal indicar el tipus
int a;
int b = 3;
string c = "Això és una C!!!";
bool q = True;

# Per modificar el valor d'una funció és bastant senzill:
a = b + b * 50 - 3;

# Ara farem una funció. Si no té paràmetres i no retorna res, nomes cal posar la keyword func:

func hola {
  "Hola!";
}

# Si una funció obte paràmetres s'introdueixen després del nom seguit de ':', indicant el tipus i el nom del paràmetre al costat:

func salutacio: string a, string b {
  a, "saluda a", b;
}

# Si una funció ha de retornar algo s'escriu "=>" abans del '{' i després el tipus de retorn:

func suma: int a, int b => int {
  "Anem a sumar", a, "i", b;
  return a + b;
}

func tres => int {
  return 3;
}

# Per executar funcions també és com altres llenguatges:
"Tres més dos és", suma(tres(), 2);

# També existeixen condicionals. Es poden compactificar
if q {
  "Q és True";
  if tres() == 3 {
    "tres és 3";
  } else if suma(2,2) == 4: "2+2 = 4";
    else {
    "Això no és possible!";
  }
} else: print("Q és False");

# Els bucles whiles també son com altres llenguatges
print("Comptem fins a 10!");
int i = 1;
while i <= 10 {
  i;
  i += 1;
}

# També hi ha ordre de trencar while i seguir una altra volta. S'anomenen "break" i "skip" respectivament:
i = 2;
while i < 100 {
  if i % 2 == 0 {
    i += 3;
    skip;
  }
  if i % 3 == 0: i;
  if i % 23 == 0: break;
  i += 1;
}

# També existeix el bucle for, que és un while compacte, igual que els altres llenguatges:
for int j = 0; j < i; j += 1: print(j * i);
```

## Dependències

Les marcades amb un (*) necessiten ser instalades manualment per l'usuari final. (Podria fer un instalador potser).

- [CMake](https://cmake.org/)*
- [GMP - The GNU Multiple Precision Arithmetic Library](https://gmplib.org/)
- [MPFR](https://www.mpfr.org/)