# Rendszerterv

Az operációs rendszerek (xy) fejezetben felsorolt paramétereit több módszerrel meghatározhatjuk.

Az egyik kézenfekvő megoldás, ha szoftveres úton az érdekes futási szakaszokban elmentjük a rendszer órájának aktuális értékét, majd ezek ismeretében határozzuk meg a jellemzőket. Az információ kinyerése a beágyazott eszközből legegyszerűbben soros kommunikáción történhet.

Másik megoldás egy kiegészítő hardver tervezése, ami lehetővé teszi külső mérőeszköz csatlakoztatását.

Az operációs rendszerek mérése folyamán fontos, hogy maga a mérés lehetőleg ne befolyásolja a mért értékeket. Ezért a mérőrendszer és mérőfolyamat összeállításánál kiemelt figyelmet kapott, hogy a közvetlenül a méréshez kapcsolódó szoftverrészek a minimálisak legyenek. Ennek okán a kiegészítő harver alkalmazása mellett döntöttem.


## Magas szintű Rendszerterv

A kiegészítő hardver tervezése során a fő szempont a mérés folyamatának egyszerű felépítése volt. A feladat részét képezi egy ipari alkalmazás szimulációja, melyhez szükséges alkatrészek is ugyanezen hardveren kaptak helyet.


### Ipari alkalmazás

Az ipari alkalmazás egy helyi és egy távoli állomás megfigyelését teszi lehetővé. A helyi állomáson egy potméter, egy hőmérő, kapcsolók és indikátor LED-ek találhatóak, míg a távoli állomáson egy TI SensorTag méri a hőmérsékletet, páratartalmat és a fényviszonyokat. Amért adatokat az eszközön futó webszerver jeleníti meg (Raspberry PI esetén, STM32-t soros kommunikáció segítségével, grafikus alkalmazással lehet vezérelni), illetve az indikátor LED-ek állapotai ugyanezen felületen állíthatóak.


### Mérőkártya

Az mérőkártya a vezérlő hardvertől független felépítést kapott. Az egyes perifériák vezetékekkel csatlakoznak a kártyához.

Az egyes perifériák a csatlakoztatható fejlesztőkártyák szerint táblázatba szedve:

| Közös | STM32F4 | Raspberry Pi |
|:-----:|:-------:|:------------:|
| Kapcsolók: 2 GPIO | <sup>1</sup>MCP9700A: 1 ADC | ADS7924: 1 I<sup>2</sup>C+Reset |
| LED-ek: 2 GPIO | <sup>1</sup>Potméter: 1 ADC |  |
| Input: 1 Interrupt | SDCard: SDIO + egyéb|  |
| <sup>2</sup>Output: &lceil;log<sub>2</sub>N&rceil;+1 GPIO | BLE112: 1 UART+Reset |  |
<sup>1</sup>: Jumperrel választható, hogy az STM ADC-jére, vagy az ADS7942-re csatlakozzon.

<sup>2</sup>: N a taszkok száma (Idle taszkkal együtt).

![system_design](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/System_design/01_system_design.png "system_design")

| Tüskesor 1 <br>(Csatlakozó) | Tüskesor 2 <br>(Output) | Tüskesor 3 <br>(Input) | Tüskesor 4 <br>(Worst response) |
|:----------:|:----------:|:----------:|:----------:|
| 5V (1 pin) | &lceil;log<sub>2</sub>N&rceil;+1 output | 1 input | 1 input |
| 3V (1 pin) | | | 1 output |
| GND (1 pin) | | | |
| 2db kapcsoló (2 pin)  | | | |
| 2db LED (2 pin) | | | |
| 1db input | | | |
| &lceil;log<sub>2</sub>N&rceil;+1 output | | | |
| 2db ADC (2 pin) | | | |
| 1 I<sup>2</sup>C (2 pin) | | | |
| 1 UART (2 pin) | | | |
| SDCard (11 pin) | | | |
| Reset-ek (2 pin) | | | |
| &sum; &lceil;log<sub>2</sub>N&rceil;+27 pin | &sum; &lceil;log<sub>2</sub>N&rceil;+1 pin | &sum; 1 pin | &sum; 2 pin |


