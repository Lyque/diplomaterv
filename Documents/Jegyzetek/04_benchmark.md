# Teljesítménymérő metrikák

Asztali alkalmazás fejlesztésekor a rendelkezésre álló teljesítmény és tárhely ma már nem számít akadálynak. Ha valamelyik erőforrás szűk keresztmetszetté válik, akkor alkatrészcserével általában megszüntethető a probléma.

Nem ez a helyzet beágyazott rendszerek esetén. A rendszer központi egységének számítási kapacitása általában nem haladja meg nagy mértékben az elégséges szintet, így különösen figyelni kell a fejlesztés során, hogy az implementált kód hatékony legyen. A rendelkezésre álló memória sem tekinthető korlátlannak, és gyakran a bővítés is nehézkes, esetleg nem megoldható. Az eszköz fogyasztása is fontos szempont, amit a szoftver szintén nagy mértékben befolyásolhat.

Az operációs rendszer választása összetett folyamattá is bonyolódhat, mert mérlegelni kell az alkalmazásunk igényeit, az operációs rendszer támogatottságát (támogatott mikrokontrollerek, fórumok, gyártói támogatás), a becsülhető fejlesztési időt és az ezzel járó költségeket, illetve fizetős operációs rendszer esetén a rendszer árát.

A választást az sem segíti előre, hogy nincs egyértelmű módszer az operációs rendszerek értékelésére. [lábjegyzet->
Bár a Német Szabványügyi Intézet (Deutsche Institut für Normung - DIN)
az 1980-as évek végén hozott létre szabványt a folyamatirányító számítógépes rendszerek teljesítménymutatóinak mérésére (DIN 19242 szabvány-sorozat), a valós idejű operációs rendszerek értékelésére ez nem jelent megoldást.]

Az alkalmazott mérési folyamatnak több szempontnak is eleget kell tennie, hogy az eredmény használható legyen. Egy mérés során több forrásból is eredhet hiba, melyek mértékét szeretnénk a lehető legkisebb szintre csökkenteni.
A kulcsfontosságú szempontok az alábbiak:
- Megismételhetőség: egy mérést meg kell tudnunk ismételni. Ehhez szükséges a pontos mérési összeállítás, a mérés körülményei, a használt eszközök és szoftverek.
- Véletlenszerűség: a mérés során nem független események következhetik egymást, amik a mérés eredményét befolyásolják. Ezeket csak ritkán lehet teljes mértékben kiküszöbölni, ezért törekedni kell a mérési folyamatok véletlenszerű futtatására (például méréssorozat esetén az egyes folyamatok ne mindig ugyan abban a sorrendben kövessék egymást).
- Vezérlés: a mérés során a vezérelhető paramétereket (melyek a mérést befolyásolhatják) lehetőségeinkhez mérten kézben kell tartani.
- Szemléletesség: a mérés eredményének reprezentatívnak kell lennie. Számértékek esetén két mérés eredményét össze kell tudnunk hasonlítani és tudnunk kell relációt vonni a két érték közé.

A továbbiakban különböző forrásokból vett szempontokat vizsgálok meg, majd azok alapján állítom fel a dolgozat során megfigyelt tulajdonságok listáját.


## Memóriaigény

A mikrokontrollerek területén a memória mérete korlátozott (ROM és RAM egyaránt), ezért fontos, hogy a használt rendszer minél kisebb lenyomattal rendelkezzen.


## Késleltetés

A rendszer késleltetése az az idő, ami egy esemény beérkezésétől a rendszer válaszáig eltelik. Ezt okozhatja a mikrovezérlő megszakítási mechanizmusához szükséges műveletek sora, az operációs rendszer ütemezőjének overhead-je, de a közben végrehajtandó feladat is nagy mértékben befolyásolja a nagyságát.

![latency](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Benchmark/01_latency.png "Latency")

## Jitter

A jitter egy folyamat vizsgálata során a többszöri bekövetkezés után mért késleltetésekből határozható meg.

![jitter](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Benchmark/02_jitter.png "Jitter")


## Rhealstone

1989-ben a Dr. Dobbs Journal cikkeként jelent meg egy javaslat, ami a valós idejű rendszerek objektív értékelését célozta meg. Rabindra P. Kar, az Intel Systems Group senior mérnöke ismertette a módszer előnyeit és szempontjait, melynek a Rhealstone nevet adta.[lábjegyzet-> név eredete]

A cikk megjelenésekor már léteztek teljesítménymérő metódusok (Whetstone, Dhrystone), de ezek a fordító által generált kódot, illetve a hardware-t minősítették. A Rhealstone metrika célja, hogy a fejlesztőket segítse az alkalmazásukhoz leginkább megfelelő operációs rendszer kiválasztásában.

