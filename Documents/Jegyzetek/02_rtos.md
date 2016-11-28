# RTOS meghatározása ([FreeRTOS](http://www.freertos.org/about-RTOS.html))

## Operációs rendszer meghatározás

Az operációs rendszer egy szoftver, ami kezeli a számítógép alap funkcióit és szolgáltatásokat biztosít más programok (vagy alkalmazások) számára. Az alkalmazások valósítják meg azt a funkcionalitást, amire a számítógép felhasználójának szüksége van, vagy a számítógép felhasználója akar. Az operációs rendszer által nyújtott szolgálatások az alkalmazások feljesztését egyszerűsítik, ezáltal felgyorsítják a fejlesztést és karbantarthatóbbá teszik a programot.

Többféle operációs rendszert különböztetünk meg a futtatható folyamatok, az egyidejűleg kezelt felhasználók számának és az ütemező működésének függvényében. A továbbiakban csak a multitaszkot támogató operációs rendszerekkel foglalkozom.


## Real-time operációs rendszer meghatározás

A legtöbb operációs rendszer látszólag lehetővé teszi több program egyidejű futtatását (multitasking). A valóságban minden processzor mag csak egy szálat tud egy időpillanatban futtatni. Az operációs rendszer ütemezője a felelős azért, hogy eldöntse, melyik program mikor fusson, és a programok közti gyors váltással éri el az egyidejű futás látszatát.

[Kép]

Az operációs rendszer típusát az ütemező döntési mechanizmusa határozza meg. Egy real time operációs rendszer ütemezője úgy van megtervezve, hogy a végrehajtási minta determinisztikus legyen. Ez beágyazott rendszerek esetén érdekes, mert a beágyazott rendszereknél gyakran követelmény a valósidejűség, vagyis hogy a rendszernek egy szigorúan meghatározott időn belül reagálnia kell egy adott eseményre. A. valósidejű követelményeknek való megfelelés csak úgy lehetséges, ha az operációs rendszer ütemezője előre megjósolható döntéseket hoz.

Valósidejű operációs rendszerek közül megkülönböztetjük a soft real-time és a hard real-time rendszereket.

Soft real-time rendszer esetén nem probléma, ha nem érkezik válasz a megadott határidőn belül, az csak csak a rendszer minősítését rontja.

Hard real-time rendszer esetén viszont a rendszer alkalmatlanná válik a feladatra, ha a határidőt nem tudja betartani. Például egy személygépjármű légzsákjának késedelmes nyitása akár halálos következményekkel is járhat.

## Operációs rendszer feladatai

Az operációs rendszer elsődleges feladatad a használt processzor perifériáinak kezelése, azokhoz meghatározott interfész biztosítása. További feladata még a létrehozott taszkok ütemezése, a taszkok közötti kommunikáció és szinkronizáció megvalósítása.


### Ütemezés

Az operációs rendszer egyik meghatározó része a használt ütemező működésének elve, mely meghatározza az eszköz alkalmasságát egy adott feladatra. Az ütemező dönti el, hogy a futásra kész taszkok közül melyik futhat a következő ütemezési szakaszban. További feladata tasztváltáskor az éppen futó taszk állapotának elmentése és a futtatandó taszk állapotának betöltése. Az ütemező három okból futhat le:
- Egy taszk lemond a futás jogáról,
- Megszakítás érkezik (tick esemény, külső megszakítás),
- Egy tasz létrejött vagy befejeződött.

A taszkváltási mód szerint kétféle működést különböztetünk meg:
- Preemptív ütemezés: ekkor az ütemező megszakíthatja az éppen futó taszkot, amennyiben magasabb prioritású taszk futásra kész állapotban várakozik,
- Nem-preemptív vagy kooperatív ütemezés: ebben az esetben az ütemező csak akkor futtat új taszkot, ha az éppen futó taszk befejeződött vagy explicit lemond a futásról (blokkolódik vagy átadja a futás jogát).

