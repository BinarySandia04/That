<br />
 <p align="center">
  <img src="logo.png" alt="Logo" width="80" height="80">

  <h1 align="center">That</h1>
    <p align="center">
    Un llenguatge de programació programable interpretat petit i senzill
    <br />
    <a href="https://git.aranroig.com/BinarySandia04/that/issues">Informa d'un error</a>
</p>
<p align="center">
  <a href="https://git.aranroig.com/BinarySandia04/that">Pàgina web del repositori</a>
</p>

## Coses
Això encara està en obres. Acabaré el README quan tingui temps. Com m'agrada fer amb tots els projectes que mai acabo, aqui poso una llista amb coses per fer:

## Coses per fer en total

Aquesta llista pot canviar en un futur

- [ ] Fer una versió estable del llenguatge
- [ ] Fer un README
- [ ] Fer un logo
- [ ] Fer una llibreria estandard per That

## Raons per las quals That hauria d'existir

En un futur m'agradaria que That tingui les següents "features" (molt guais):

- That serà un llenguatge amb precisió numèrica pràcticament infinita utilitzant GML per defecte. El programador podrà inserir la precisió (en bytes) que necessita de coma flotant. Això és útil per a aplicacions matemàtiques i de no haver de fer estructures més complicades o l'ús de llibreries externes per tal de poder emmagatzemar nombres massa grans. Això pot ser útil per exemple, poder calcular el nombre pi amb una precisió de 300 xifres decimals, per exemple.
- That serà un llenguatge de programació que s'allunyi del paradigma de la programació orientada a objectes, pero sense allunyar-se massa per tal de que sigui cómode i funcional, pero alhora que sigui bastant optimitzat
- That també tindrà un gestor de paquets invisible. La idea principal és que amb una línia (per exemple, `import "algo/algunacosa"`) el programa ja instal·li automàticament els paquets necessaris per funcionar en temps d'execució. A part també vindrà amb una utilitat de terminal per tal de poder gestionar de forma usual els paquets instal·lats.
- That disposarà d'una gran llibreria estàndard per tal de ser molt ràpid a l'hora de crear aplicacions multimèdia visuals lo més ràpid possible, similar a Processing. En principi vull implementar ImGui i Vulkan en un futur molt llunyà. També hauria de poder compilar in situ aquestes llibreries amb GCC/G++ i doncs que en pugui accedir i es pugui comunicar amb That. També estaria bastant bé fer que no només es limiti a C/C++, sino que també es pugui comunicar amb funcions base d'altres llenguatges de més baix nivell, com Rust per exemple.
- A la llibreria estàndard haurien d'haver-hi funcions modernes com poder fer peticions HTTP, escriure en fitxers, processament d'arxius JSON, manipulació d'imatges, vídeo, entre d'altres
- That ha de ser un llenguatge bastant fàcil d'incorporar en altres entorns fora de l'execució autònoma.
- That també serà molt modificable i programable, de manera que es pugui modificar cada operació de forma senzilla (com C++) disposant del màxim nombre de símbols i personalització possible
- A part, també el compilador de That pot triar una opció Debug i una final, on el nom ja indica tot, la idea es que la final no tingui la informació per debugar i sigui molt més lleujera i fàcil d'executar
- També hauria de permetre implementar de la forma més senzilla possible (si existeix) la possibilitat de fer execucions paral·leles. (I si es pot amb la GPU també)
## Exemple de programa amb That

```
# That programming language test.

# Això és un comentari

func main { # Aquest programa fa print de 3 + 4
    int a = 3 + 4;
    print(a);
    if a == 7 {
        print("Hello from that!");
    }
}
```

## Dependències
