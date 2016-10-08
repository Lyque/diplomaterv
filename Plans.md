# Diplomatervezés I.

## Ipari alkalmazásban szereplő perifériák

- GPIO:
    - Input: beolvasott értéket megjeleníti,
    - Output: vezérelhető kimenet,
- ADC: 
    - hőmérő, fényérzékelő, etc.
- SDIO: 
    - SD Card,
    - Logoláshoz,
    - Képfeldolgozás ott történne,
    - FAT fájlrendszerrel,
- PWM: 
    - Motorvezérlő PID szabályozó,
    - Szögszabályozás,
    - Fordulatszámszabályozás,
- ETHERNET:
    - Webszerver a tesztelt eszközön,
    - Távoli felügyelet, beavatkozás.

## Operációs rendszerek

### Kis erőforrásigényű
- FreeRTOS,
- ...

### Nagyobb erőforrásigényű
- Linux,
- ...

## Rendszerterv

### Szoftver

### Hardver

Az összes kártyához/operációs rendszerhez egyetlen egy 
kártya készül. Mivel így nehézkes (és egyben felesleges is) 
olyan interfészt kialakítani, ami minden kártyával kompatibilis, 
ezért a csatlakozás tüskesorok és kábelek segítségével valósul meg.

#### Felhasznált eszközök

- STM32F4-Discovery,
- Raspberry-Pi,


- SD Card,
- RS385-ST DC motor incrementális adóval,
- STM32F4DIS-BB ETHERNET baseboard,

#### Csatlakozók

- NXW-06K,
- SD Card slot,
- Speedometer mutató,
- Raszterosztású tüskesor,
- LED-ek.