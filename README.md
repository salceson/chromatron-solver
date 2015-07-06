Michał Ciołczyk

# Algorytmy dla Problemów Trudnych Obliczeniowo

# Chromatron - Dokumentacja

## Ogólny zarys rozwiązania

Rozwiązanie polega na niepełnym przeglądzie wszystkich możliwych kombinacji ustawień luster na planszy.

Przegląd ten jest ograniczony do:

* sprawdzania tylko tych pól, na których jest światło
* sprawdzania tylko tych ustawień luster, które odbiją światło

## Pseudokod (a'la Python)

```python
def solve:
    computeLights()
    if targetsAreLighted():
        return true
    for lightedBoardCell in board:
        for deviceType in deviceTypes:
            for direction in goodDirections(deviceType, lightedBoardCell)
                setDevice(lightedBoardCell, direction, deviceType)
                if solve():
                    return true
                unsetDevice(lightedBoardCell, direction, deviceType)
    return false
```

## Ważniejsze funkcje użyte w rozwiązaniu

* computeLights - funkcja przeprowadza tzw. ray-tracing od źródeł światła, zaznaczając na planszy, jakie kolory są na
danym polu
* propagateLight - funkcja dokonuje propagacji danego (pojedynczego) promienia świetlnego
* propagationDirection - funkcja wylicza, czy i w jakim kierunku propaguje się promień w danym polu
* getGoodDirectionsForDevice - funkcja dla danego pola wyznacza, w jakich kierunkach można odbić promień danym rodzajem
lustra i zwraca odpowiednie ustawienia lustra
* solve - funkcja rekurencyjna dokonująca przeglądu rozwiązania
* printBoard - wypisuje rozwiązanie
* readBoardDescription - wczytuje dane wejsciowe

## Cache'owanie

Ponieważ funkcja solve próbuje wstawić kolejne lustra na plansze, po każdym wstawieniu i zdjęciu w przypadku
niepowodzenia, powinniśmy ponownie przeliczyć światła na planszy. Zamiast to robić, światła (przed ustawieniem w danym 
kroku luster) są obliczane raz i cache'owane przez całą funkcję solve.
