# Zadanie MapReduce

* Do zadania potrzebne będą zbiory danych tekstowych o wielkości 1G, 10G i 20G. Proszę wykorzystać plik `gutenberg-500M.txt.gz` (po rozpakowaniu 500M). W celu uzyskania większych danych wystarczy kopiować ten plik odpowiednią liczbę razy. 

* Komenda do bezpośredniego pobrania pliku: 
    ```
    wget --no-check-certificate  "https://drive.usercontent.google.com/download?id=1smWPl3RMS9LwStvMHNF0wnhTDtLawBS0&confirm=t"  -O "gutenberg-500M.txt.gz"
    ```

* Proszę wykonać **jak najbardziej wydajną sekwencyjną implementację algorytmu word count** (język programowania dowolny, ale jego wybór może mieć znaczenie) i zmierzyć czas jej działania dla w/w zbiorów danych. Proszę to zrobić na maszynie EC2 o takim samym typie instancji, jaka będzie używana w klastrze EMR. Może to być węzeł Primary (ale upewnić się, że nie jest on obciążony) lub instancja przygotowana osobno przez usługę EC2. 

* Proszę uruchomić te same obliczenia z użyciem Hadoop na klastrze EMR w co najmniej dwóch różnych konfiguracjach o tej samej liczbie rdzeni (np. 2 mocniejsze i 4 słabsze węzły robocze). Proszę wykorzystać tyle rdzeni na ile pozwalają limity. Zmierzyć czas obliczeń. 

* Wszystkie pomiary proszę zapisać w jednym pliku CSV o następujących kolumnach: 

* **nCores** - liczba rdzeni 

* **confid** - jakiś identyfikator konfiguracji (słabsze węzły / mocniejsze węzły) 

* **dataSize** - rozmiar danych wejściowych (1, 10G, 20G) 

* **time** - czas obliczeń 

* Korzystając z tego pliku proszę zrobić wykresy średniego czasu obliczeń dla każdego rozmiaru danych wejściowych i konfiguracji (w tym wersja sekwencyjna); polecane narzędzie Pandas+matplotlib po kolumnach nCores i confid należy zrobić groupBy i dla każdej grupy wyliczyć średni czas - zgodnie z metodą split-apply-combine). 

* Czy w którymś przypadku da się określić metrykę COST (wyrażoną liczbą rdzeni)? 

### Uwagi:
* Proszę pamiętać, że punktem odniesienia (baseline) powinien być czas obliczeń jak najlepszej implementacji sekwencyjnej na pojedynczym węźle. 
* Proszę pamiętać o właściwej metodologii pomiaru (kilka pomiarów i obliczenie średniej/stdev - ze względu na ograniczenia wystarczą 2 pomiary) 
* Większe dane można umieścić w HDFS kopiując ten sam plik (polecenie hdfs dfs -put) wiele razy z jednoczesną zmianą nazwy. Uwaga: proszę upewnić się gdzie na węźle Primary jest odpowiednia ilość miejsca na dane (prawdopodobnie na **/mnt1**). 
* Dane mogą być przesyłane na klaster EMR na wiele sposobów, opisanych na How to get data into Amazon EMR - Amazon EMR. W szczególności mogą pochodzić z bucketu S3. Przykład polecenia (z pomiarem czasu): 
    ```
    time hadoop jar /usr/lib/hadoop/hadoop-streaming.jar -files s3://elasticmapreduce/samples/wordcount/wordSplitter.py -mapper wordSplitter.py -reducer aggregate -input s3://elasticmapreduce/samples/wordcount/input -output wordcount-output
    ``` 

Jako wynik proszę załączyć sprawozdanie z pomiarów przedstawiające parametry mierzonych konfiguracji, wykresy przyspieszenia, odpowiedzi na pytania oraz inne komentarze do wyników. Proszę również dołączyć plik CSV z pomiarami i kod generujący wykresy. 

### Ocena:
* Opis sekwencyjnej implementacji - uzasadnienie, że została zaimplementowana optymalnie (2 pkt) 
* Wykonanie pomiarów we wszystkich konfiguracjach (5 pkt) 
* Sprawozdanie (5 pkt) 
* Poprawność metodologiczna (3 pkt)