A továbbiakban ismertetem néhány elterjedt ütemezési mechanizmus alapelvét.

#### First-come first-served (Kiszolgálás beérkezési sorrendben)

A taszkok futtatása az érkezési sorrendben történik. A beérkező taszkok egy sorba kerülnek, ahonnan sorrendben kapják meg a CPU használat jogát.

Az átlagos várakozási idő nagy lehet, ha egy időigényes folyamatra több gyors lefutású folyamat várakozik.


#### Shortest Job First (Legrövidebb feladat először)

A várhatóan leggyorsabban lefutó taszk kerül futási állapotba ütemezés bekövetkezésekor. Helyes működés esetén az átalgos várakozási idő optimális lesz.

A gyakorlatban nehéz előre megjósolni egy taszk futási idejét.


#### Prioritásos ütemezés

A következő taszk kiválasztása a prioritása alapján történik. Növelni lehet a hatákonyságát más metódusok keverésével (például a prioritást a várható futási idejéből határozzuk meg; a prioritást növeljük az idő elteltével; a prioritást csökkentjük az eddigi futó állapotban töltött idő arányában).

Rossz tervezés esetén az alacsony prioritású feladatok nem jutnak processzoridőhöz (kiéheztetés - starvation).

#### Round Robin

Időosztáson alapuló mechanizmus. A várakozási sor egy cirkuláris buffer. Minden taszk adott időszeletet kap, majd az időszelet lejártával a sorban következő taszk kapja meg a futás jogát.


### Hibrid ütemezés

A felsorolt ütemezési elveket akár keverve is lehet alkalmazni. Ilyen ütemezési mechanizmus például a többszintű sorok használata, ahol minden sor saját ütemezési algoritmussal rendelkezik, és egy külön algoritmus felel az egyes sorok arbitrációjáért.

[Kép]


### Operációs rendszer által nyújtott szolgáltatások

Az operációs rendszer felelős az egyes taszkoknak szükséges memóriaterületek kezeléséért és a taszkok közötti kommunikáció megvalósításáért.


#### Memóriakezelés

Egy taszk létrehozásakor az operációs rendszer ossza ki a taszk számára a használható memóriaterület helyét és méretét, és ezt az információt a rendszernek tárolnia kell. Ha az adott taszk befejezi a futását (megszűnik), akkor az operációs rendszer feladata a taszkhoz tartozó memóriaterület felszabadítása is.

 Amennyiben a taszk dinamikusan allokál memóriát futás közben, úgy ezen memória kezelése szintén az operációs rendszer feladatkörébe tartozik, viszont az egyszerűbb operációs rendszerek esetében a futás közben lefoglalt memória felszabadítása a taszk felelősége.

#### Atomi művelet

Bizonyos műveletek során szükség van a műveletsor szigorúan egymás utáni futtatására. Ilyen eset például a megosztott adatterületre való írás, amikor ha taszkváltás következik be az adatterület írása közben, akkor a tartalmazott adatt érvénytelen értéket vehet fel. Az ilyen, _oszthatatlan_ műveletek nevezzük atomi műveleteknek. 

A konzisztencia biztosítása céljából az atomi műveleteket _kritikus szakaszokba_ kell ágyazni, ezzel jelezve az operációs rendszernek, hogy a műveletek végrehajtása alatt nem következhet be taszkváltás, bizonyos esetekben megszakítás sem.

Az operációs rendszerek a kritikus szakaszokat több szinten megvalósíthatják. Legszigorúbb esetben a megszakítások letiltásra kerülnek, és az ütemező a kritikus szakasz befejezéséig felfüggesztett állapotban van. A kritikus szakasz megvalósításának egy kevésbé drasztikus módja az ütemező letiltása. Ekkor a kódrészlet védett a más taszkok általi preemtálástól, viszont a megszakítások nem kerülnek letiltásra.

