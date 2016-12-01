# FreeRTOS


## Bevezetés

A FreeRTOS a Real Time Engineers Ltd. által fejlesztett valós idejű operációs rendszer. A fejlesztők céljai között volt a rendszer erőforrásigényének minimalizálása, hogy a legkisebb beágyazott rendszereken is futtatható legyen. Ebből adódóan csak az alap funkciók vannak megvalósítva, mint ütemezés, taszkok közötti kommunikáció lehetősége, memóriamenedzsment, de nincs beépített támogatás hálózati kommunikációra vagy bármiféle külső hardver használatára (ezeket vagy nekünk meg megírnunk, vagy harmadik féltől származó kódot kell használnunk).

A rendszer módosított GPLv2 licencet használ. A licencmódosítás lehetővé teszi GPL-től eltérő licenccel ellátott modulok használatát, amennyiben azok a FreeRTOS-sal kizárólag a FreeRTOS API-n keresztül kommunikálnak.


## Taszkok

A FreeRTOS nem korlátozza a létrehozható taszkok és prioritások számát, amíg a rendelkezésre álló memória lehetővé teszi azok futtatását. A rendszer lehetőséget biztosít ciklikus és nem ciklikus taszkok futtatására egyaránt.

A beágyazott rendszerek döntő része egymagos processzorokat használ, amiből az következik, hogy egyszerre csak egy taszk futhat. A taszkok eszerint két nagy csoportba oszthatóak: éppen futó taszk (___Fut___ állapot), illetve az összes többi (___Nem fut___ állapot).

![freertos_states](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/FreeRTOS/01_FreeRTOS_states.png "FreeRTOS states simplified")

Annak, hogy egy taszk éppen miért nem fut, több oka lehet. Ez alapján a __Nem fut__ állapot több állapotra felosztható.
Ha egy taszk képes lenne futni, de például egy nagyobb prioritású taszk birtokolja a processzort, akkor a taszk állapota ___Futásra kész___. Ha a taszk valamilyen eseményre vár (időzítés, másik taszk szinkronizáló jele), akkor a taszk ___Blokkolt___ állapotban van. Az operációs rendszer lehetőséget ad arra, hogy a taszkokat függvényhívással ___Felfüggesztett___ állapotba kényszerítsük. Ekkor egy másik függvényhívással tudjuk visszahozni az ütemezendő feladatok sorába a taszkot.

A taszkok önként lemondhatnak a futásról (időzítés, szinkronizáció, taskYIELD() függvény hívása), viszont futó állapotba csak az ütemező helyezheti.

![freertos_states_expanded](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/FreeRTOS/02_FreeRTOS_states_expanded.png "FreeRTOS states expanded")


### Idle taszk

A processzor folyamatosan utasításokat hajt végre működése közben (eltekintve a különböző energiatakarékos üzemmódoktól), ezért legalább egy taszknak mindig Futásra kész állapotban kell lennie. Hogy ez biztosítva legyen, az ütemező indulásakor automatikusan létrejön egy ciklikus ___Idle taszk___.

Az Idle taszk a legkisebb prioritással rendelkezik, így biztosítva, hogy elhagyja a Fut állapotot, amint egy magasabb prioritású taszk Futásra kész állapotba kerül.

Taszk törlése esetén az Idle taszk végzi el a különböző erőforrások felszabadítását. Mivel a FreeRTOS nem biztosít védelmet egy taszk _kiéheztetésével_ szemben ezért fontos, hogy az alkalmazás tervezésekor biztosítsunk olyan időszeletet, amikor másik, nagyobb prioritású taszk nem fut.


#### Idle hook függvény

Előfordulhat, hogy az alkalmazásunkban olyan funkciót szeretnénk megvalósítani, amelyet az Idle taszk minden egyes iterációjára le kell futtatni (például teljesítménymérés érdekében). Ezt a célt szolgálja az ___Idle hook__ függvény, ami az Idle taszk minden lefutásakor meghívódik.

