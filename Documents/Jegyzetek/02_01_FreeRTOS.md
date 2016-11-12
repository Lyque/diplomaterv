# FreeRTOS


## Bevezetés

A FreeRTOS a Real Time Engineers Ltd. által fejlesztett valós idejű operációs rendszer. A fejlesztők céljai között volt a rendszer erőforrásigényének minimalizálása, hogy a legkisebb beágyazott rendszereken is futtatható legyen. Ebből adódóan csak az alap funkciók vannak megvalóstva, mint ütemezés, taszkok közötti kommunikáció lehetősége, memóriamenedzsment, de nincs beépített támogatás hálózati kommunikációra vagy bármiféle külső hardver használatára (ezeket vagy nekünk meg megírnunk, vagy harmadik féltől származó kódot kell használnunk).

A rendszer módosított GPLv2 licencet használ. A licencmódosítás lehetővé teszi GPL-től eltérő licenccel ellátott modulok használatát, amennyiben azok a FreeRTOS-sal kizárólag a FreeRTOS API-n keresztül kommunikálnak. 


## Taszkok

A FreeRTOS nem korlátozza a létrehozható taszkok és prioritások számát, amíg a rendelkezésre álló memória lehetővé teszi azok futtatását. A rendszer lehetőséget biztosít ciklikus és nem ciklikus taszkok futtatására egyaránt. 

A beágyazott rendszerek döntő része egymagos processzorokat használ, amiből az következik, hogy egyszerre csak egy taszk futhat. A taszkok eszerint két nagy csoportba oszthatóak: éppen futó taszk (___Fut___ állapot), illetve az összes többi (___Nem fut___ állapot).