A kritikus szakaszt a lehető leggyorsabban el kell hagyni, mert különben a beérkező megszakítások és magasabb prioritású taszkok késleltetést szenvednek, ami rontja az alkalmazás hatékonyságát. 


#### Kommunikációs objektumok

Az alkalmazások egymástól független taszkok konstrukciójából áll. Viszont ezeknek a taszkoknak ahhoz, hogy a feladatukat el tudják látni, gyakran kommunikálniuk kell egymással.

A felsorolt kommunikációs objektumok listája nem teljes. Bizonyos operációs rendszerek ezektől eltéró struktúrákat is használhatnak, és az itt felsorolt struktúrák implementációja is eltérhet a leírtaktól (természetesen az sem biztos, hogy implementálva van az adott objektum).


##### Szemafor

Két szemafor típust különböztetünk meg:
- Bináris szemafor, amikor a szemafor két értéket vehet fel,
- Számláló szemafor, mikor a szemafor több állapotot is felvehet.

A szemaforon két művelet értelmezett:
- A szemafor jelzése (elterjedt elnevezések: give, signal, post, V() művelet[lábjegyzet-> Dijkstra holland, verhogen]), amikor a szemafor értéke növelésre kerül.
- A szemafor elvétele (elterjedt elnevezések: take, wait, pend, P() művelet [lábjegyzet-> proberen]), amikor a szemafor először tesztelésre kerül, hogy tartalmaz-e elemet, ha igen, akkor az értékét csökkentjük, ha nem, akkor várakozunk addig, amíg valamelyik másik taszk elérhetővé nem teszi azt.

A szemafor látszólag egyszerűen helyettesíthető egy egyszerű változó (boolean vagy előjel nélküli egész) használatával, viszont a beépített szemafor struktúra atomi műveletként kerül kezelésre (így nem következik be ütemezés a szemafor tesztelése és állítása közben), illetve a rendszer automatikusan tudja kezelni a várakozó folyamatok állapotok közti mozgatását.

Szemaforokat leggyakrabban szinkronizációs célból, vagy erőforrások védelmére használnak.


##### Bináris szemafor

Bináris szemafor esetén a szemafor két értéket vehet fel. Felfogható úgy is, mint egy egy adat tárolására (egy elem hosszú) sor, melynek nem vizsgáljuk a tartalmazott értékét, csak azt, hogy éppen tartalmaz-e adatot vagy sem.

Leggyakoribb felhasználása a taszkok szinkronizálása. Ekkor az egyik taszk a futásának egy adott pontján várakozik egy másik taszk jelzésére. Ezzel a módszerrel megvalósítható a megszakítások taszkokban történő kezelése, ezzel is minimalizálva a megszakítási rutin hosszát.

Bináris szemafor használatakor különös figyelmet kell fordítani arra, hogy ha a szemafor egy adott taszkban gyakrabban kerül jelzésre, mint ahogy feldolgozzuk, akkor jelzések veszhetnek el (amíg az egyik jelzés várakozik, addig az utána következő eseményeknek nincs lehetőségük várakozó állapotba kerülni).


##### Számláló szemafor

A számláló típusú szemafor minden jelzéskor növeli az értékét. Ekkor (amíg el nem éri a maximális értékét) nem kerül Blokkolt állapotba a jelző taszk. A számláló szemafor felfogható úgy, mint egy egynél több adat tárolására képes sor, melynek nem vizsgáljuk az értékét, csak azt, hogy éppen tartalmaz-e még adatot vagy sem.