A Rhealstone hat kategóriában vizsgálja meg az operációs rendszer képességeit:
- Taszkváltási idő (Task switching time),
- Preemptálási idő (Preemption time),
- Megszakítás-késleltetési idő (Interrupt latency time),
- Szemafor-váltási idő (Semaphore shuffling time),
- Deadlock-feloldási idő (Deadlock breaking time) [lábjegyzet-> Nincs kialakult holtpont, igazából a prioritásinverzió jelenségét vizsgálja],
- Datagram-átviteli idő (Datagram throughput time) [lábjegyzet-> Bár időnek van feltüntetve, kB/sec-ben értendő. Minden kategóriának a reciprokát kell venni a mréés végén, így Rhealstone/sec értéket kapva. Ha az 1kB-hoz tartozó időt mérjük, akkor minden úgy történik, mint a többi kategóriánál.]

1990-ben megjelent egy második cikk is, amely amellett, hogy az egyes kategóriák példaprogramjait tartalmazta (iRMX operációs rendszerhez), pár kategória meghatározását megváltoztatták. Ezen változtatásokat az adott kategória részletezésekor ismertetem.

### Taszkváltási idő

A taszváltási idő a két független, futásra kész, azonos prioritású taszkok váltásához szükséges átlagos időtartam.

![task_switching_time](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Benchmark/03_task_switching_time.png "Task switching time")

A taszkváltási idő alapvető jellemzője egy multitaszk rendszernek. A mérés a taszkokat nyilvántartó struktúrák hatékonyságáról ad képet. A taszkváltási időt a használt processzor architektúrája, utasításkészlete is befolyásolja.

A rendszerek a futtatható taszkokat általában valamilyen listában tárolják, így különböző számú taszkkal elvégezve a mérést más eredményt kaphatunk.


### Preemptálási idő

A preemptálási idő egy magasabb prioritású taszk érvényre jutásához szükséges átlagos időtartam.

![preemption_time](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Benchmark/04_preemption_time.png "Preemption time")

A preemptálási idő nagyban hasonlít a taszkváltási időhöz, azonban a járulékos utasítások miatt általában hosszabb időt jelent.


### Megszakítás-késleltetési idő

A megszakítás-késleltetési idő egy esemény beérkezése és a megszakítás kezelő rutin első utasítása között eltelt átlagos időtartam.

![interrupt_latency_time](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Benchmark/05_interrupt_latency_time.png "Interrupt latency time")


### Szemafor-váltási idő

Az 1989-es cikk szerint szemafor-váltási idő az az átlagos időtartam, ami egy szemafor elengedése és egy, a szemaforra várakozó taszk elindulása között eltelik.

![semaphore_shuffling_time_1](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Benchmark/06_semaphore_shuffling_time_1.png "Semaphore shuffling time 1")

Ezt a meghatározást 1990-ben annyival módosították, hogy a szemafor-váltási idő egy már birtokolt szemafor kérése és a kérés teljesítése között eltelt időtartam, a birtokló taszk futási idejétől eltekintve.

![semaphore_shuffling_time_2](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Benchmark/07_semaphore_shuffling_time_2.png "Semaphore shuffling time 2")

A mérés célja az overhead meghatározása, mikor egy szemafor kölcsönös kizárást (mutex) valósít meg.


### Deadlock-feloldási idő

A deadlock-feloldási idő az az átlagos időtartam, ami egy olyan erőforrás eléréséhez szükséges, amit egy alacsonyabb prioritású taszk már birtokol.

![deadlock_breaking_time](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Benchmark/08_deadlock_breaking_time.png "Deadlock breaking time")

Vagyis a deadlock-feloldási idő a birtoklási probléma feloldásához szükséges összesített idő egy alacsony és egy magas prioritású taszk között.


### Datagram-átviteli idő

A datagram-átviteli idő a taszkok között elérhető adatsebesség az operációs rendszer objektumait kihasználva (vagyis nem megosztott memórián vagy pointeren keresztül). Az adatküldő taszknak kapnia kell értesítést az adat átvételéről.

![datagram_throughput_time](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Benchmark/09_datagram_throughput_time.png "Datagram throughput time")

Az egy évvel később megjelent cikkben ezt a kategóriát is módosították kis mértékben. Egyrészt a megnevezést taszk közötti üzenet-késleltetésre változtatták, másrészt nem a maximális adatsebesség meghatározása a mérés célja, hanem az adattovábbítást végző objektum kezelésének és az operációs rendszer járulékos műveleteinek hatékonyságának megmérése.

![intertask_message_latency](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Benchmark/10_intertask_message_latency.png "Intertask message latency")


### Rhealstone jellemzők összegzése

Az elvégzett mérések várhatóan mikroszekundum és milliszekumdum nagyságrendű értékeket adnak vissza. Minden értéket másodpercre váltva, majd a reciprokát véve az értékek összegezhetőek egy reprezentatív értékké. Az átváltásnak köszönhetően a nagyobb érték jobb teljesítményt jelent, ami a teljesítménymutatók világában elvárt.

#### Objektív Rhealstone érték

Objektív értékelés esetén minden jellemző azonos súllyal szerepel a számolás során.

[Képlet]


#### Súlyozott Rhealstone érték

Az esetek döntő többségében a vizsgált feladatok nem azonos gyakorisággal szerepelnek egy alkalmazásban, sőt, előfordulhat, hogy valamely funkciót nem is használ az alkalmazás. Ekkor informatívabb eredményt kapunk, ha az egyes jellemzőkre kapott számértékeket különböző súllyal vesszük bele a végeredmény meghatározásába.

