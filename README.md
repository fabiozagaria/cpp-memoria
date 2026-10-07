# Piccola biblioteca: da Java a C++ moderno

Un programma da console, senza dipendenze esterne, per osservare copie, riferimenti, puntatori e durata degli oggetti. Tutto il codice è commentato in italiano. È pensato per chi conosce le basi di Java: leggi una sezione, eseguila, modifica un dettaglio e osserva cosa cambia.

## Compilare ed eseguire

Serve un compilatore C++17, ad esempio GCC o Clang. Dal terminale nella cartella `cpp-memoria`:

```bash
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -g src/main.cpp -o build/biblioteca
./build/biblioteca
```

Con Clang puoi sostituire `g++` con `clang++`. `-g` include informazioni per il debugger. In alternativa, se hai CMake e un compilatore installati:

```bash
cmake -S . -B build
cmake --build build
./build/biblioteca
```

Su Windows l'eseguibile può essere `build\Debug\biblioteca.exe` con i generatori Visual Studio. I comandi diretti sopra usano una shell Unix; CMake funziona anche con MSVC.

## Percorso di lettura

Apri `src/main.cpp`: prima la classe `Libro`, poi le funzioni e infine `main()`. Le cinque sezioni sono indipendenti e ogni blocco `{ ... }` rende visibile il momento della distruzione.

1. **Valori e copie:** modifica una copia e verifica che l'originale resti diverso. Confronta i parametri `Libro`, `Libro&` e `const Libro&`.
2. **Puntatori:** osserva gli indirizzi, il significato di `&`, `*`, `->` e `nullptr`.
3. **RAII:** il contenitore gestisce gli elementi e la propria memoria.
4. **Proprietà esclusiva:** `unique_ptr` gestisce un oggetto dinamico; `std::move` trasferisce la proprietà.
5. **Proprietà condivisa:** `shared_ptr` mantiene vivo l'oggetto, `weak_ptr` permette di osservarlo senza mantenerlo vivo. Studia questa parte per ultima.

Gli indirizzi cambiano tra esecuzioni: è normale. I messaggi dei distruttori mostrano quando termina la vita degli oggetti. Un distruttore viene eseguito anche per una copia passata a una funzione.

## Confronto con Java

| Situazione | Java | C++ nell'esempio |
| --- | --- | --- |
| Creazione oggetto | `Libro a = new Libro(...)` | `Libro a{...}` crea direttamente un oggetto |
| Assegnazione oggetto | `Libro b = a` copia il riferimento | `Libro b = a` copia l'oggetto |
| Alias dell'originale | Due variabili possono riferirsi allo stesso oggetto | `Libro& b = a` è un alias |
| Parametro | Sempre per valore; per gli oggetti si copia il riferimento | Può essere valore, riferimento o puntatore |
| Assenza | Un riferimento può essere `null` | Un puntatore può essere `nullptr`; un riferimento deve essere valido |
| Memoria | Garbage collector recupera oggetti non raggiungibili | Durata degli oggetti, RAII e proprietari determinano la distruzione |
| Collezione | `ArrayList<Libro>` contiene riferimenti | `vector<Libro>` contiene oggetti per valore |
| Immutabilità | `final` impedisce la riassegnazione della variabile | `const Libro&` impedisce di modificare il Libro attraverso quell'alias |

In Java una funzione che riceve un Libro può modificarne lo stato, ma riassegnare il parametro non cambia la variabile del chiamante. In C++ `Libro` come parametro crea una copia; `Libro&` permette di modificare l'originale. Anche Java ha tipi primitivi con semantica per valore.

`final` e `const` non sono equivalenti: `final` su un riferimento Java non rende immutabile l'oggetto; un riferimento C++ non può essere associato a un altro oggetto dopo l'inizializzazione. Assegnare ad un riferimento C++ modifica l'oggetto a cui si riferisce.

## Memoria: durata, proprietà e indirizzi

