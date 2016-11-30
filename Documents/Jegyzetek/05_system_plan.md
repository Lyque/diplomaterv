# Rendszerterv

Az operációs rendszerek (xy) fejezetben felsorolt paramétereit több módszerrel meghatározhatjuk.

Az egyik kézenfekvő megoldás, ha szoftveres úton az érdekes futási szakaszokban elmentjük a rendszer órájának aktuális értékét, majd ezek ismeretében határozzuk meg a jellemzőket. Az információ kinyerése a beágyazott eszközből legegyszerűbben soros kommunikáción történhet.

Másik megoldás egy kiegészítő hardver tervezése, ami lehetővé teszi külső mérőeszköz csatlakoztatását.

Az operációs rendszerek mérése folyamán fontos, hogy maga a mérés lehetőleg ne befolyásolja a mért értékeket. Ezért a mérőrendszer és mérőfolyamat összeállításánál kiemelt figyelmet kapott, hogy a közvetlenül a méréshez kapcsolódó szoftverrészek a minimálisak legyenek.