Az Idle hook általános felhasználása:
- Alacsony prioritású háttérfolyamat, vagy folyamatos feldolgozás,
- A szabad processzoridő mérése (teljesítménymérés),
- Processzor alacsony fogyasztású üzemmódba váltása.


##### Korlátozások az Idle hook függvénnyel kapcsolatban

1. Az Idle hook függvény nem hívhat blokkoló vagy felfüggesztést indító függvényeket[lábjegyzet->A hook függvény blokkolása esetén felléphet az az eset, hogy nincs Futásra kész állapotban levő taszk],
2. Ha az alkalmazás valahol töröl egy taszkot, akkor az Idle hook függvénynek elfogadható időn belül vissza kell térnie. [lábjegyzet->Az Idle taszk felelős a kernel erőforrások felszabadításáért, ha egy taszk törlésre kerül. Ha az Idle taszk bentragad az Idle hook-ban, akkor ez a tisztítás nem tud bekövetkezni.]


## Ütemező

A FreeRTOS által használt ütemezési mechanizmust Fix Prioritásos Preemptív Ütemezésnek hívjuk[lábjegyzet->A FreeRTOS kooperatív ütemezést is támogat, viszont a valós idejű futás eléréséhez a preemptív ütemezés szükséges, ezért a továbbiakban csak a preemptív ütemezéssel foglalkozunk.]. _Fix prioritásos_, mivel a rendszer magától nem változtatja a prioritásokat, _preemptív_, mert egy taszk Futásra kész állapotba lépésekor preemtálja az éppen futó taszkot, ha a futó taszk prioritása alacsonyabb.

[ToDo -> Itt átfogalmazni kicsit a dolgokat. Elég kusza a gondolatmenet.]

A taszkok lehetnek Blokkolt állapotban, ahonnan egy esemény bekövetkezését követően automatikusan Futásra kész állapotba kerülnek.

Időbeli események azok, amik egy bizonyos időpillanatban következnek be (például egy késleltetési idő letelik). Az időbeli eseményeket kihasználva lehetőség nyílik periodikus futtatásra, vagy időtúllépés detektálására.

Szinkronizáló események azok, amikor egy taszk vagy megszakításkezelő rutin jelzést küld valamilyen kommunikációs struktúrán keresztül egy másik taszknak. Tipikusan aszinkron jelzésre használjuk, mint például adat érkezését egy periférián keresztül.


### Taszkok prioritásának meghatározása

[ToDo -> Itt átfogalmazni kicsit a dolgokat. Elég kusza a gondolatmenet.]

Ökölszabályként alkalmazható, hogy a hard real-time funkciókat magasabb prioritással látjuk el, mint a soft real-time funkciókat.

Rate Monotonic Scheduling (Gyakoriság Monoton Ütemezés) az a gyakran alkalmazott prioritás-hozzárendelés, amikor minden taszkhoz egyedi prioritásszintet rendelünk a végrehajtási gyakoriság függvényében. A gyakrabban lefutó folyamatokhoz magasabb, a ritkábban lefutó folyamatokhoz alacsonyabb prioritást állítunk be.


## Kommunikációs objektumok

[ToDo -> Átfogalmazni. Ne legyen nagyon redundáns az átlalános résszel.]

Az alkalmazások egymástól független taszkok konstrukciójából áll. Viszont ezeknek a taszkoknak ahhoz, hogy a feladatukat el tudják látni, gyakran kommunikálniuk kell egymással.
A FreeRTOS három alap kommunikációs struktúrát kínál a felhasználónak:
- sor (__queue__),
- szemafor (__semaphore__),
    - bináris (__binary semaphore__),
    - számláló (__counting semaphore__),
- mutex (__mutex__ [<b>MUT</b>ual <b>EX</b>cluison]).


### Sorok (queues)

