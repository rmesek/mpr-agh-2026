## Przypadek sekwencyjny

Możemy użyć rozwiązania MPI z jedym rdzeniem, ale musimy napisać w sprawozdaniu, że należałoby użyć możliwie najlepszego rozwiązania sekwencyjnego (bez MPI).

## Testujemy na Aresie
Zawsze `--nodes=1`, alokujemy jeden węzeł i 12 tasków (12 rdzeni). Ustawić ilość powtórzeń (najpierw 3x, potem 10x), najlepiej wstawiać kilka razy, a nie pętlą.

## Skalowalność silna (wg. Amdahla)

N = const = 12e10
p = 1, 2, 3, ..., 12 (równy podział pracy)

## Skalowalność słaba (wg. Gustafsona)

N ~ p
p = 1, 2, 3, ..., 12 (N = p * 1e10)