Két felhasználása elterjedt a számláló szemaforoknak:
- Események számlálása: ekkor minden esemény hatására növeljük a szemafor értékét (új elemet helyezünk a sorba). A szemafor aktuális értéke a beérkezett és a feldolgozott események különbsége. A számlálásra használt szemafor inicializálási értéke nulla.
- Erőforrás menedzsment: ekkor a szemafor értéke a rendelkezésre álló erőforrások számát mutatja. Mikor az operációs rendszertől az erőforrást igényeljük, akkor a szemafor értékét csökkentjük, mikor felszabadítjuk a birtolkolt erőforrást, akkor a szemafor értékét növeljük. Ha a szemafor értéke nulla, akkor nincs rendelkezésre álló erőforrás. Erőforrások kezelésére használt számláló szemafor esetén az inicializálási érték az elérhető erőforrások száma.


##### Mutex

Taszkok vagy taszkok és megszakítási rutinok között megosztott erőforrás kezelésekor a Mutex (kölcsönös kizárás) használata indokolt. Mikor egy taszk vagy megszakítás hozzáférést indít egy erőforráshoz, akkor a hozzá tartozó mutex-et elkéri. Ha az erőforrás szabad, akkor az igénylő taszk megkapja a kezelés jogát, és mindaddig megtartja, amíg be nem fejezi az erőforrással való munkát. A mutex-et a lehető legkorábban (az erőforrással való munka befejeztével) fel kell szabadítani, ezzel is csökkentve az esetleges holtpont kialakulásának veszélyét. 

Látható, hogy a mutex nagyon hasonlít a bináris szemaforhoz. A különbség abból adódik, hogy mivel a bináris szemafort leggyakrabban szinkronizációra használjuk, ezért azt nem kell felszabadítani: a jelző taszk vagy megszakítás jelzést ad a szemforon keresztül a feldolgozó taszknak. A feldolgozó taszk elveszi a szemafort, de a feldolgozás befejeztével a szemafort nem adja vissza. 


##### Sor (Queue)

A sorok fix méretű adatból tudnak véges számú üzenetet tárolni. Ezek a jellemzők a sor létrehozásakor kerülnek meghatározásra. Alapértelmezetten FIFO-ként működnek.

A sorba való írás során másolat készül az eredeti változóról, és ez a másolat kerül tárolásra a sorban.


##### Üzenet (Message)

Az üzenet három dolgot tartalmaz:
- Az adatra mutató pointert,
- A mutatott adat méretét,
- Egy időbélyegzőt, ami megmutatja, mikor került az pointer a tárolóba.

A pointer mutathat egy egész adatterületre vagy akár függvényre is. A küldő és fogadó félnek tudnia kell, hogy az üzenet tartalma mire mutat.

Mivel a kommunikáció során csak az adat referenciája kerül átvitelre, ezért különös figyelmet kell fordítani az adat konzisztenciájára.


##### Események (Event flag)

Az események taszkok közötti szinkronizációra szolgálnak. Megvalósítható diszjunktív szinkronizáció, amikor több esemény küzül bármelyik bekövetkezése esetén a taszk futásra kész állapotba kerül (logikai VAGY), és megvalósítható konjuktív szinkronizáció, amikor az események mindegyikének bekövetkezése feltétele a taszk futásának (logikai ÉS).


### Operációs rendszer használata esetén felmerülő problémák

Operációs rendszer használatával bizonyos problémák is felmerülhetnek egy alkalmazás fejlesztése során. Ezek közül látunk példákat a továbbiakban.


#### Kiéheztetés (Starving)

Prioritásos ütemezés esetén, ha egy magas prioritású taszk folyamatosan futásra kész állapotban van, akkor az alacsony prioritású taszkok sosem kapnak processzoridőt. Ezt a jelenséget nevezzük kiéheztetésnek (starving), amit átgondolt tervezéssel könnyedén elkerülhetünk.


#### Prioritás inverzió

Vegyünk egy esetet, mikor legalább három különböző prioritási szinten futó taszkot hozunk létre. A legalacsonyabb prioritásútól a magasabb felé haladva nevezzük őket _TaskA_-nak, _TaskB_-nek és _TaskC_-nek.

