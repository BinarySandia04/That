<br />
 <p align="center">
  <img src="logo.png" alt="Logo" width="80" height="80">

  <h1 align="center">Zag</h1>
    <p align="center">
    The most powerful and dumbest programming language
    <br />
    <a href="https://git.aranroig.com/BinarySandia04/Wyrm/issues">Informa d'un error</a>
</p>
<p align="center">
  <a href="https://git.aranroig.com/BinarySandia04/Wyrm">Pàgina web del repositori</a>
</p>
<p align="center">
<img src="https://img.shields.io/github/repo-size/BinarySandia04/Wyrm?color=%23fff&label=Repo%20size&style=for-the-badge">
<img src="https://img.shields.io/github/languages/code-size/BinarySandia04/Wyrm?color=%23ccc&style=for-the-badge">
<img src="https://img.shields.io/github/v/tag/BinarySandia04/Wyrm?color=%23a5dff2&label=Version&style=for-the-badge">
<img src="https://img.shields.io/github/downloads/BinarySandia04/Wyrm/total?style=for-the-badge">
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
- [ ] Completar les llibreries estandard per Wyrm (v3.0)
- [ ] Multithreading i altres coses molt a futur (v4.0)

## Motivació

Volia fer un llenguatge de programació molt lleuger i també molt potent per tal de poder
computar mètodes numèrics de forma fàcil i ràpida. Principalment Wyrm està actualitzat i desenvolupat
per a funcionar com a motor base per les computacions que fa [Calcatrum](https://git.aranroig.com/BinarySandia04/Calcutrum), una calculadora gràfica programada amb [Godot](https://github.com/godotengine/godot) altament modificable (gràcies a Wyrm) i feta exclusivament per poder ensenyar millor conceptes matemàtics avançats

## Raons per las quals Wyrm hauria d'existir

En un futur m'agradaria que Wyrm tingui les següents "features" (molt guais):

- Wyrm és un llenguatge interpretat que pot ser transcompilat a C/C++. La idea es que té per una banda una màquina virtual integrada que s'utilitza per defecte, i també en cas que es desitji, es pot transcompilar un codi escrit amb Wyrm a C/C++ com a un JIT, de manera que si es vol es pot obtenir el rendiment d'un llenguatge compilat mantenint simplicitat. Entre les funcionalitats de Wyrm hi ha:
- Suport per classes. En principi el mateix que a C++ pero ja veurem
- Wyrm serà un llenguatge amb precisió numèrica pràcticament infinita utilitzant GMP per defecte. El programador podrà inserir la precisió (en bytes) que necessita de coma flotant. Això és útil per a aplicacions matemàtiques i de no haver de fer estructures més complicades o l'ús de llibreries externes per tal de poder emmagatzemar nombres massa grans. Això pot ser útil per exemple, poder calcular el nombre pi amb una precisió de 300 xifres decimals, per exemple.
- Wyrm serà un llenguatge de programació que s'allunyi del paradigma de la programació orientada a objectes, pero sense allunyar-se massa per tal de que sigui cómode i funcional, pero alhora que sigui bastant optimitzat, (ja veurem com).
- Wyrm també tindrà un gestor de paquets invisible. La idea principal és que amb una línia (per exemple, `import "algo/algunacosa"`) el programa ja instal·li automàticament els paquets necessaris per funcionar en temps d'execució. A part també vindrà amb una utilitat de terminal per tal de poder gestionar de forma usual els paquets instal·lats. Com que també necessita dependre de CMake, es pot fer que aquests paquets es compilin en temps real o que ja vinguin compilats i l'interpret els pugui cridar. Ja veurem també, pero no ho veig gaire complicat.
- Wyrm disposarà d'una gran llibreria estàndard per tal de ser molt ràpid a l'hora de crear aplicacions multimèdia visuals lo més ràpid possible, similar a Processing. En principi vull implementar ImGui i Vulkan en un futur molt llunyà. També hauria de poder compilar in situ aquestes llibreries amb GCC/G++ i doncs que en pugui accedir i es pugui comunicar amb Wyrm. També estaria bastant bé fer que no només es limiti a C/C++, sino que també es pugui comunicar amb funcions base d'altres llenguatges de més baix nivell, com Rust per exemple.
- A la llibreria estàndard haurien d'haver-hi funcions modernes com poder fer peticions HTTP, escriure en fitxers, processament d'arxius JSON, manipulació d'imatges, vídeo, entre d'altres.
- Wyrm ha de ser un llenguatge bastant fàcil d'incorporar en altres entorns fora de l'execució autònoma.
- Wyrm també serà molt modificable i programable, de manera que es pugui modificar cada operació de forma senzilla (com C++) disposant del màxim nombre de símbols i personalització possible
- A part, també el compilador de Wyrm pot triar una opció Debug i una final, on el nom ja indica tot, la idea es que la final no tingui la informació per debugar i sigui molt més lleujera i fàcil d'executar
- També hauria de permetre implementar de la forma més senzilla possible (si existeix) la possibilitat de fer execucions paral·leles. (I si es pot amb la GPU també)

El llenguatge de programació Wyrm per defecte genera una "representació intermitja" (feta per mi, similar a la de LLVM).

- **Wyrm**: Llenguatge de programació. Passa un projecte de Wyrm a la representació intermitja
- **Phi**: Interprete de Wyrm
- **Drac**: Compilador de Wyrm
- **Zmey**: Gestor de paquets i llibreries de Wyrm

## Com està estructuras Wyrm

Wyrm es molt extensible, la manera la qual es fa aquesta extensió és amb mòduls, que són llibreries dinàmiques amb funcions exposades.
Aquestes llibreries dinàmiques han d'importar el fitxer `wyrm.h` per tal de poder definir una llibreria correctament.

Hi ha dos tipus de llibreries a Wyrm:

- Mòduls: Afegeixen funcionalitat extra al llenguatge
- Runners: Executen el llenguatge de forma diferent. Poden ser tant interprets, compiladors o JITs, qualsevol cosa que executi codi

Els mòduls defineixen gran part del comportament del llengatge. Els runners, per tal de executar el codi per tant, han de fer
referència als moduls. Per poder solucionar aquest problema, aquests carreguen les llibreries del directori on s'instala Wyrm,
anteriorment i la qual també a l'hora de compilar el runner, aquest gràcies a `wyrm.h` tindrà mètodes per tal de trobar definicions
d'aquestes funcions de forma automàtica dins de la llibreria compartida.

## Dependències

Les marcades amb un (*) necessiten ser instalades manualment per l'usuari final. (Podria fer un instalador potser).

- [CMake](https://cmake.org/)* (En cas que es vulgui compilar)
- [SWIG](https://www.swig.org/)
- [GMP - The GNU Multiple Precision Arithmetic Library](https://gmplib.org/)
- [MPFR](https://www.mpfr.org/)

## Build
- CMake
- build-essential
- mkdocs
  - pip
    - python
