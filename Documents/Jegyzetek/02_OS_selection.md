# Választás szempontjai

Az operációs rendszerek kiválasztásánál elsődleges szempont a hardverek támogatottsága volt, de ezen kívül még az oktatási célra való elérhetőséget is szem előtt tartottam.


## STM32F4 Discovery

Az UBM Tech minden évben készít egy kutatást a beágyazott rendszereket piacán, melyben többek között a használt beágyazott operációs rendszerekkel kapcsolatban is pbulikál adatokat. A statisztika alapján a két leggyakrabban használt operációs rendszer a FreeRTOS és a uC/OS-II. A uC/OS új verziója, a uC/OS-III a listán hátrébb kapott helyet, de még így is befért a tíz vezető rendszer közé. Mindegyik rendszer népszerűsége nőtt a 2014-es évhez képest.

Az STM32F4 Discovery kártyához elérhető szoftvercsomag tartalmazza a FreeRTOS rendszert, ami jelzi a rendszer támogatottságának mértékét. A FreeRTOS hivatalos oldalán megvásárolhatóak a rendszer használatát bemutató könyvek, illetve online elérhető leírások, amik szintén segítik a rendszer megismerését. Ezáltal az első megvizsgált rendszernek a FreeRTOS-t választottam.

A Micrium uC/OS rendszerei oktatási célra ingyenesen elérhetőek, beleértve a rendszer dokumentációit is. A választott másik rendszer a uC/OS-III.


## Raspberry Pi 3

A Raspberry Pi megjelenése óta támogatja különböző linux disztribúciók futtatását az eszközön, és a fejlesztőkártya második verziója óta a Windows 10 IoT Core is telepíthető rá. Az asztali alkalmazásfejlesztők körében mind a linux, mind a Windows elterjedten használt operációs rendszer, ezért a Raspberry Pi 3-on ezt a két rendszert vizsgálom meg. 


--------------------------------------------------------------

# Források

- [EETimes](http://www.eetimes.com/document.asp?doc_id=1328259)
- 2015 Embedded Market Study