A sorok fix méretű adatból tudnak véges számú üzenetet tárolni. Ezek a jellemzők a sor létrehozásakor kerülnek meghatározásra. Alapértelmezetten FIFO-ként működnek (First In First Out), bár a FreeRTOS lehetővé teszi a sor elejére való írást is.
A sorba való írás során másolat készül az eredeti változóról, és ez a másolat kerül tárolásra a sorban. Olvasáskor a paraméterként átadott memóriacímre kerül átmásolásra, ezért figyelni kell arra, hogy az átadott változó által elfoglalt memória legalább akkora legyen, mint a sor által tartalmazott adattípus mérete. Amennyiben a továbbítandó adattípus már nagynak tekinthető, akkor érdemes a memóriára mutató pointereket elhelyezni a sorban, ezzel csökkentve a RAM kihasználtságát (ekkor viszont különösen figyelni kell arra, hogy a kijelölt memóriaterület tolajdonosa egyértelmű legyen, vagyis ne történjen különböző taszkokból módosítás egyidőben, illetve biztosítani kell a memóriaterüet érvényességét). A FreeRTOS API kétféle függvényt használ olvasásra:
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

Taszkok vagy taszkok és megszakítási rutinok között megosztott erőforrás kezelésekor a Mutex (kölcsönös kizárás) használata indokolt. Mikor egy taszk vagy megszakítás hozzáférést indít egy erőforráshoz, akkor a hozzá tartozó mutex-et elkéri. Ha az erőforrás szabad, akkor az igénylő taszk megkapja a kezelés jogát, és mindaddig megtartja, amíg be nem fejezi az erőforrással való munkát. A mutex-et a lehető legkorábban (az erőforrással való munka befejeztével) fel kell szabadítani, ezzel is csökkentve az esetleges holtpont kialakulásának veszélyét.

Látható, hogy a mutex nagyon hasonlít a bináris szemaforhoz. A különbség abból adódik, hogy mivel a bináris szemafort leggyakrabban szinkronizációra használjuk, ezért azt nem kell felszabadítani: a jelző taszk vagy megszakítás jelzést ad a szemforon keresztül a feldolgozó taszknak. A feldolgozó taszk elveszi a szemafort, de a feldolgozás befejeztével a szemafort nem adja vissza.

A felhasználásból adódó különbségek miatt a mutex védett a prioritás inverzió problémájával szemben, míg a bináris szemafor implementációjából hiányzik.[lábjegyzet-> a FreeRTOS prioritás öröklési mechanizmusa csak egyszerű implementációt tartalmaz, és feltételezi, hogy csak egy taszk csak egy mutex-et birtokol egy adott pillanatban.]


## Megszakítás-kezelés

Beágyazott rendszereknél gyakran kell a környezettől származó eseményekre reagálni (például adat érkezése valamely kommunikációs interfészen). Az ilyen események kezelésekor a megszakítások alkalmazása gyakran elengedhetetlen.

Megszakítás használata esetén figyelni kell arra, hogy a megszakítási rutinokban csak _FromISR_-re végződő API függvényeket hívhatunk. Ellenkező esetben nem várt működés következhet be (blokkoljuk a megszakítási rutint, ami az alkalmazás fagyásához vezethet; kontextus-váltást okozunk, amiből nem térünk vissza, így a megszakítási rutinból sosem lépünk ki, stb.).

A FreeRTOS ütemezője a (STM32-re épülő rendszerekben) a SysTick interruptot használja az ütemező periodikus futtatásához. A megszakítási rutin futása közben emiatt nem történik ütemezés. Amennyiben valamely magasabb prioritású taszkunk a megszakítás hatására Futásra kész állapotba kerül, akkor vagy a következő ütemezéskor kapja meg a processzort, vagy explicit függvényhívással kell kérni az operációs rendszer az ütemező futtatására.

Az alacsonyabb prioritású megszakítások szintén nem tudnak érvényre jutni, így azok bekövetkezéséről nem kapunk értesítést (az első beérkező, alacsonyabb prioritású megszakítás jelző bitje bebillen az esemény hatására, de amennyiben több is érkezik a magasabb prioritású megszakítási rutin futása alatt, úgy azok elvesznek).
Az említett problémák végett a megszakítási rutint a lehető legrövidebb idő alatt be kell fejezni.

