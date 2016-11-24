# Teljesítménymérő metrikák

Asztali alkalmazás fejlesztésekor a rendelkezésre teljesítmény és tárhely ma már nem számít akadálynak. Ha valamelyik erőforrás szűk keresztmetszetté válik, akkor alkatrészcserével általában megoldható.

Nem ez a helyzet beágyazott rendszerek esetén. A rendszer központi egységének számítási kapacitása általában nem haladja meg nagy mértékben az elégséges szintet, így különösen figyelni kell a fejlesztés során, hogy az implementált kód hatékony legyen. A rendelkezésre álló memória sem tekinthető korlátlannak, és gyakran a bővítés is nehézkes, esetleg nem megoldható. Az eszköz fogyasztása is fontos szempont, amit a szoftver szintén nagy mértékben befolyásolhat.

Az operációs rendszer választása összetett folyamattá is bonyolódhat, mert mérlegelni kell az alkalmazásunk igényeit, az operációs rendszer támogatottságát (támogatott mikrokontrollerek, fórumok, gyártói támogatás), a becsülhető fejlesztési időt és az ezzel járó költségeket, illetve fizetős operációs rendszer esetén a rendszer árát.

A választást az sem segíti előre, hogy nincs egyértelmű módszer az operációs rendszerek értékelésére. [lábjegyzet-> 
Bár a Német Szabványügyi Intézet (Deutsche Institut für Normung - DIN) 
az 1980-as évek végén hozott létre szabványt a folyamatirányító számítógépes rendszerek teljesítménymutatóinak mérésére (DIN 19242 szabvány-sorozat), a valós idejű operációs rendszerek értékelésére ez nem jelent megoldást.]

A továbbiakban különböző forrásokból vett szempontokat vizsgálok meg, majd azok alapján állítom fel a dolgozat során megfigyelt tulajdonságok listáját.


# Memóriaigény

A mikrokontrollerek területén a memória mérete korlátozott (ROM és RAM egyaránt), ezért fontos, hogy a használt rendszer minél kisebb lenyomattal rendelkezzen.


## Késleltetés

A rendszer késleltetése az az idő, ami egy esemény beérkezésétől a rendszer válaszáig eltelik. Ezt okozhatja a mikrovezérlő megszakítási mechanizmusához szükséges műveletek sora, az operációs rendszer ütemezőjének overhead-je, de a közben végrehajtandó feladat is nagy mértékben befolyásolja a nagyságát. 


## Jitter

A jitter egy folyamat vizsgálata során a többszöri bekövetkezés után mért késleltetésekből határozható meg.


## Rhealstone




## Legrosszabb válaszidő


## Választott metrikák



--------------------------------------------------------------------------

## Operációs rendszer specifikus

- Memóriaigény:
    - Skálázhatóság,
- Késleltetés:
    - Megszakítás,
    - Ütemezés,
- Kernel szolgáltatások:
    - Kommunikációs objektumok kezelése,
- Kódhordozhatóság,
- Valós idejűség,
- Megbízhatóság.


## Mikrokontroller specifikus

- Aritmetikai utasítások elvégzése különböző számábrázolások esetén:
    - 32/64 bites számok,
    - Lebegőpontos aritmetika,
- Megszakítások:
    - Késleltetés,
    - Prioritás,
    - Egymásba ágyazás,
- Grafikai gyorsítás.




# Források

- [Embedded OS metrics](http://www.embedded.com/design/operating-systems/4437792/Why-and-how-to-measure-your-RTOS-performance)
- PDF-ek