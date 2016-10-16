# LwIP jegyzet (ST [UM1713])


## Támogatott protokollok

- __IPv4__,
- IPv6,
- ICMP (Internet Control Message Protocol),
- IGMP (Internet Group Management Protocol),
- UDP,
- __TCP__,
- DNS,
- SNMP,
- DHCP,
- PPP (Point to Point Protocol),
- ARP (Address Resolution Protocol).


## Három programozási interfész

- Raw API: 
    - Natív LwIP API,
    - Esemény callback alapú fejlesztés,
    - Legjobb teljesítmény,
    - Optimalizált kódméret,
    - Nehezebb fejlesztés.
- Netconn API:
    - Magasszintű API,
    - RTOS szükséges hozzá,
    - Multithread működés.
- BSD Socket API:
    - Berkeley-szerű socket API
    - Berkeley socket (kitérő):
        - Internet socket-hez API,
        - Processzek közötti kommunikációhoz (IPC),
        - A socket-et mint file leíró reprezentálja (UNIX rendszereknél megszokott),
        - A POSIX socket szinonímája (Berkeley socket először mint de facto standard, majd POSIX specifikáció részévé nőtte ki magát),
        - Ma már minden modern operációs rendszer implementálja,
        - A Socket API függvényei:
            - socket(),
            - bind(),
            - listen(),
            - connect(),
            - accept(),
            - send(),
            - close(),
            - gethostbyname(),
            - gethostbyaddr(),
            - select(),
            - poll(),
            - getsockopt(),
            - setsockopt(),
        - (Ismerős függvények... Talán ezeket használtam Qt-ban és V-Rep-ben is?),
        - Létezik _Raw Socket_, ami megkerüli a host TCP/IP stack-jét, így segíti a debuggolást,
        - A socket működhet blokkoló és nem-blokkoló módon is (fcntl() és ioctl() függvényekkel állítható a mód),
        - Az operációs rendszer nem engedi el az erőforrást egészen addig, amíg a socket-et le nem zárják,
            - Ez akkor is bekövetkezik, ha pl. a _connect_ elhasal, és újrapróbálkozunk,
            - (Talán emiatt volt, hogy a V-Rep-ben nem megfelelő leállítás esetén nem tudtuk egyből elölről kezdeni a tesztelést?),
        - Forrás: [Wikipedia](https://en.wikipedia.org/wiki/Berkeley_sockets),


## TCP/IP model rétegei

További források:
- [Omnisecu](http://www.omnisecu.com/tcpip/tcpip-model.php),
- [Microsoft](https://technet.microsoft.com/en-us/library/cc958821.aspx).


1. Link layer:
    - Más forrásokban Network Access Layer,
    - Definiálja, hogy az adat fizikailag hogyan kell haladjon a hálózati csatornán (electormos/optikai jelek),
2. Internet layer:
    - Független hálózatokat köt össze,
    - Az adatot csomagokba rendezi (IP datagrams),
        - Tartalmazza a forrás és a cél címét (logikai vagy IP cím),
    - Felelős az IP datagramok továbbításáért,
    - Feladata:
        - Csomagok hálózatra illesztése,
        - Csomagok független továbbítása a cél felé,
        - A csomagok (mivel csomagkapcsolt kommunikáció) különböző sorrendben érkezhetnek a célhoz,
            - A felsőbb rétegek feladata a csomagok sorbarendezése,
    - Fő protokolljai:
        - IP,
        - ICMP,
        - ARP,
        - RARP,
        - IGMP,
3. Transport layer:
    - A forrás és a cél host közötti kommunikációért felelős,
    - Definiálja a szolgáltatás szintjét és állapotát az adattovábbítás során,
    - Fő protokolljai:
        - __TCP__,
        - UDP,
4. Application layer:
    - Process-to-process kommunikáció,
    - Fő protokolljai:
        - __HTTP__,
        - Telnet,
        - SSH,
        - FTP,
        - TFTP,
        - SNMP,
        - SMTP,
        - DHCP,
        - RDP,
        - stb.


## LwIP mappa rendszer

- __doc:__ 
    - a dokumentációt tartalmazza,
- __src:__
    - az LwIP stack forrásfájljait tartalmazza,
- __api:__
    - a Netconn és a Socket API fájljait tartalmazza,
- __core:__
    - az LwIP alapfájljait tartalmazza,
- __include:__
    - az LwIP include fájljait tartalmazza,
- __netif:__
    - a Network interfész fájlokat tartalmazza,
- __system:__
    - az  LwIP hardware specifikus fájljait tartalmazza,
- __arch:__
    - STM32 architektúra fájljai tartalmazza,
- __OS:__
    - az LwIP hardware specifikus fájljait tartalmazza operációs rendszer használata esetén,
- __noOS:__
    - az LwIP hardware specifikus fájljait tartalmazza standalone használat esetén.


## LwIP API áttekintés

Három féle API áll rendelkezésre:
- Raw API,
- Netconn API,
- Socket API.


### Raw API

- Az LwIP API-ra alapszik,
- Callback-alapú alkalmazás fejlesztéséhez,
- A felhasználónak kell callback függvényeket regisztálnia a különböző eseményekhez (TCP_Sent, TCP_error, stb..).
- A callback függvényt az LwIP core layer hívja meg, ha az esemény bekövetkezik,


#### TCP Raw API függvények

- TCP kapcsolat felépítés:
    - tcp_new,
    - tcp_bind,
    - tcp_listen,
    - tcp_accept,
    - tcp_accepted,
    - tcp_connect,
- TCP adat küldése:
    - tcp_write,
    - tcp_sent,
    - tcp_output,
- TCP adat fogadása:
    - tcp_recv,
    - tcp_recved,
- Alkalmazás polling:
    - tcp_poll,
- Kapcsolat lezárása és megszakítása:
    - tcp_close,
    - tcp_err,
    - tcp_abort.


#### UDP Raw API függvények

- udp_new,
- udp_remove,
- udp_bind,
- udp_connect,
- udp_disconnect,
- udp_send,
- udp_recv.


### Netconn API

- Magas szintű szekvenciális API,
- Blokkoló open-read-write paradigmákat használ,
- Helyes működéshez szükséges multithread-et támogató operációs rendszer.


#### Netconn API függvények

- netconn_new,
- netconn_delete,
- netconn_bind,
- netconn_connect,
- netconn_send,
- netconn_recv,
- netconn_listen,
- netconn_accept,
- netconn_write,
- netconn_close.


### Socket API

- BSD socket API,
- A Netconn API fölé épült.


### Socket API függvények

- soocket,
- bind,
- listen,
- connect,
- accept,
- read,
- write,
- close.


## LwIP buffer menedzsment

- Az LwIP pbuf adatstruktúrát használ a csomagok bufferelésére,
- A pbuf lehetővé teszi a dinamikus memóriafoglalást a csomagok tárolására,
- A pbuf láncba fűzhető, így lehetővé téve a csomagok több pbuf-ban történő kiterjesztését,

![pbuf_structure](https://github.com/Lyque/diplomaterv/raw/LwIP/Documents/Jegyzetek/01_pbuf_structure.PNG "Pbuf structure")

- __next:__ a következő pbuf-ra mutató pointert tartalmazza,
- __payload:__ a csomag adat-ra mutató pointert tartalmazza,
- __len:__ a pbuf által tartalmazott adat hosszát tartalmazza,
- __tot_len:__ a pbuf hosszát és az összes, láncban lévő pbuf hosszát Tartalmazza,
- __ref:__ 4-bites számláló, ami számolja, hány pointer mutat a pbuf-ra. A pbuf csak akkor törölhető a memóriából, ha ez a számláló nulla.
- __flags:__ 4-bit, ami mutatja a pbuf típusát.


### Pbuf típusok

- __PBUF_POOL:__
    - pbuf foglalás egy közös, előre definiált méretű készletből történik. A foglalandó adatmérettől függően egy vagy több felfűzött pbuf szükséges.
- __PBUF_RAM:__
    - pbuf foglalás dinamikusan történik a memóriában (egy folytonos memóriaterület az egész pbuf számára),
- __PBUF_ROM:__
    - nem szükséges területfoglalás az adatnak: a pbuf payload pointer a ROM-ban lévő adatra mutat, ami csak konstans adat küldésre használható.



- Csomag fogadására a PBUF_POOL felel meg. Lehetővé teszi a gyors memóriafoglalást a beérkező adat számára. Az adat méretétől függően egy vagy több felfűzött pbuf foglalás történik. A PBUF_RAM nem megfelelő a csomagok fogadására, mert a dinamikus memóriafoglalás késleltetéssel jár, és memóriatöredezettséghez vezet.
- Csomagok küldéséhez a felhasználó választhatja ki az adathoz leginkább megfelelő pbuf típust.


### Pbuf menedzsment API

- Az LwIP tartalmaz egy külön API-t a pbuf-ok kezeléséhez (pbuf.c).


#### Pbuf API függvények

- pbuf_alloc,
- pbuf_realloc,
- pbuf_ref,
- pbuf_free,
- pbuf_clen,
- pbuf_cat,
- pbuf_chain,
- pbuf_dechain,
- pbuf_header,
- pbuf_copy_partial,
- pbuf_take,
- pbuf_coalesce,
- pbuf_mamcmp,
- pbuf_memfind,
- pbuf_strstr.


## LwIP használata az Ethernet HAL driver segítségével

- Két implementáció:
    - Operációs rendszer nélkül (standalone),
    - Operációs rendszerrel,
- Mindkét implementációnál az _ethernetif.c_ köti össze az LwIP-t az STM32 Ethernet hálózati interfészével,
- Az Ethernet kezelő változót (ETH_HandleTypeDef) ls az Ethernet DMA leíró válltozót (ETH_DMADescTyoeDef), illetve az Rx/Tx buffereket szintén az _ethernetif.c_ fájlban célszerű deklarálni.


#### Ethernet interfész függvények

- low_level_init,
- low_level_output,
- low_level_input,
- ethernetif_init,
- ethernetif_input.


### Különbségek standalone és RTOS használata esetén

- Az ethernetif_input() függvény használata különbözik a két alkalmazás esetén:
    - Standalone alkalmazás esetén a függvényt a main hurokba kell helyezni, és pollingolni kell az érkező adatokat,
    - RTOS alkalmazásokban egy külön thread-ben kell várakozni egy szemaforra. A szemafort az Ethernet perifária által generált interrupt szolgáltatja.


- Az _ethernetif.c_ implementálja a periféria MSP rutinjait:
    - Alacsony szintű inicializálás (GPIO, CLK, stb.),
    - Interrupt callback függvények létrehozása.

- RTOS esetén a _sys_arch.c_ fájlt is használni kell:
    - Ebben a fájlban történik az üzenetek átadásának implementációja (mailbox, szemforok).


## LwIP konfigurálása

- Az LwIP tartalmaz egy _lwipopts.h_ fájlt, ami lehetővé teszi a stack konfigurálását,
- Nem szükséges mindent külön beállítani, a beállítatlan értékek az alapértelmezett értéküket veszik fel (_opt.h_-ban levő értékek).


### Modul támogatás

- A felhasználó kiválaszthatja az alkalmazásához szükséges modulokat, így optimalizálva a kódméretet.


### Memória konfiguráció

- Az LwIP rugalmas memóriamenedzsmentet tesz lehetővé,
- Fix méretű statikus memóriaterületet foglal le,
- Ezt a memóriaterületet darabolja fel a többféle adatstruktúra számára, amit az LwIP használ,
- Minden memóriakészlet konfigurálható úgy, hogy fix számú adatstruktúrát tároljon,
    - Ezt a sázmot az _lwipopts.h_ fájlban lehet megváltoztatni,


#### LwIP memória konfiurációs értékek

- MEM_SIZE,
- MEMP_NUM_PBUF,
- MEMP_NUMUDP_PCB,
- MEMP_NUMTCP_PCB,
- MEMP_NUM_TCP_PCB_LISTEN,
- MEMP_TCP_SEG,
- PBUF_POOL_SIZE,
- PBUF_POOL_BUFSIZE,
- TCP_MSS,
- TCP_SND_BUF,
- TCP_SND_QUEUELEN,
- TCP_WND.


## Alkalmazás fejlesztése LwIP stack-kel


### Raw API használata standalone módban


#### Működési modell

- Standalone módban a működés a folyamatos pollingoláson alapszik,
- Egy csomad érkezésekor először az Ethernet driver bufferéből az LwIP bufferébe másoljuk az adatot,
- A leggyorsabb működéshez az LwIP buffereket a pool-ból célszerű allokálni,
- Amint a fájl másolás kész, az LwIP feldolgozza azt,
- Az LwIP az alkalmazás réteggel callback függvények segítségével kommunikál,
    - Ezeket a függvényeket még a kommunikációs folyamat előtt hozzá kell rendelni,

![standalone_application_model](https://github.com/Lyque/diplomaterv/raw/LwIP/Documents/Jegyzetek/02_standalone_application_model.PNG "Standalone application model")

- TCP alkalmazáshoz a következő callback függvényeket kell hozzárendelni AZ LwIP-hez:
    - __TCP_accept:__ bejövő TCP kapcsolódási esemény,
    - __TCP_recev:__ bejövő adatcsomag esemény,
    - __TCP_sent:__ sikeres adatküldés jelzése,
    - __TCP_err:__ TCP hiba jelzése,
    - __TCP_poll:__ periodikus callback (minden 1-2 másodpercben) az alkalmazás pollingolására.


### Netconn és Socket API használata RTOS-sel


#### Működési modell

- RTOS esetén a működés valamelyest változik,
- A TCP/IP stack és az alkalmazás külön szálon futnak,
- Az alkalmazás és a stack API hívásokon kereszütl kommunikálnak,
    - Az API hívások az RTOS mailbox mechanizmusát használja a folyamatok közti kommunikációra,
- Az API hívások blokkoló hívások,
    - Az alkalmazás szála addig blokkolódik, amíg válasz nem érkezik a stack-től,
- A hálózati thread felelős, hogy a driver-től kapott csomagokat továbbítsa a TCP/IP stack felé (RTOS mailbox segítségével),
    - A csomag érkezéséről az Ethernet interrupt értesíti.

![lwip_operation_model_with_rtos](https://github.com/Lyque/diplomaterv/raw/LwIP/Documents/Jegyzetek/03_lwip_operation_model_with_rtos.PNG "LwIP operation model with RTOS")


## Alkalmazás beállítások


### PHY interfész beállításai

- Az Ethernet periféria egy külső PHY-hez csatlakozik, ami megvalósítja a kommunikáció fizikai rétegét,
- A PHY regisztereinek definiálása a HAL konfigurációs fájlban találhatóak (_stm32f4xx_hal_conf.h_),
- A PHY működhet MII vagy RMII módban,
    - A mód kiválasztásához a _MediaInterface_ paramétert kell beállítani az Init structure paraméterezésekor,


#### MAC és IP cím beállítások

- Az alapértelmezett MAC cím _00:00:00:00:00:02_,
    - Megváltoztatásához az _stm32f4xx_hal_conf.h_-ban szereplő megfelelő byte-okat kell átírni,
- Az alapértelmezett IP cím _192.168.0.10,
    - Megváltoztatásához az _stm32f4xx_hal_conf.h_-ban szereplő megfelelő byte-okat kell átírni.
    - Ha az alkalmazás DHCP-re van konfigurálva, de nem talál DHCP szervert, akkor az IP cím automatikusan a statikus IP-re lesz beállítva.


## SSI (Server Side Includes)

- Az SSI egy módszer, hogy dinamik adatot ágyazzunk be HTML kódba,
- Ezt a HTML kódba való speciális tag-ek elhelyezésével érhetjük el,
    - A tag-ek az alábbi példához hasonlóan néznek ki:
    ```
    <!--#tag-->
    ```


## CGI (Common Gateway Interface)

- A CGI egy megszokott web technika a klienstől jövő kérések feldolgozására szerver oldalon,
- Az LwIP csak a GET metódust támogatja, és 16 paraméterig használható,
- A CGI kezelő függvény szerver oldalon fut le és egy HTML file-al tér vissza, amit a HHTP szerver továbbít a kliens felé.