#### Taszkok számának becslése

| Azonosító | Becsült taszkok száma |
|:---------:|:---------------------:|
| Idle | 1 |
| Késleltetés + Jitter | 1 |
| Rhealstone | 6 |
| Kapcsolók | 1 |
| LED-ek | 1 |
| SD kártya | 1 |
| Bluetooth | 1 |
| ADC-k | 1 |
| &sum; | 16 |


#### STM32F4 lábkiosztás

| Funkció | Mikrovezérlő láb | Láb funkció |
|:-------:|:----------------:|:-----------:|
| NCD | PB15 | GPIO_IN |
| DAT1 | PC9 | SDIO_D1 |
| DAT0 | PC8 | SDIO_D0 |
| CLK | PC12 | SDIO_CK |
| CMD | PD2 | SDIO_CMD |
| CD/DAT3 | PC11 | SDIO_D3 |
| DAT2 | PC10 | SDIO_D2 |
|  |  |  |
| UART6_TX | PC6 | UART6_TX |
| UART6_RX | PC7 | UART6_RX |
|  |  |  |
| BLE_TX | PA9 | UART1_TX |
| BLE_RX | PA10 | UART1_RX |
|  |  |  |
| ADC_SCL | PB6 | I2C1_SCL |
| ADC_SDA | PB7 | I2C1_SDA |
|  |  |  |
| TERM | PC2 | ADC1_IN12 |
| POT | PC3 | ADC2_IN13 |
|  |  |  |
| IN | PC13 | GPIO_IN |
| ID3 | PD7 | GPIO_OUT |
| ID2 | PD6 | GPIO_OUT |
| ID1 | PD5 | GPIO_OUT |
| ID0 | PD4 | GPIO_OUT |
| OUT | PD3 | GPIO_OUT |
|  |  |  |
| BLE_RST | PE4 | GPIO_OUT |
| ADC_RST | PE5 | GPIO_OUT |
|  |  |  |
| LED0 | PD8 | GPIO_OUT |
| LED1 | PD9 | GPIO_OUT |
| SW0 | PD10 | GPIO_IN |
| SW1 | PD11 | GPIO_IN |

--------------------------------------------------

# Teljesítménymérő szálak terve

## Memóriaméret

Az operációs rendszerek memóriaigényét az egyes fordítások után kapott eredmények alapján táblázatba foglalom, ami alapján összehasonlíthatóak a rendszerek. Ez a módszer csak az STM esetén járható út, mivel Raspberry PI-n az operációs rendszerek mérete várhatóan nagyságrendekkel nagyobb, mint az egyes alkalmazásoké.


## Késleltetés

A késleltetés mérése során a kimeneti láb változtatását a megszakítási rutinban hajtom végre. Ekkor az esetleges szemafor átadása nem okoz többletkésleltetést.


## Jitter

A jitter meghatározása több késleltetés-mérés alapján történik


## Rhealstone

### Taszkváltási idő

A taszkváltási idő méréséhez három, azonos prioritású taszkot hozok létre, melyek végtelen ciklusban futnak. Egyéb hasznos feladatot nem látnak el a taszkok.


### Preemptálási idő

A preemptálási idő méréséhez három, különböző prioritású taszkot hozok létre. A legalacsonyabb prioritású taszk végtelen ciklusban fut, a közepes prioritású taszk rövid futási idő után várakozik, míg a legmagasabb prioritású taszk szintén rövid idő után hosszabb ideig várakozik. 


### Megszakítás-késleltetési idő

