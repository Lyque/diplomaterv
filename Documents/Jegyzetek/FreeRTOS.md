# FreeRTOS


## Bevezetés

A FreeRTOS a Real Time Engineers Ltd. által fejlesztett valós idejű operációs rendszer. A fejlesztők céljai között volt a rendszer erőforrásigényének minimalizálása, hogy a legkisebb beágyazott rendszereken is futtatható legyen. Ebből adódóan csak az alap funkciók vannak megvalóstva, mint ütemezés, taszkok közötti kommunikáció lehetősége, memóriamenedzsment, de nincs beépített támogatás hálózati kommunikációra vagy bármiféle külső hardver használatára (ezeket vagy nekünk meg megírnunk, vagy harmadik féltől származó kódot kell használnunk).

A rendszer módosíított GPLv2 licencet használ. A licencmódosítás lehetővé teszi GPL-től eltérő licenccel ellátott modulok használatát, amennyiben azok a FreeRTOS-sal kizárólag a FreeRTOS API-n keresztül kommunikálnak. 


## Taszkok

A FreeRTOS nem korlátozza a létrehozható taszkok számát, amíg a rendelkezésre álló memória lehetővé teszi azok futtatását. A rendszer lehetőséget biztost ciklikus és nem ciklikus taszkok futtatására egyaránt. 

![freertos_states](https://github.com/Lyque/diplomaterv/raw/FreeRTOS_notes/Documents/Jegyzetek/Figures/01_FreeRTOS_states.png "FreeRTOS states simplified")


![freertos_states_expanded](https://github.com/Lyque/diplomaterv/raw/FreeRTOS_notes/Documents/Jegyzetek/Figures/02_FreeRTOS_states_expanded.png "FreeRTOS states expanded")



##
