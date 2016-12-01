# Fejlesztőkártyák

## STM32F4 Discovery

Manapság egyre inkább teret nyernek maguknak az ARM alapú mikrokontrollerek, melyek nem csak nagy számítási kapacitásukkal, de egyre alacsonyabb árukkal szorítják ki versenytársaikat. Az egyik legelterjedtebb gyártó, az STMicroelectronics (továbbiakban STM) több fejlesztőkártyát is piacra bocsátott az elmúlt években, melyeken a különböző mikrokontrollerek képességeit ismerheti meg a fejlesztő. A kapható fejlesztőkártyák amellett, hogy megkíméli a fejlesztőt a saját hardver tervezésétől - így a tervezési hibából adódó problémák keresésétől is -, a legtöbb esetben a gyártó széles körű támogatást is nyújt a termékhez (mintaprogramok, fórumok, stb.). 

Az STM által gyártott fejlesztőkártyák közül ár-érték arányának köszönhetően talán a leggyakrabban használt az STM32F407 Discovery kártya. A rajta található mikrokontroller rendelkezik a legtöbb alkalmazásban előforduló perifériák mindegyikével. A teljesség igénye nélkül:

- GPIO-k,
- Soros kommunikációs portok (szinkron és aszinkron egyaránt),
- Ethernet port,
- Analog-Digital Converter,
- Digital-Analog Converter,
- Időzítők,
- High-Speed USB (OTG támogatással),
- SPI,
- I2C,
- I2S,
- SDIO (SD illetve MMC kártya kezeléséhez),
- CAN,
- Szoftveres és hardveres magszakítások.

A nagyszámú periféria mellett a mikrokontroller számítási kapacitása is kimagaslik a hasonló árkategóriájú eszközök köréből, köszönhetően az akár 168 MHz-es órajelének, a beépített CRC és lebegő pontos aritmetikai egységének, illetve a több perifériát is kezelni képes DMA-knak.

Az eszköz 1MByte Flash memóriával és 192kbyte RAM-mal rendelkezik.

A kártyán megtalálható több periféria, mely a különböző interfészek kipróbálását teszi lehetővé (mint például gyorsulásszenzor, mikrofon).

![stm32f4_discovery](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Eval_Boards/01_STM32F4-Discovery.jpg "STM32F4 Discovery")


### STM32F4 Discovery - Base Board

Az STM32F4 Discovery fejlesztőeszközhöz több kiegészítő kártya is kapható, melyek célja a kipróbálható perifériák számának növelése. Egyik ilyen bővítőkártya az STM32F4DIS-BB, ami tartalmaz microSD-kártya foglalatot, az Ethernet interfész fizikai rétegét megvalósító IC-t, illetve a csatlakoztatáshoz szükséges RJ45-ös csatlakozót. Ezen kívül kivezetésre került egy DB9-es csatlakozó - mely az egyik soros kommunikációs portot teszi elérhetővé -, egy FPC csatlakozó - mely kamera csatlakoztatását teszi lehetővé -, illetve az egyik oldali csatlakozósorra ráköthető 3,5"-os TFT kijelző.

![stm32f4_disco_bb](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Eval_Boards/02_STM32F4DIS-BB.jpg "STM32F4 Discovery BaseBoard")

## Raspberry Pi 3

A Raspberry Pi Foundation 2008-ban alapították azzal a céllal, hogy a informatikai tudományok területén segítse az oktatást.

Az első nagyteljesítményű, bankkártya méretű számítógépüket 2012 februárjában bocsájtották piacra, melynek ára töredéke volt az asztali számítógépekének. Azóta több verziója is kijött az eszköznek, melyet folyamatosan fejlesztettek mind teljesítményben, mind az integrált funkciók számában. 2015 novemberében a világ első 5$-os számítógépével jelentek meg a piacon, melynek a Raspberry Pi Zero nevet adták.

A legújabb verziójú kártya, a Raspberry Pi 3 model B az előző verzióhoz képest erősebb processzort kapott, illetve tartalmaz beépített Bluetooth illetve WiFi modult.

![raspberry_pi_3](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Eval_Boards/03_Raspberry_Pi_3.jpg "Raspberry Pi 3")

A hardver főbb jellemzői:
- 1,2GHz 64-bit quad-core ARMv8 CPU,
- 802.11n Wireless LAN,
- Bluetooth 4.1,
- Bluetooth Low Energy,
- 1GB RAM,
- 4 USB port,
- 40 GPIO pin,
- HDMI port,
- Ethernet port,
- Kombinált 3,5mm-es jack aljzat (audio és kompozit videó),
- MicroSD kártyafoglalat,
- VideoCore IV GPU.

Interfészek:
- SPI,
- UART,
- DPI (Display Parallel Interface),
- SDIO,
- PCM (Pulse-code Modulation),
- 1-WIRE,
- JTAG,
- GPCLK (General Purpose Clock),
- PWM.

Már az első verzió megjelenésekor rendelkezésre álltak különböző linux disztribúciók portjai, melyekkel az eszköz asztali számítógépként használható volt. Népszerűségének köszönhetően a második verziótól kezdve már a Windows 10 IoT Core is támogatja a platformot.


-------------------------------------------------------------------------

# Források

- [Raspberry Pi Strategy](https://www.raspberrypi.org/files/about/RaspberryPiFoundationStrategy2016-18.pdf)
- [Raspberry Pi 3](https://www.raspberrypi.org/products/raspberry-pi-3-model-b/)
- [Raspberry Pi 3 interfaces](http://pinout.xyz/pinout/)