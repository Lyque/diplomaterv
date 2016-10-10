# LwIP jegyzet

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
        - 