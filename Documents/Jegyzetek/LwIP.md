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

## 