### Késleltetett megszakítás-kezelés

A megszakítási rutint a lehető legrövidebb idő alatt el kell hagyni, emiatt célszerű a kevésbé fontos műveleteket egy kezelő taszkban megvalósítani. A FreeRTOS a szemaforokon keresztül biztosít lehetőséget a megszakítás és taszk szinkronizációjára.

A megszakítás hatására a megszakítási rutinban csak szükséges lépéseket végezzük el (például eseményjelző bitek törlése), majd egy szemaforon keresztül jelzi a feldolgozó taszknak az esemény bekövetkeztét. Ha a feldolgozás időkritikus, akkor a feldolgozó szálhoz rendelt magas prioritással biztosítható, hogy az éppen futó taszkot preemtálja. Ekkor a megszakítási rutin végén az ütemező meghívásával a visszatérés utána azonnal feldolgozásra kerül az esemény.

Az eseményt kezelő taszk blokkoló _take_ utasítással várakozik Blokkolt állapoban a szemafor érkezésére. A jelzés hatására Futásra kész állapotba kerül, ahonnan az ütemező (az éppen futó taszkot preemtálva) Futó állapotba mozgatja. Az esemény feldolgozását követően újra meghívja a blokkoló _take_ utasítást, így újból Blokkolt állapotba kerül az újabb esemény bekövetkezéséig.

Todo: Példa

![deferred_interrupt](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/FreeRTOS/03_deferred_interrupt.png "Deferred interrupt")


### Interruptok egymásba ágyazása

Az STM32 mikrokontrollerek lehetővé teszik prioritások hozzárendelését a megszakításokhoz. Ezáltal létrejöhet olyan állapot, amikor egy magasabb prioritású megszakítás érkezik egy alacsonyabb szintű megszakítási rutin futása közben. Ekkor a magasabb prioritású megszakítás késleltetés nélkül érvényre jut, és a magasabb prioritású megszakítási rutin befejeztével az alacsony prioritású folytatódik.

A FreeRTOS rendszer konfigurációs fájlában (_FreeRTOSConfig.h_) beállítható azon legmagasabb prioritás, amihez a FreeRTOS hozzáférhet. Ezáltal a megszkaításoknak két csoportja adódik:
- A FreeRTOS által kezelt megszakítások,
- A FreeRTOS-tól teljesen függetlem megszakítások.

A FreeRTOS által kezelt prioritások kritikus szakaszba lépéskor letiltásra kerülnek, így azok nem szakíthatják meg az atomi utasításként futtatandó kódrészletet. Viszont a megszakítások ezen csoportja használhatja a _FromISR_-re végződő FreeRTOS API függvényeket.

A FreeRTOS-tól független megszakítások kezelése teljes mértékben a fejlesztő feladata. Az operációs rendszer nem tudja megakadályozni a futásukat, így a kritikus szakaszban is képesek futni. A kiemelkedően időkritikus kódrészleteket célszerű ezekben a megszakítási turinokban megvalósítani (például motorvezérlés). A FreeRTOS-tól független megszakítási rutinokban nem szabad API függvényeket használni!


## Erőforrás-kezelés

Multitaszk rendszerek esetén fennáll a lehetősége, hogy egy taszk kikerül a Futó állapotból még mielőtt befejezné egy erőforrással a műveleteket. Ha az erőforrást egy másik taszk is használni akarja, akkor inkonzisztencia léphet fel. Tipikus megjelenései a problémának:
- Periféria elérése,
- Egy közös adat olvasása, módosítása, majd visszaírása,[lábjegyzet-> magas szintű nyelv esetén (pl C) ez látszólag lehet egy utasítás, viszont a fordító által előállított gépi kód több utasításból áll]
- Változó nem atomi elérése (például több tagú struktúra értékeinek megváltoztatása),
- Nem reentráns függvények, [lábjegyzet-> reentráns függvény]

