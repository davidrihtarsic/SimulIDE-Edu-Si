# Serial Terminal: podatkovni vodili Tx-bus in Rx-bus

V lastnostih komponente **Serial Terminal** vključite **Show data buses**.
Na desni strani se prikažeta dva izhodna priključka:

- **Tx-bus**: zadnji bajt, ki ga je terminal dokončno oddal na svoj Tx;
- **Rx-bus**: zadnji bajt, sprejet na terminalov Rx.

Oba priključka sta 8-bitni vodili z bitom 0 kot najmanj pomembnim bitom.
Podatek ostane na izhodu do naslednjega bajta; ob ponovnem zagonu simulacije
se vodili ponastavita na nič. Oznaki Tx in Rx sta z vidika terminala, zato se
podatki, ki jih Arduino odda v terminal, pojavijo na **Rx-bus**.

## Povezava z analizatorjem

1. Ustavite simulacijo in vključite **Show data buses** na terminalu.
2. Na **Logic Analyzerju** kliknite **Expand**.
3. V vrstici **Bus** vključite kanal 1 in kanal 2, preden naredite povezavi.
4. Povežite **Tx-bus → kanal 1** in **Rx-bus → kanal 2**. Dodatna komponenta
   Bus ni potrebna.
5. Izberite **Trigger: None** in ustrezno časovno merilo; pri 9600 baud
   začnite z **Time Div = 1 ms**.
6. Zaženite simulacijo. Analizator prikazuje vrednosti v obliki `0x00`,
   `0x05`, `0x41` ali `0xFF`. Oznaka se izpiše, ko je odsek dovolj širok.

Za opazovanje serijskih bitov priključite terminalov Tx ali Rx še na dodaten
običajni kanal analizatorja. HEX vrstica se spremeni ob dokončanem bajtu;
ne predstavlja oznake, raztegnjene čez njegove serijske bite.

## Preprost preizkus brez mikrokrmilnika

Pripravljena shema: [serial-terminal-buses.sim2](../examples/serial-terminal-buses.sim2).
Za podrobnejši pregled v njej zmanjšajte **Time Div** s 100 ms na približno 1 ms.

Povežite terminalov **Tx neposredno na njegov Rx**. Nastavite **9600 baud**,
**8 podatkovnih bitov** in **1 stop bit**. Na analizator priključite obe vodili.

Odprite terminal z gumbom **Open**, za pošiljanje izberite način **HEX** ter
pošljite `41 05 00 FF 80`. Oba bus kanala morata prikazati enako zaporedje;
sprejem se lahko zaključi nekoliko pred koncem oddajnega okvirja.
Po zadnjem bajtu ostane na obeh vodilih `0x80`. Uporabite premor in časovno
približanje za pregled kratkih odsekov.

## Omejitve

- To sta podatkovna izhoda, ne dodatna serijska vhoda.
- Zaporedna enaka bajta, npr. `FF FF`, ne povzročita novega prehoda na vodilu.
  Za štetje posameznih bajtov bi bil potreben še ločen impulz veljavnosti.
- Pri nastavitvah UART z več kot osmimi podatkovnimi biti sta izhoda omejena
  na osem bitov, enako kot obstoječa bajtna vmesnika terminala.
- Izklop **Show data buses** odklopi bus povezavi in po potrebi ustavi
  simulacijo. Obstoječa priključka Tx in Rx ostaneta na svojih mestih.