Un **oggetto** contiene stato. Un **puntatore** contiene un indirizzo. Un **riferimento** è un alias. Un **proprietario** è responsabile della durata della risorsa. Tenere un indirizzo non significa possedere l'oggetto.

```text
Libro libro{...};
  libro                 oggetto locale con durata automatica
  Libro& alias = libro  alias dello stesso oggetto
  Libro* p = &libro     puntatore che prende in prestito l'oggetto

auto owner = make_unique<Libro>(...);
  owner                 smart pointer locale con durata automatica
    └── Libro           oggetto dinamico posseduto da owner
```

Gli oggetti locali con durata automatica sono normalmente sullo **stack**; le allocazioni dinamiche sono normalmente nell'**heap**. Il linguaggio descrive durate e proprietà: queste parole non garantiscono una precisa collocazione fisica dopo le ottimizzazioni. Inoltre una variabile locale `string` o `vector` può gestire un buffer allocato dinamicamente.

**RAII** significa legare la gestione di una risorsa alla vita di un oggetto: quando l'oggetto termina la propria vita, il suo distruttore rilascia la risorsa. Vale per memoria, file e lock. Vale anche quando si esce da un blocco per un'eccezione durante il normale unwinding dello stack.

Per questo non ci sono `new` e `delete` manuali nel progetto. Per impostazione predefinita usa oggetti per valore e contenitori standard. Usa `const T&` per leggere senza copie, `T&` per modificare un oggetto esistente, `T*` per un prestito che può essere assente, `unique_ptr<T>` quando serve proprietà dinamica esclusiva. `shared_ptr<T>` richiede un motivo concreto di condivisione della proprietà.

`std::move` non sposta da solo: rende possibile scegliere operazioni di trasferimento. Nell'esempio con `unique_ptr` il nuovo proprietario acquisisce lo stesso Libro e il vecchio diventa vuoto. Per altri tipi, un oggetto dopo uno spostamento non è necessariamente vuoto. La classe Libro ha un distruttore didattico: qui non mostriamo lo spostamento di Libro stesso; normalmente preferiremmo la **regola dello zero**, lasciando ai membri standard copia, spostamento e distruzione.

## Errori da riconoscere

- Un puntatore **dangling** punta a un oggetto la cui vita è terminata. Non viene azzerato automaticamente. Dereferenziarlo ha comportamento indefinito: non è garantita un'eccezione come in Java.
- `nullptr` si può controllare; un indirizzo diverso da null non prova che l'oggetto sia ancora vivo.
- Non restituire un riferimento o un puntatore a una variabile locale: alla fine della funzione l'oggetto non esiste più.
- La riallocazione di un `vector` invalida riferimenti e puntatori ai suoi elementi. Gli smart pointer non rendono sicuro ogni accesso.
- Cicli di `shared_ptr` possono impedire la distruzione: il conteggio dei proprietari non è un garbage collector.

## Esercizi da fare da solo

1. Commenta la chiamata a `presta_originale`: prevedi quali stampe cambieranno.
2. Aggiungi `void restituisci()` alla classe e una funzione che riceve `Libro&`.
3. Aggiungi un terzo libro al vector. Non usare riferimenti conservati prima dell'aggiunta.
4. Decommenta la copia vietata del `unique_ptr`, leggi l'errore e poi ripristina il codice.
5. Togli `nuovo_proprietario.reset()` e osserva quando viene chiamato il distruttore.
6. Chiama `osservatore.lock()` nella sezione 5 prima di `primo.reset()`: osserva il ramo eseguito. Il risultato di `lock()` mantiene vivo l'oggetto finché quello smart pointer esiste.

Per cercare accessi illeciti durante gli esperimenti, con GCC/Clang che supportano i sanitizer:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -fno-omit-frame-pointer src/main.cpp -o build/biblioteca-check
./build/biblioteca-check
```

I sanitizer aiutano a trovare errori durante l'esecuzione, ma non dimostrano che ogni possibile percorso del programma sia corretto.
