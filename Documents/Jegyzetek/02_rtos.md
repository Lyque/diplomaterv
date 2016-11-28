# RTOS meghatározása ([FreeRTOS](http://www.freertos.org/about-RTOS.html))

## Operációs rendszer meghatározás

Az operációs rendszer egy szoftver, ami kezeli a számítógép alap funkcióit és szolgáltatásokat biztosít más programok (vagy alkalmazások) számára. Az alkalmazások valósítják meg azt a funkcionalitást, amire a számítógép felhasználójának szüksége van, vagy a számítógép felhasználója akar. Az operációs rendszer által nyújtott szolgálatások az alkalmazások feljesztését egyszerűsítik, ezáltal felgyorsítják a fejlesztést és karbantarthatóbbá teszik a programot.


## Real-time operációs rendszer meghatározás

A legtöbb operációs rendszer látszólag lehetővé teszi több program egyidejű futtatását (multitasking). A valóságban minden processzor mag csak egy szálat tud egy időpillanatban futtatni. Az operációs rendszer ütemezője a felelős azért, hogy eldöntse, melyik program mikor fusson, és a programok közti gyors váltással éri el az egyidejű futás látszatát.

[Kép]

Az operációs rendszer típusát az ütemező döntési mechanizmusa határozza meg. Egy real time operációs rendszer ütemezője úgy van megtervezve, hogy a végrehajtási minta determinisztikus legyen. Ez beágyazott rendszerek esetén érdekes, mert a beágyazott rendszereknél gyakran követelmény a valósidejűség, vagyis hogy a rendszernek egy szigorúan meghatározott időn belül reagálnia kell egy adott eseményre. A. valósidejű követelményeknek való megfelelés csak úgy lehetséges, ha az operációs rendszer ütemezője előre megjósolható döntéseket hoz.

Valósidejű operációs rendszerek közül megkülönböztetjük a soft real-time és a hard real-time rendszereket.

Soft real-time rendszer esetén nem probléma, ha nem érkezik válasz a megadott határidőn belül, az csak csak a rendszer minősítését rontja.

Hard real-time rendszer esetén viszont a rendszer alkalmatlanná válik a feladatra, ha a határidőt nem tudja betartani. Például egy személygépjármű légzsákjának késedelmes nyitása akár halálos következményekkel is járhat.

## Operációs rendszer feladatai

Az operációs rendszer elsődleges feladata elsősorban a használt processzor perifériáinak kezelése, azokhoz meghatározott interfész biztosítása. További feladata még a létrehozott taszkok ütemezése, a taszkok közötti kommunikáció és szinkronizáció megvalósítása.


### Ütemezés

Az operációs rendszer egyik meghatározó része a használt ütemező működésének elve, mely meghatározza az eszköz alkalmasságát egy adott feladatra. Az ütemező feladata tasztváltáskor az éppen futó taszk állapotának elmentése és a futtatandó taszk állapotának betöltése.

A továbbiakban ismertetem néhány elterjedt ütemezési mechanizmus alapelvét.

#### First-come first-served (Kiszolgálás beérkezési sorrendben)




#### Shortest Job First (Legrövidebb feladat először)

#### Prioritásos ütemezés

#### Round Robin


### Operációs rendszer által nyújtott szolgáltatások


### Operációs rendszer használata esetén felmerülő problémák


-------------------------------------------------------------------------------

# Források

- [Definíciók](http://www.freertos.org/about-RTOS.html)
- [Scheduling](https://cseweb.ucsd.edu/classes/fa02/cse120/lectures/sched.pdf)