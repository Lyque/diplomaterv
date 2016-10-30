# RTOS meghatározása ([FreeRTOS](http://www.freertos.org/about-RTOS.html))

## Operációs rendszer meghatározás

Az operációs rendszer egy program, ami támogatja a számítógép alap funkcióit és szolgáltatásokat biztosít más programok (vagy alkalmazások) számára, amelyek a számítógépen futnak. Az alkalmazások valósítják meg azt a funkcionalitást, amire a számítógép felhasználójának szüksége van, vagy a számítógép felhasználója akar. Az operációs rendszer által nyújtott szolgálatások az alkalmazások feljesztését egyszerűsítik, ezáltal felgyorsítják és kézbentarthatóbbá teszik.


## Real time operációs rendszer meghatározás

A legtöbb operációs rendszer látszólag lehetővé teszi több program egyidejű futtatását (multitasking). A valóságban minden processzor mag csak egy szálat tud egy időpillanatban futtatni. Az operációs rendszer ütemezője a felelős azért, hogy eldöntse, melyik program mikor fusson, és a programok közti gyors váltással éri el az egyidejű futás látszatát.

Az operációs rendszer típusát az ütemező döntési mechanizmusa határozza meg. Egy real time operációs rendszer ütemezője úgy van megtervezve, hogy a végrehajtási minta determinisztikus legyen. Ez inkább beágyazott rendszerek esetén érdekes, mert a beágyazott rendszereknél gyakran követelmény a valósidejűség, vagyis hogy a rendszernek egy szigorúan meghatározott időn belül reagálnia kell egy adott eseményre. Az vlósidejű követelményeknek való megfeleléscsak őgy lehetséges, ha az operációs rendszer ütemezője előre megjósolható döntéseket hoz.