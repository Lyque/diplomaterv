# FreeRTOS


## Bevezetés

A FreeRTOS a Real Time Engineers Ltd. által fejlesztett valós idejű operációs rendszer. A fejlesztők céljai között volt a rendszer erőforrásigényének minimalizálása, hogy a legkisebb beágyazott rendszereken is futtatható legyen. Ebből adódóan csak az alap funkciók vannak megvalóstva, mint ütemezés, taszkok közötti kommunikáció lehetősége, memóriamenedzsment, de nincs beépített támogatás hálózati kommunikációra vagy bármiféle külső hardver használatára (ezeket vagy nekünk meg megírnunk, vagy harmadik féltől származó kódot kell használnunk).

A rendszer módosított GPLv2 licencet használ. A licencmódosítás lehetővé teszi GPL-től eltérő licenccel ellátott modulok használatát, amennyiben azok a FreeRTOS-sal kizárólag a FreeRTOS API-n keresztül kommunikálnak. 


## Taszkok

A FreeRTOS nem korlátozza a létrehozható taszkok és prioritások számát, amíg a rendelkezésre álló memória lehetővé teszi azok futtatását. A rendszer lehetőséget biztost ciklikus és nem ciklikus taszkok futtatására egyaránt. 

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

A taszkok lehetne Blokkolt állapotba, ahonnan egy esemény bekövetkezését követően automatikusan Futásra kész állapotba kerülnek. 

Időbeli események azok, amik egy bizonyos időpillanatban következnek be (például egy kéleltetési idő letelik). Az időbeli eseményeket kihasználva lehetőség nyílik periodikus futtatásra, vagy időtúllépés detektálására. 

Szinkronizáló események azok, amikor egy taszk vagy megszakításkezelő rutin jelzést küld valamilyen kommunikációs struktúrán keresztül egy másik taszknak. Tipikusan aszinkron jelzésre használjuk, mint például adat érkezését egy periférián keresztül.


### Taszkok prioritásának meghatározása

Ökölszabályként alkalmazható, hogy a hard real-time funkciókat magasabb prioritással látjuk el, mint a soft real-time funkciókat.

Rate Monotonic Scheduling (Gyakoriság Monoton Ütemezés) az a gyakran alkalmazott prioritás-hozzárendelés, amikor minden taszkhoz egyedi prioritásszintet rendelünk a végrehajtási gyakoriság függvényében. A gyakrabban lefutó folyamatokhoz magasabb, a ritkábban lefutó folyamatokhoz alacsonyabb prioritást állítunk be.