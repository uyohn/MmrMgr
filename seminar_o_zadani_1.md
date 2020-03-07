# Informacie

- max 50 000 block size
- **best-fit** - lower fragmentation
- dynamicke hlavicky? nie - offset ako int
                      ano - offset ako char, int ...
- implicitne zoznamy
- programovat testy - 4b
- nemenit mena/parametre funkcii
- hodnoti sa vhodne komentovanie/struktura

## Udrziavanie volnej pamate

1. implicitny zoznam s pouzitim dlzok
1. explicitny zoznam blokov volnej pamati pomocou
ukazovatelov zapisanych priamo vo volnych blokoch
1. oddelene zoznamy blokov volnej pamati
(rozne zoznamy pre triedy blokov volnej pamati podla dlzky)
1. bloky usporiadane podla velkosti
(vyvazeny strom, s ukazovatelom zopisanym v kazdom volnom bloku, dlzka je kluc)