Az adat inkonzisztencia elkerüléséhez hasnzálhatunk mutex-et. Amikor egy taszk megkapja egy erőforrás kezelésének jogát, akkor más taszk nem férhet hozzá, egészen addig, amig a birtokló taszk be nem fejezte az erőforrással a feladatát, és az erőforrás felszabadítását mutex-en keresztül nem jelezte.


### Kritikus szakasz

[ToDo -> Itt átfogalmazni kicsit a dolgokat. Elég kusza a gondolatmenet.]

A közös erőforrások használatakor gyakran szükség van egy adott műveletsor atomivá tételére, azaz arra, hogy a kijelölt műveletek futását semmi ne szakíthassa meg, látszólag egy utasításként fussanak le.

A FreeRTOS támogatja a kritikus szakaszok használatát a __taskENTER_CRITICAL()__ és __taskEXIT_CRITICAL()__ makrók használatával.

A kritikus szakaszokat minden probléma nélkül egymásba lehet ágyazni, mivel a rendszerkernel nyílvántartja, hogy milyen mélyen van az alkalmazás a kritikus szakaszokban. A rendszer csak akkor hagyja el a kritikus szakaszt, ha a számláló nullára csökken, vagyis ha minden __taskENTER_CRITICAL()__ híváshoz tartozik egy __taskEXIT_CRITICAL()__ is.

A kritikus szakaszt a lehető leggyorsabban el kell hagyni, különben a beérkező megszakítások válaszideje nagy mértékben megnőhet.


### Ütemező felfüggesztése

A kritikus szakasz megvalósításának egy kevésbé drasztikus módja az ütemező letiltása. Ekkor a kódrészlet védett a más taszkok általi preemtálástól, viszont a megszakítások nem kerülnek letiltásra. Hátránya, hogy az ütemező elindítása hosszabb időt vehet igénybe.


### Gatekeeper taszk

A gatekeeper taszk alkalmazása a kölcsönös kizárás egy olyan megvalósítása, mely működésénél fogva védett a prioritás inverzió és a holtpont kialakulásával szemben.

A gatekeeper taszk egyedüli birtokosa egy erőforrásnak, így csak a taszk tudja közvetlenül elérni az erőforrást, a többi taszk közvetetten, a gatekeeper taszk szolgáltatásain keresztül tudja használni az erőforrást.

Amikor egy taszk használni akarja az erőforrást, akkor üzenetet küld a gatekeeper taszknak (általában sor használatával). Mivel egyedül a gatekeeper taszk jogosult elérni az erőforrást, és nincs szükség explicit mutex használatára.

A gatekeeper taszk Blokkolt állapotban vár, amíg nem érkezik üzenet a sorba. Az üzenet beérkezése után elvégzi a megfelelő műveleteket az erőforráson, majd ha kiürült a sor, akkor ismét Blokkolt állapotba kerül.

A megszakítások probléma nélkül tudják használni a gatekeeper taszkok szolgáltatásait, mivel a sorba való írás támogatott megszakítási rutinból is.


## Memória-kezelés

Beágyazott alkalmazások fejlesztése során is szükség van dinamikus memóriafoglalásra. Az asztali alkalmazásoknál megszokott __malloc()__ és __calloc()__ függvények több szempontból sem felelnek meg mikrokontrolleres alkalmazásokban:
- Kisebb rendszerekben nem biztos, hogy elérhető,
- Az implementációjuk sok helyet foglalhat,
- Nem determinisztikus a lefutásuk; a végrehajtási idő különbözhet különböző híváskor,
- Memóriatöredezettség léphet fel.

Minden taszkhoz tartozik egy TCB (Task Control Block) és egy stack. A TCB struktúra a következő elemeket tartalmazza (a teljesség igénye nélkül):

[Kép]

