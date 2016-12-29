# Rendszerterv

Az operációs rendszerek (xy) fejezetben felsorolt paramétereit több módszerrel meghatározhatjuk.

Az egyik kézenfekvő megoldás, ha szoftveres úton az érdekes futási szakaszokban elmentjük a rendszer órájának aktuális értékét, majd ezek ismeretében határozzuk meg a jellemzőket. Az információ kinyerése a beágyazott eszközből legegyszerűbben soros kommunikáción történhet.

Másik megoldás egy kiegészítő hardver tervezése, ami lehetővé teszi külső mérőeszköz csatlakoztatását.

Az operációs rendszerek mérése folyamán fontos, hogy maga a mérés lehetőleg ne befolyásolja a mért értékeket. Ezért a mérőrendszer és mérőfolyamat összeállításánál kiemelt figyelmet kapott, hogy a közvetlenül a méréshez kapcsolódó szoftverrészek a minimálisak legyenek. Ennek okán a kiegészítő harver alkalmazása mellett döntöttem.


## Magas szintű Rendszerterv

A kiegészítő hardver tervezése során a fő szempont a mérés folyamatának egyszerű felépítése volt. A feladat részét képezi egy ipari alkalmazás szimulációja, melyhez szükséges alkatrészek is ugyanezen hardveren kaptak helyet.


### Ipari alkalmazás

Az ipari alkalmazás egy helyi és egy távoli állomás megfigyelését teszi lehetővé. A helyi állomáson egy potméter, egy hőmérő, kapcsolók és indikátor LED-ek találhatóak, míg a távoli állomáson egy TI SensorTag méri a hőmérsékletet, páratartalmat és a fényviszonyokat. Amért adatokat az eszközön futó webszerver jeleníti meg, illetve az indikátor LED-ek állapotai ugyanezen felületen állíthatóak.


### Mérőkártya

Az mérőkártya egy vezérlő hardvertől független felépítést kapott. Az egyes perifériák vezetékeken keresztül csatlakoznak a kártyához.

Az egyes perifériák a csatlakoztatható fejlesztőkártyák szerint táblázatba szedve:

| Közös | STM32F4 | Raspberry Pi |
|:-----:|:-------:|:------------:|
| Kapcsolók: 2 GPIO | <sup>1</sup>MCP9700A: 1 ADC | ADS7924: 1 I<sup>2</sup>C |
| LED-ek: 2 GPIO | <sup>1</sup>Potméter: 1 ADC |  |
| Input: 1 Interrupt | SDCard: SDIO |  |
| <sup>2</sup>Output: &lceil;log<sub>2</sub>N&rceil;+1 GPIO | BLE112: 1 UART |  |
<sup>1</sup>: Jumperrel választható, hogy az STM ADc-jére, vagy az ADS7942-re csatlakozzon.

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
| &sum; &lceil;log<sub>2</sub>N&rceil;+14 pin | &sum; &lceil;log<sub>2</sub>N&rceil;+1 pin | &sum; 1 pin | &sum; 2 pin |