A megszakítás-késleltetési idő nagyon hasonlít a késleltetési időhöz, viszont itt a megszakítás kezelő rutin első utasításáig eltelt időt mérem. Ehhez a beágyazott operációs rendszereknél elterjedt szemfarorral történik a kezelő rutin várakoztatása.


### Szemafor-váltási idő

A szemafor-váltási idő méréséhez két, különböző prioritású taszkot hozok létre. Az alacsony prioritású taszk lefoglalja a szemafort, majd egy hosszú lefutású ciklusba lép. Közben a magasabb prioritású taszk futásra kész állapotba lép, és preemptálja az alacsony prioritásút. A magas prioritású taszk is megpróbálja lefoglalni a szemafort, de mivel azt már az alacsonyabb prioritású taszk birtokolja, ezért várakozó állapotba lép. Az alacsony prioritású taszk a ciklus lefutása után visszaadja a szemafort, amit a magas prioritású taszk futása követ. A szemafor felszabadulása és a magas prioritású taszk indulása között eltelt időt mérem. A mérés során mindkét definíció szerinti adatok kinyerhetőek.


### Deadlock-feloldási idő

A deadlock-feloldási idő mérése során három, különböző prioritású taszkot hozok létre. Az alacsony prioritású taszk lefoglalja az erőforrást (mutex), majd egy hosszú lefutású ciklusba lép. Közben a közepes prioritású taszk elkezdi a futását (ezzel preemptálva az alacsony prioritású taszkot), és szintén egy hosszú lefutású cilusba lép. Ezalatt a legmagasabb prioritású taszk kerül futó állapotba, és megpróbálja lefoglalni a közös erőforrást. Az erőforrás igénylésétől a megszerzéséig szükséges időt mérem a mérés során.


### Datagram-átviteli idő

A datagram-átviteli idő mérése során két, különböző prioritású taszkot hozok létre. Az alacsony prioritású taszk a futás elején lefoglalja a szemafort. Közben a magasabb prioritású taszk futásra kész állapotba kerül, és elkezd feltölteni egy 1kB nagyságú objektumot. Az objektummal végzett művelet megkezdését egy kimeneti láb magas szintre állításával jelzi. Mikor végzett a tároló feltöltésével, megpróbálja lefoglalni a szemafort, ezzel átadva a processzort az alacsony prioritású taszknak. Az alacsony prioritású taszk kiüríti a tárolót, majd felszabadítja a szemafort, amit újra a magas prioritású taszk futása követ. A mérési folyamat befejeztét a kimeneti láb alacsony szintre állításával jelzi.

Az üzenet-késleltetési idő nem kerül meghatározásra a dolgozatban.


### Legrosszabb válaszidő

A legrosszabb válaszidő méréséhez a megszakítás-késleltetési idő szoftverét használom, mivel a megszakítás operációs-rendszereknél használt kezelését az valósítja meg.


--------------------------------------------------

# ToDo

## Schematic+Footprint

- Potméter __[DONE]__
- Induktivitás __[DONE]__


## Design

- Potméter __[DONE]__
- BLE112 __[DONE]__
    - Tápszűrés __[DONE]__
    - Debugger/Programmer bekötése __[DONE]__
    - UART bekötése __[DONE]__
- ADS9724 __[DONE]__
- MCP9700A __[DONE]__
- Párhuzamos LED-ek __[DONE]__
- LED-ek __[DONE]__
- POWER LED-ek __[DONE]__
- Kapcsolók __[DONE]__
- SDCard módosítása __[DONE]__
    - VDD átnevezése __[DONE]__
- Polaritásvédelmet átgondolni
    - GND3 és GND5 nincs használva -> át kell tervezni __[DONE]__
        - Talán elég a power subschematicból kihozni a két segédföldet, és az egyes blokkokba azt bevezetni. (Természetesen a több feszültséget használó blokkok külön földet igényelnek!) __[DONE]__
        - GND3 el lett távolítva (feleseleges és egyben logikátlan is) __[DONE]__