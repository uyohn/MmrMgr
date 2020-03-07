# Zadanie 1 - malloc implementácia

## Požiadavky

1. Dokumentácia   **3b** *čo, prečo, ako, pre / proti,
                          odhad pamäťovej a časovej náročnosti
1. Program        **8b** *implementácia
1. Testovanie     **4b**

**Minimálne 6/15 bodov**
Za lepšiu implementáciu, bonusové body - môžu pomôcť pri nasledujúcich
zadaniach, avšak maximálne 40b za semester

## Implementácia

1. `*p`   malloc
1. `int`  free
1. `int`  init
1. `*p`   check
1. Povolený **1** globálny pointer na začiatok pamäte

32B mem:
++----------++------++--------------------+
||    8B    ||  4B  ||     FREE MEM       |
++----------++------++--------------------+
^           ^       ^

## Štruktúra

1. Spájaný zoznam - 4b

