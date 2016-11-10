# FreeRTOS


## Bevezetés

A FreeRTOS a Real Time Engineers Ltd. által fejlesztett valós idejű operációs rendszer. A fejlesztők céljai között volt a rendszer erőforrásigényének minimalizálása, hogy a legkisebb beágyazott rendszereken is futtatható legyen. Ebből adódóan csak az alap funkciók vannak megvalóstva, mint ütemezés, taszkok közötti kommunikáció lehetősége, memóriamenedzsment, de nincs beépített támogatás hálózati kommunikációra vagy bármiféle külső hardver használatára (ezeket vagy nekünk meg megírnunk, vagy harmadik féltől származó kódot kell használnunk).

A rendszer módosított GPLv2 licencet használ. A licencmódosítás lehetővé teszi GPL-től eltérő licenccel ellátott modulok használatát, amennyiben azok a FreeRTOS-sal kizárólag a FreeRTOS API-n keresztül kommunikálnak. 


## Taszkok



A FreeRTOS nem korlátozza a létrehozható taszkok számát, amíg a rendelkezésre álló memória lehetővé teszi azok futtatását. A rendszer lehetőséget biztost ciklikus és nem ciklikus taszkok futtatására egyaránt. 

A beágyazott rendszerek döntő része egymagos processzorokat használ, amiből az következik, hogy egyszerre csak egy taszk futhat. A taszkok eszerint két nagy csoportba oszthatóak: éppen futó taszk (___Fut___ állapot), illetve az összes többi (___Nem fut___ állapot).

![freertos_states](https://github.com/Lyque/diplomaterv/raw/FreeRTOS_notes/Documents/Jegyzetek/Figures/FreeRTOS/01_FreeRTOS_states.png "FreeRTOS states simplified")

Annak, hogy egy taszk éppen miért nem fut, több oka lehet. Ez alapján a __Nem fut__ állapot több állapotra felosztható.



![freertos_states_expanded](https://github.com/Lyque/diplomaterv/raw/FreeRTOS_notes/Documents/Jegyzetek/Figures/FreeRTOS/02_FreeRTOS_states_expanded.png "FreeRTOS states expanded")



##