|      Változó      |   Jelentés   |
|:-----------------:|:------------:|
|   pxTopOfStack    | Az stack utolsó elemére mutató pointer|
| xGenericListItem  | A FreeRTOS a TCB ezen elemét helyezi az adott állapothoz tartozó listába (nem magát a TCB-t) |
|  xEventListItem   | A FreeRTOS a TCB ezen elemét helyezi az adott eseményhez tartozó listába (nem magát a TCB-t) |
|    uxPriority     | A taszk prioritása |
|      pxStack      | A stack kezdetére mutató pointer |
|    pcTaskName     | A taszk neve. Kizárólag debug célokra |
|   pxEndOfStack    | A stack végére mutató pointer a stack túlcsordulásának detektálására |
|  uxBasePriority   | Az utojára taszkhoz rendelt prioritás. Mutex használata esetén a prioritás öröklés során megnövelt prioritás visszaállítására |
| ulRunTimeCounter  | A taszk __Fut__ állapotban töltött idejét tárolja futási statisztika készítéséhez |

Az egyes alkalmazások különböznek memória-allokációs igényükben és az erőírt időzítési korlátokban, ezért nincs olyan memória-allokációs séma, amely minden alkalmazásban megállná a helyét. A FreeRTOS több allokációs algoritmust is a fejlesztő rendelekezésére bocsát, amiből az alkalmazásnak megfelelő kiválasztásával lehet elérni a megfelelő működést.


### Heap_1.c

Kisebb beágyazott alkalmazásoknál gyakran még az ütemező indulása előtt létrehozunk minden taszkot és kommunikációs objektumot. Ilyenkor elég a memóriát lefoglalni az alkalmazás indulása előtt, és a futás alatt minden memória lefoglalva marad. Ez azt is jelenti, hogy nem kell komplex algoritmusokat megvalósítani a determinisztikusság biztosítására és a Memóriatöredezettség ellkerülésére, hanem elég a kódméretet és az egyszerűséget szem előtt tartani.

Ezt az implementációt tartalmazza a __heap1.h__. A fájl a __pvPortMalloc()__ egyszerű megvalósítását tartalmazza, azonban a __pvPortFree()__ nincs implementálva. A __heap_1.c__ nem fenyegeti a rendszer determinisztikusságát.

A __pvPortMalloc()__ függvény a FreeRTOS heap-jét ossza fel kisebb területekre, majd ezeket rendeli hozzá az egyes taszkokhoz. A heap teljes méretét a __configTOTAL_HEAP_SIZE__ konfigurációs érték határozza meg a __FreeRTOSConfig.h__ fájlban. Nagy méretű tömböt definiálva már a memórifoglalás előtt látszólag sok memóriát fog felhasználni az alkalmazás, mivel a FreeRTOS ezt induláskor lefoglalja.


### Heap_2.c

A __heap_2.c__ szintén a __configTOTAL_HEAP_SIZE__ konfigurációs értéket használja, viszont a __pvPortMalloc()__ mellett már implementálva van a __pvPortFree()__ is. A memóriafoglalás során a legjobban illeszkedő területből oszt ki a taszk számára memóriát.

A legjobban illeszkedő (best fit) algoritmus biztosítja, hogy a memóriakérés a hozzá méretben legközelebb eső, elgendő nagyságú blokkból legyen kiszolgálva.

A megvalósítás nem egyesíti a szomszédos szabad területeket egy nagyobb egységes blokkba, így töredezettség léphet fel. Ez nem okoz gondot, ha a lefoglalt és felszabadított memória mérete nem változik.

A __heap_2.c__ fájl használata javasolt, ha az alkalmazás ismételve létrehoz és töröl taszkokat, és a taszkokhoz tartozó stack mérete nem változik.

A __heap_2.c__ működése nem determinisztikus, de hatékonyabb, mint a _standard library_ implementációi.


### Heap_3.c

A __heap_3.c__ a _standard library_ függvényeit használja, de a függvények alatt felfüggeszti az ütemező működését, ezzel elérve, hogy a memória-kezelés thread-safe legyen.

A heap méretét nem befolyásolja a __configTOTAL_HEAP_SIZE__ érték, ehelyett a linker beállításai határozza meg.



# Hivatkozások

- [FreeRTOS TCB](http://www.aosabook.org/en/freertos.html)