[Képlet]


## Legrosszabb válaszidő

2001-ben a Nemzetközi Automatizálási Társaság (International Society of Automation - ISA) egy jelentésben fejtette ki azt az álláspontját, miszerint a késleltetés nem jellemzi a valós idejű rendszert, mert lehet, hogy a legtöbb esetben az előírt időn belül válaszol, de ritkán előfordulhatnak késleltetések vagy kihagyott események, amiket a mérés során nem lehet detektálni.

A Társaság egy olyan mérési összeállítást javasol a fejlesztőknek, ami egyszerűsége ellenére képes meghatározni a rendszer legrosszabb válaszidejét.

A méréshez szükséges eszközök:
- Mérendő rendszer (legalább egy be- és kimenettel),
- Jelgenerátor.
- Két darab digitális számláló.


### Mérési elrendezés

A jelgenerátor kimenetét a mérendő rendszer bemenetére, illetve mindkét számláló __count up__ bemenetére kötjük, míg a mérendő rendszer kimenetét a kimeneti számláló __count down__ bemenetére kötjük.

![worst_case_response_time](https://github.com/Lyque/diplomaterv/raw/master/Documents/Jegyzetek/Figures/Benchmark/11_worst_case_response_time.png "Worst-case response time")


### Mérési elv

A mérés során azt a legkisebb frekvenciát keressük, amit a rendszer már nem tud követni, vagyis impulzusokat veszít. Ezáltal a kimenetén megjelenő impulzusok száma különbözni fog a bemenetére adott impulzusok számától. A kapott frekvencia a a legrosszabb válaszidő reciproka.


### Mérés menete

1. A rendszeren futó program a bemenetére érkező jelet a kimenetére másolja. A mérés során digitális és analóg I/O láb is használható.
2. Mérési eszközök csatlakoztatása.
3. Alacsony frekvenciáról indulva növeljük a bemeneti jel frekvenciáját. Az _A_ számláló folyamatosan számol felfele. Amíg a rendszer képes követni a bemenetet, addig a _B_ számláló 1 és 0 között váltakozik. Amikor a rendszer már nem képes követni a bemenetet, akkor a _B_ számláló elkezdt felfele számolni.
4. Csökkentjük a bemeneti frekvencia értékét egészen addig, amíg a rendszer újból képessé nem válik a bemenet követésére. A kapott frekvencia a legrosszabb válaszidő reciproka.

A mérést célszerű elvégezni különböző terhelés mellett. Ha valamelyik funkció használata közben (adattároló vezérlése, hálózati kommunikáció, stb.) a _B_ számláló elindul, akkor az adott frekvencián a rendszer nem determinisztikus.


## Vizsgált operációs rendszer jellemzők

A feladat megoldása során elsődlegesen az operációs rendszerek jellemzőit vizsgálom, ezért nem kerülnek külön tesztelésre az egyes hardverek előnyei. Az egyes rendszer-jellemzőket terheletlenül és terhelés alatt is megmérem.

Egy ipari alkalmazás szimulációját is megvalósítom, mely egy másik összehasonlítási alapot nyújt a dolgozathoz. Az alkalmazást felhasználom a terhelés alatti mérés megvalósításához is.

A dolgozat további részeiben a [referencia] fejezetben felsorolt összes jellemző meghatározására képes rendszert állítok össze, mellyel végrehajtom a méréseket. A meghatározandó jellemzők:
- Memóriaigény,
- Késleltetés,
- Jitter,
- Rhealstone értékek,
- Legrosszabb válaszidő.


--------------------------------------------------------------------------

# Saját mérések

- Külső mérőegység alkalmazása -> működés közben tesztelhető,
- Késleltetések mérése,
- Jitter,
- Rhealstone mérések:
    - Taszkváltási idő,
    - Preemptálási idő,
    - Megszakítás-késleltetési idő,
    - Deadlock-feloldási idő,
    - Datagram-átviteli idő:
        - 1kB adat átvitele queue-n keresztül,
- Fordítás után a gépi kód elemzése -> a láb billentgetéséért felelős kódsorok idejét kompenzálni lehet.

# Operációs rendszer specifikus

- Memóriaigény:
    - Skálázhatóság,
- Késleltetés:
    - Megszakítás,
    - Ütemezés,
- Kernel szolgáltatások:
    - Kommunikációs objektumok kezelése,
- Kódhordozhatóság,
- Valós idejűség,
- Megbízhatóság.


# Mikrokontroller specifikus

- Aritmetikai utasítások elvégzése különböző számábrázolások esetén:
    - 32/64 bites számok,
    - Lebegőpontos aritmetika,
- Megszakítások:
    - Késleltetés,
    - Prioritás,
    - Egymásba ágyazás,
- Grafikai gyorsítás.




# Források

- [Embedded OS metrics](http://www.embedded.com/design/operating-systems/4437792/Why-and-how-to-measure-your-RTOS-performance)
- PDF-ek
