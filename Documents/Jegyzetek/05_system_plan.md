# Rendszerterv

Az operációs rendszerek (xy) fejezetben felsorolt paramétereit több módszerrel meghatározhatjuk.

Az egyik kézenfekvő megoldás, ha szoftveres úton az érdekes futási szakaszokban elmentjük a rendszer órájának aktuális értékét, majd ezek ismeretében határozzuk meg a jellemzőket. Az információ kinyerése a beágyazott eszközből legegyszerűbben soros kommunikáción történhet.

Másik megoldás egy kiegészítő hardver tervezése, ami lehetővé teszi külső mérőeszköz csatlakoztatását.

Az operációs rendszerek mérése folyamán fontos, hogy maga a mérés lehetőleg ne befolyásolja a mért értékeket. Ezért a mérőrendszer és mérőfolyamat összeállításánál kiemelt figyelmet kapott, hogy a közvetlenül a méréshez kapcsolódó szoftverrészek a minimálisak legyenek. Ennek okán a kiegészítő harver alkalmazása mellett döntöttem.


## Magas szintű Rendszerterv

A kiegészítő hardver tervezése során a fő szempont a mérés folyamatának egyszerű felépítése volt. A feladat részét képezi egy ipari alkalmazás szimulációja, melyhez szükséges alkatrészek is ugyanezen hardveren kaptak helyet.


### Ipari alkalmazás

Az ipari alkalmazás egy helyi és egy távoli állomás megfigyelését teszi lehetővé. A helyi állomáson egy potméter, egy hőmérő, kapcsolók és indikátor LED-ek találhatóak, míg a távoli állomáson egy TI SensorTag méri a hőmérsékletet, páratartalmat és a fényviszonyokat.


