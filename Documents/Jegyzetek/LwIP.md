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
- A pbuf lehetővé teszi a dinamikus memóriafoglalást a csomgaok tárolására,
- A pbuf láncba fűzhető, így lehetővé téve a csomagok több pbuf-ban történő kiterjesztését,

![pbuf_structure](https://github.com/Lyque/diplomaterv/raw/LwIP/Documents/Jegyzetek/01_pbuf_structure.PNG "Pbuf structure")

- __next:__ a következő pbuf-ra mutató pointert tartalmazza,
- __payload:__ a csomag adat-ra mutató pointert tartalmazza,
- __len:__ a pbuf által tartalmazott adat hosszát tartalmazza,
- __tot_len:__ a pbuf hosszát és az összes, láncban lévő pbuf hosszát Tartalmazza,
- __ref:__ 4-bites számláló, ami megmutatja, hány pointer mutat a pbuf-ra. A pbuf csak akkor törölhető a memóriából, ha ez a számláló nulla.
- __flags:__ 4-bit, ami mutatja a pbuf típusát.


### Pbuf típusok

- PBUF_POOL:
    - pbuf foglalás egy közös, előre definiált méretű készletből történik. A foglalandó adatmérettől függően egy vagy több felfűzött pbuf szükséges.
- PBUF_RAM:
    - pbuf foglalás dinamikusan történik a memóriában (egy folytonos memóriaterület az egész pbuf számára),
- PBUF_ROM:
    - nem szükséges területfoglalás az adatnak: a pbuf payload pointer a ROM-ban lévő adatra mutat, ami csak konstans adat küldésre használható.


- Csomag fogadására a PBUF_POOL megfelelő. Lehetővé teszi a gyors memóriafoglalást a beérkező adat számára. Az adat méretétől függően egy vagy több felfűzött pbuf foglalás történik. A PBUF_RAM nem megfelelő a csomagok fogadására, mert a dinamikus memóriafoglalás késleltetéssel jár, és memóriatöredezettséghez vezet.
- Csomagok küldéséhez a felhasználó választhatja ki az adathoz leginkább megfelelő pbuf típust.