![freertos_states](https://github.com/Lyque/diplomaterv/raw/FreeRTOS_notes/Documents/Jegyzetek/Figures/FreeRTOS/01_FreeRTOS_states.png "FreeRTOS states simplified")

Annak, hogy egy taszk éppen miért nem fut, több oka lehet. Ez alapján a __Nem fut__ állapot több állapotra felosztható.
Ha egy taszk képes lenne futni, de például egy nagyobb prioritású taszk birtokolja a processzort, akkor a taszk állapota ___Futásra kész___. Ha a taszk valamilyen eseményre vár (időzítés, másik taszk szinkronizáló jele), akkor a taszk ___Blokkolt___ állapotban van. Az operációs rendszer lehetőséget ad arra, hogy a taszkokat függvényhívással ___Felfüggesztett___ állapotba kényszerítsük. Ekkor egy másik függvényhívással tudjuk visszahozni az ütemezendő feladatok sorába a taszkot.

A taszkok önként lemondhatnak a futásról (időzítés, szinkronizáció, taskYIELD() függvény hívása), viszont futó állapotba csak az ütemező helyezheti.

![freertos_states_expanded](https://github.com/Lyque/diplomaterv/raw/FreeRTOS_notes/Documents/Jegyzetek/Figures/FreeRTOS/02_FreeRTOS_states_expanded.png "FreeRTOS states expanded")


### Idle taszk

A processzor működése közben mindig utasításokat kell futtatnia, ezért legalább egy taszknak Futásra kész állapotban kell lennie. Hogy ez biztosítva legyen, az ütemező indulásakor automatikusan létrejön egy ciklikus ___Idle taszk___.

Az Idle taszk a legkisebb prioritással rendelkezik, így biztosítva, hogy elhagyja a Fut állapotot, amint egy magasabb prioritású taszk Futásra kész állapotba kerül.

Taszk törlése esetén az Idle taszk végzi el a különböző erőforrások felszabadítását. Mivel a FreeRTOS nem biztosít védelmet egy taszk _kiéheztetésével_ szemben ezért fontos, hogy az alkalmazás tervezésekor biztosítsunk olyan időszeletet, amikor másik, nagyobb prioritású taszk nem fut. 


#### Idle hook függvény

Előfordulhat, hogy az alkalmazásunkban olyan funkciót szeretnénk megvalósítani, amelyet az Idle taszk minden egyes iterációjára le kell futtatni (például teljesítménymérés érdekében). Ezt a célt szolgálja az ___Idle hook__ függvény, ami az Idle taszk minden lefutásakor meghívódik. 

Az Idle hook általános felhasználása:
- Alacsony prioritású háttér-, vagy folyamatos feldolgozás,
- A szabad processzoridő mérése (teljesítménymérés),
- Processzor alacsony fogyasztású üzemmódba váltása.


##### Korlátozások az Idle hook függvénnyel kapcsolatban

1. Az Idle hook függvény nem hívhat blokkoló vagy felfüggesztést indító függvényeket[lábjegyzet->A hook függvény blokkolása esetén felléphet az az eset, hogy nincs Futásra kész állapotban levő taszk],
2. Ha az alkalmazás valahol töröl egy taszkot, akkor az Idle hook függvénynek elfogadható időn belül vissza kell térnie. [lábjegyzet->Az Idle taszk felelős a kernel erőforrások felszabadításáért, ha egy taszk törlésre kerül. Ha az Idle taszk bentragad az Idle hook-ban, akkor ez a tisztítás nem tud bekövetkezni.]


## Ütemező

A FreeRTOS által használt ütemezési mechanizmust Fix Prioritásos Preemptív Ütemezésnek hívjuk[lábjegyzet->A FreeRTOS kooperatív ütemezést is támogat, viszont a valós idejű futás eléréséhez a preemptív ütemezés szükséges, ezért a továbbiakban csak a preemptív ütemezéssel foglalkozunk.]. _Fix prioritásos_, mivel a rendszer magától nem változtatja a prioritásokat, _preemptív_, mert egy taszk Futásra kész állapotba lépésekor preemtálja az éppen futó taszkot, ha a futó taszk prioritása alacsonyabb.

A taszkok lehetnek Blokkolt állapotban, ahonnan egy esemény bekövetkezését követően automatikusan Futásra kész állapotba kerülnek. 

Időbeli események azok, amik egy bizonyos időpillanatban következnek be (például egy kéleltetési idő letelik). Az időbeli eseményeket kihasználva lehetőség nyílik periodikus futtatásra, vagy időtúllépés detektálására. 

Szinkronizáló események azok, amikor egy taszk vagy megszakításkezelő rutin jelzést küld valamilyen kommunikációs struktúrán keresztül egy másik taszknak. Tipikusan aszinkron jelzésre használjuk, mint például adat érkezését egy periférián keresztül.


### Taszkok prioritásának meghatározása

Ökölszabályként alkalmazható, hogy a hard real-time funkciókat magasabb prioritással látjuk el, mint a soft real-time funkciókat.

Rate Monotonic Scheduling (Gyakoriság Monoton Ütemezés) az a gyakran alkalmazott prioritás-hozzárendelés, amikor minden taszkhoz egyedi prioritásszintet rendelünk a végrehajtási gyakoriság függvényében. A gyakrabban lefutó folyamatokhoz magasabb, a ritkábban lefutó folyamatokhoz alacsonyabb prioritást állítunk be.


## Kommunikációs struktúrák

Az alkalmazások egymástól független taszkok konstrukciójából áll. Viszont ezeknek a taszkoknak ahhoz, hogy a feladatukat el tudják látni, gyakran kommunikálniuk kell egymással.
A FreeRTOS három alap kommunikációs struktúrát kínál a felhasználónak:
- sor (__queue__),
- szemafor (__semaphore__),
    - bináris (__binary semaphore__),
    - számláló (__counting semaphore__),
- mutex (__mutex__ [<b>MUT</b>ual <b>EX</b>cluison]).


### Sorok (queues)

A sorok fix méretű adatból tudnak véges számú üzenetet tárolni. Ezek a jellemzők a sor létrehozásakor kerülnek meghatározásra. Alapértelmezetten FIFO-ként működnek (First In First Out), bár a FreeRTOS lehetővé teszi a sor elejére való írást is.
A sorba való írás során másolat készül az eredeti változóról, és ez a másolat kerül tárolásra a sorban. Olvasáskor a paraméterként átadott memóriacímre kerül átmásolásra, ezért figyelni kell arra, hogy az átadott változó által elfoglalt memória legalább akkora legyen, mint a sor által tartalmazott adattípus mérete. A FreeRTOS API kétféle függvényt használ olvasásra:
- Az egyik automatikusan eltávolítja a kiolvasott elemet a sorból (__xQueueReceive()__),
- A másik kiolvassa a soronkövetkező elemet, de azt nem távolítja el a sorból(__xQueuePeek()__).

A FreeRTOS-ban minden kommunikációs struktúra a sor valamilyen speciális megvalósítása.

A sorok egy taszkhoz sem tartoznak, így egy sorba akár több taszk is írhat, illetve olvashat egy alkalmazáson belül. 

#### Olvasás sorból

Sorból való olvasás során az olvasó függvény paramétereként megadhatunk egy várakozási időtartamot. A taszk maximálisan ennyi ideig várakozik Blokkolt állapotban új adat érkezésére, amennyiben a sor üres. Ha ezen időtartam alatt nem érkezik adat, akkor a függvény visszatér, és a visszatérési értékével jelzi az olvasás sikertelenségét. A Blokkolt állapotból a taszk Futásra kész állapotba kerül, ha:
- Adat érkezik a sorba a megadott időtartamon belül,
- Nem érkezik adat a sorba, de a megadott várakozási idő lejárt.

Egy sorból egyszerre több taszk is kezdeményezhet olvasást, ezért előfordulhat az az eset, hogy több taszk is Blokkolt állapotban várja az adat érkezését. Ebben az esetben csak egy taszk kerülhet Futásra kész állapotba az érkezésének hatására. A rendszer a legnagyobb prioritású taszkot választja, vagy ha a várakozó taszkok azonos prioritásúak, akkor a legrégebben várakozót helyezi a Futásra kész állapotba.


#### Írás sorba

Az olvasáshoz hasonlóan a sorba való írás során is megadható egy várakozási idő. Ha a sorban nincs üres hely az adat írásához, akkor a taszk Blokkolt állapotba kerül, amelyből Futásra kész állapotba lép, ha:
- Megüresedik egy tároló a sorban,
- Letelik a maximális várakozási idő.

Egy sorba több taszk is kezdeményezhet írást egyidőben. Ekkor ha több taszk is Blokkolt állapotba kerül, akkor hely felszabadulásakor a legnagyobb prioritású taszkoz választja a rendszer, azonos prioritású taszkok esetén a legrégebben várakozót helyezi Futásra kész állapotba.


### Szemaforok

Két szemafor típust különböztetünk meg:
- Bináris szemafor, amikor a szemafor két értéket vehet fel,
- Számláló szemafor, mikor a szemafor több állapotot is felvehet.

A szemaforon két művelet értelmezett:
- A szemafor jelzése (elterjedt elnevezések: give, signal, post, V() művelet[lábjegyzet-> Dijkstra holland, verhogen]), amikor a szemafor értéke növelésre kerül.
- A szemafor elvétele (elterjedt elnevezések: take, wait, pend, P() művelet [lábjegyzet-> proberen]), amikor a szemafor először tesztelésre kerül, hogy tartalmaz-e elemet, ha igen, akkor az értékét csökkentjük, ha nem, akkor várakozunk addig, amíg valamelyik másik taszk elérhetővé nem teszi azt.

A FreeRTOS rendszerben implementált szemaforok paraméterei között megadható a maximális tick szám, ameddig várakozhat a szemaforra az adott taszk. Ezen maximális időtartam megadása az esetek nagy részében megoldást jelent a holtponti helyzetekre.

A szemafor látszólag egyszerűen helyettesíthető egy egyszerű változó (boolean vagy előjel nélküli egész) használatával, viszont a beépített szemafor struktúra atomi műveletként kerül kezelésre (így nem következik be ütemezés a szemafor tesztelése és állítása közben), illetve a rendszer automatikusan tudja kezelni a várakozó folyamatok állapotok közti mozgatását.

#### Bináris szemafor

Bináris szemafor esetén a szemafor két értéket vehet fel. Felfogható úgy is, mint egy egy adat tárolására (egy elem hosszú) sor, melynek nem vizsgáljuk a tartalmazott értékét, csak azt, hogy éppen tartalmaz-e adatot vagy sem.

Leggyakoribb felhasználása a taszkok szinkronizálása. Ekkor az egyik taszk a futásának egy adott pontján várakozik egy másik taszk jelzésére. Ezzel a módszerrel megvalósítható a megszakítások taszkokban történő kezelése, ezzel is minimalizálva a megszakítási rutin hosszát.

Bináris szemafor használatakor különös figyelmet kell fordítani arra, hogy ha a szemafor egy adott taszkban gyakrabban kerül jelzésre, mint ahogy feldolgozzuk, akkor jelzések veszhetnek el (amíg az egyik jelzés várakozik, addig az utána következő eseményeknek nincs lehetőségük várakozó állapotba kerülni).


#### Számláló szemafor

A számláló típusú szemafor minden jelzéskor növeli az értékét. Ekkor (amíg el nem éri a maximális értékét) nem kerül Blokkolt állapotba a jelző taszk. A számláló szemafor felfogható úgy, mint egy egynél több adat tárolására képes sor, melynek nem vizsgáljuk az értékét, csak azt, hogy éppen tartalmaz-e még adatot vagy sem.

Két felhasználása elterjedt a számláló szemaforoknak:
- Események számlálása: ekkor minden esemény hatására növeljük a szemafor értékét (új elemet helyezünk a sorba). A szemafor aktuális értéke a beérkezett és a feldolgozott események különbsége. A számlálásra használt szemafor inicializálási értéke nulla.
- Erőforrás menedzsment: ekkor a szemafor értéke a rendelkezésre álló erőforrások számát mutatja. Mikor az operációs rendszertől az erőforrást igényeljük, akkor a szemafor értékét csökkentjük, mikor felszabadítjuk a birtolkolt erőforrást, akkor a szemafor értékét növeljük. Ha a szemafor értéke nulla, akkor nincs rendelkezésre álló erőforrás. Erőforrások kezelésére használt számláló szemafor esetén az inicializálási érték az elérhető erőforrások száma.


### Mutexek




## Megszakítás-kezelés

