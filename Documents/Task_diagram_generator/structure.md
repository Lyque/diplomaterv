# Funkciók, felépítés

A python script a template-eket használva másolással hozza létre a keretet, aminek a belsejét tölti fel a szükséges sorokkal. Kommentelt sorokkal jelzi a hasznos sorok kezdetét és végét.

A taszkok és egyéb objektumok (időbeli jelzések, szemafor átadások, nyilak, diagram beállításai) paramétereit JSON objektumból tölti be a script


## A JSON objektum tartalma

### Beállítás

- Taszkok közti távolság (függőleges),
- Taszkváltási idő (két szomszédos taszk eltolása),
- Diagram neve (kommentben megjelenik).


### Taszkok beállításai

- Név,
- Megjegyzés (opc),
- Mettől,
- Hossz (opc),
- Meddig (opc),
- Szín (opc),
- Felirat (opc).


### Egyéb sor (pl szemafor tulajdonos jelzése)

- Név,
- Téglalap tulajdonságai:
    - Mettől,
    - Hossz (opc),
    - Meddig (opc),
    - Fölé/Alá,
    - Szöveg.


### Függőleges jelzések

- Mettől,
- Hossz (opc),
- Meddig (opc),
- Felirat (opc).


### Intertaszk jelzések

- Mettől,
- Meddig,
- Melyik taszktól,
- Melyik taszkhoz,
- Felirat (opc),
- Típus.


### Egyéb jelzések (pl szemafor adás/vétel)

- Melyik taszk,
- Mikor.



---------------------------------------------------------------------------------------

# Ábra alapja

- [Forrás](http://tex.stackexchange.com/questions/57533/how-to-draw-a-diagram-for-packets-scheduling-in-networks-or-task-scheduling)