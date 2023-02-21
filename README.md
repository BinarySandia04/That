<br />
 <p align="center">
  <img src="logo.png" alt="Logo" width="80" height="80">

  <h1 align="center">That</h1>
    <p align="center">
    Un llenguatge de programació programable interpretat molt petit
    <br />
    <a href="https://git.aranroig.com/BinarySandia04/that/issues">Informa d'un error</a>
</p>
<p align="center">
  <a href="https://git.aranroig.com/BinarySandia04/that">Pàgina web del repositori</a>
</p>

## Coses
Això encara està en obres. Acabaré el README quan tingui temps. Com m'agrada fer amb tots els projectes que mai acabo, aqui poso una llista amb coses per fer:

## Coses per fer en concret

- [ ] Fer ifs i elses encadenats infinits
- [ ] Fer un bucle while
- [ ] Parchejar una cosa de que no pillava bé les operacions o una cosa així

## Coses per fer en total

Aquesta llista pot canviar en un futur

- [ ] Fer una versió estable del llenguatge
  - [X] Poder crear variables
  - [X] Poder assignar variables a operacions
  - [ ] Aconseguir transformar AST a bytecode
  - [ ] Acabar la màquina virtual
  - [ ] Aconseguir fer variables amb GML
  - [ ] Tapar memory leaks (amb loctite)
- [ ] Fer un README
- [ ] Fer una llibreria estandard per That
- [ ] Fer un logo

## Coses que That farà (roadmap)

En un futur m'agradaria que That tingui les següents "features" (molt guais):

- That serà un llenguatge amb precisió numèrica pràcticament infinita utilitzant GML per defecte. El programador podrà inserir la precisió (en bytes) que necessita de coma flotant. Això és útil per a aplicacions matemàtiques i de no haver de fer estructures més complicades o l'ús de llibreries externes per tal de poder emmagatzemar nombres massa grans. Això pot ser útil per exemple, poder calcular el nombre pi amb una precisió de 300 xifres decimals, per exemple.
- That serà un llenguatge de programació més inclinat cap a programació orientada a objectes.
- That també tindrà un gestor de paquets invisible. La idea principal és que amb una línia (per exemple, `import "algo/algunacosa"`) el programa ja instal·li automàticament els paquets necessaris per funcionar en temps d'execució. A part també vindrà amb una utilitat de terminal per tal de poder gestionar de forma usual els paquets instal·lats.
- That disposarà d'una gran llibreria estàndard per tal de ser molt ràpid a l'hora de crear aplicacions multimèdia visuals lo més ràpid possible, similar a Processing. En principi vull implementar ImGui i Vulkan en un futur molt llunyà
- A part a la llibreria estàndard haurien d'haver-hi funcions modernes com poder fer peticions HTTP, escriure en fitxers, processament d'arxius JSON, manipulació d'imatges, entre d'altres
- A part, That ha de ser un llenguatge bastant fàcil d'incorporar en altres entorns fora de l'execució autònoma.

## Exemple de programa amb That

```
# That programming language test.

# Això és un comentari

##
Això és un comentari llarg
que no es veu bonic perque al markdown
no he posat syntax highlight.
##

func main { # Aquest programa fa print de 3 + 4
    int a = 3 + 4;
    print(a);
    if a == 7 {
        print("Hello from that!");
    }
}
```