Vegyünk egy esetet, mikor legalább három, különböző prioritási szinten futó taszkot hozunk létre. Kezdetben csak a _TaskA_ képes futni, ami egy mutex segítségével megkapja egy erőforrás használati jogát. Közben a _TaskC_ futásra kész éllapotba kerül, ezért preemptálja a _TaskA_-t. A _TaskC_ is használná az erőforrást, de mivel azt már a _TaskA_ birtokolja, ezért várakozó állapotba kerül. Közben a _TaskB_ is futásra kész állapotba került, és mivel magasabb a prioritása, mint a _TaskA_-nak, ezért megkapja a futás jogát. A _TaskA_ csak a _TaskB_ befejeződése (vagy blokkolódása) esetén kerül újra futó állapotba. Miután a _TaskA_ befejezte az erőforrással a feladatait és felszabadítja azt, a _TaskC_ újból futásra kész állapotba kerül, és preemptálja a _TaskA_-t.

[Kép]

A vizsgált példa során a _TaskB_ késleltette a _TaskC_ futását azzal, hogy nem engedte a _TaskA_-nak az erőforrás felszabadítását. Így látszólag a _TaskB_ magasabb prioritással rendelkezett, mint _TaskC_. Erre mondjuk, hogy prioritás inverzió lépett fel.

A prioritás inverzió problémájának egy megoldása a prioritás öröklés. Ekkor a magad prioritású taszk a saját prioritási szintjére emeli azt az alacsony prioritású taszkot, mely blokkolja a további futását. Amint a szükséges erőforrás felszabadul, az eredeti prioritási értékek kerülnek visszaállításra.

[Kép]


#### Holtpont (Deadlock)

Szemaforok és mutexek használata során alakulhat ki holtponti helyzet. Nézzük azt az esetet, hogy van két, azonos prioritású taszk (a taszkok prioritása itt nem lényeges), melyek működésük során ugyan azt a két erőforrást használják. A két taszkot nevezzük _TaskA_-nak és _TaskB_-nek, a két erőforrást pedig _ResA_-nak és _ResB_-nek.

Induláskor a _TaskA_ kapja meg a futás jogát, és lefoglalja a _ResA_-t. Közben lejár a _TaskA_-nak kiosztott időszelet, és _TaskB_ kerül futó állapotba. A _TaskB_ lefoglalja a _ResB_ erőforrást, majd megpróbálja lefoglalni a _ResA_ erőforrást is. Mivel a _ResA_-t már a _TaskA_ használja, ezért a _TaskB_ várakozó állapotba kerül. A _TaskA_ újból megkapja a processzort, és hozzáférést kezdeményez a _ResB_ erőforráshoz. Mivel a _ResB_ erőforrást a _TaskB_ folyamat birtokolja, ezért a _TaskA_ is várakozó állapotba lép. Egyik folyamat sem tudja folytatni a feladatát, emiatt az erőforrásokat sem tudják felszabadítani.

[Kép]

A holtponti helyzetek elkerülésére és feloldására több szabály létezik, de beágyazott rendszereknél átgondolt tervezéssel, illetve időkorlát megadásával általában elkerülhető a kialakulásuk.


#### Újrahívható függvények (Reentrant functions)

Egy függvény reentráns (újrahívható), ha biztonságosan meghívható több különböző taszkból vagy megszakításból.

Minden taszk rendelkezik saját stack-kel és saját regiszterekkel. Ha egy függvény minden adatot a stack-jén vagy a regisztereiben tárol (vagyis nem használ globális és statikus változókat), akkor a függvény reentráns.


-------------------------------------------------------------------------------

# Források

- [Definíciók](http://www.freertos.org/about-RTOS.html)
- [Scheduling](https://cseweb.ucsd.edu/classes/fa02/cse120/lectures/sched.pdf)
- [Multi-level queues](http://web.cs.wpi.edu/~cs3013/c07/lectures/Section05-Scheduling.pdf)
- uC/OS-III Users Guide
- FreeRTOS Practical Guide (Function Reentracy)