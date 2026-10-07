// Biblioteca didattica: C++17 moderno, senza new/delete manuali.
// Leggi prima README.md, poi segui le sezioni numerate di main().
#include <iostream> // std::cout: scrive sulla console.
#include <memory>   // Smart pointer: unique_ptr, shared_ptr, weak_ptr.
#include <string>   // Stringhe che gestiscono automaticamente la propria memoria.
#include <utility>  // std::move: permette di trasferire risorse.
#include <vector>   // Contenitore dinamico che gestisce la propria memoria.

// In Java una variabile Libro contiene un riferimento; qui può contenere l'oggetto.
class Libro {
public:
    // Il parametro è per valore: poi spostiamo la stringa nel membro.
    explicit Libro(std::string titolo) : titolo_(std::move(titolo)) {
        std::cout << "  Costruito: " << titolo_ << '\n';
    }

    // Il distruttore viene chiamato quando finisce la vita dell'oggetto.
    // Serve solo a rendere visibile il fenomeno: normalmente non lo scriveremmo.
    ~Libro() {
        std::cout << "  Distrutto: " << titolo_ << '\n';
    }

    // Restituisce una copia: il chiamante ha una stringa indipendente.
    // const dopo () significa che questo metodo non modifica l'oggetto.
    std::string titolo() const { return titolo_; }

    // Modifica lo stato di questa precisa istanza.
    void presta() { prestato_ = true; }

    // bool è simile al boolean di Java.
    bool prestato() const { return prestato_; }

private:
    std::string titolo_;       // Ogni Libro possiede la propria stringa.
    bool prestato_ = false;    // Inizializziamo sempre i membri.
};

// Passaggio per valore: viene creata una copia del Libro originale.
void presta_copia(Libro libro) {
    libro.presta(); // Cambia soltanto la copia, distrutta al ritorno.
}

// Passaggio per riferimento: nessuna copia, modifichiamo l'originale.
// Il riferimento è un alias e deve riferirsi a un oggetto valido.
void presta_originale(Libro& libro) {
    libro.presta();
}

// const &: leggiamo l'originale senza copiarlo e senza poterlo modificare.
void mostra(const Libro& libro) {
    std::cout << "  " << libro.titolo()
              << " | prestato: " << libro.prestato() << '\n';
}

// Un puntatore non proprietario consente anche l'assenza di un oggetto.
// Non alloca, non libera e non prolunga la vita del Libro puntato.
void mostra_se_presente(const Libro* libro) {
    if (libro != nullptr) { // Verifica prima di dereferenziare.
        mostra(*libro);    // * ottiene l'oggetto a quell'indirizzo.
    } else {
        std::cout << "  Nessun libro selezionato\n";
    }
}

int main() {
    std::cout << std::boolalpha; // Stampa true/false invece di 1/0.

    std::cout << "\n1. Valore, copia e riferimento\n";
    {
        Libro originale{"Il piccolo principe"}; // Oggetto con durata automatica.
        Libro copia = originale;                // Copia indipendente in C++.
        copia.presta();                          // Non cambia originale.
        mostra(originale);
        mostra(copia);

        presta_copia(originale); // Un'altra copia: originale resta disponibile.
        mostra(originale);
        presta_originale(originale); // Libro& permette di modificare originale.
        mostra(originale);
    } // Distruzione automatica in ordine inverso: copia, poi originale.

    std::cout << "\n2. Indirizzi e puntatori non proprietari\n";
    {
        Libro libro{"1984"};
        Libro& alias = libro;       // & nella dichiarazione: riferimento.
        Libro* puntatore = &libro;  // & nell'espressione: indirizzo dell'oggetto.
        std::cout << "  Indirizzo libro: " << &libro << '\n';
        std::cout << "  Indirizzo alias: " << &alias << '\n'; // Identico.
        std::cout << "  Indirizzo contenuto nel puntatore: " << puntatore << '\n';
        puntatore->presta();        // -> equivale a (*puntatore).presta().
        mostra_se_presente(puntatore);
        puntatore = nullptr;       // Svuota il puntatore; NON distrugge libro.
        mostra_se_presente(puntatore);
    } // libro termina la propria vita qui: non conservarne puntatori fuori.

    std::cout << "\n3. Contenitori e RAII\n";
    {
        std::vector<Libro> scaffale; // Il vector possiede i propri elementi.
        scaffale.reserve(2);        // Spazio per due elementi senza riallocazioni.
        scaffale.emplace_back("Dune"); // Costruzione direttamente nel vector.
        scaffale.emplace_back("Lo Hobbit");
        for (const Libro& libro : scaffale) { // Legge senza copiare gli elementi.
            mostra(libro);
        }
        // Aggiunte oltre la capacità possono invalidare riferimenti/puntatori.
        // reserve(2) non è un limite: una terza aggiunta può riallocare.
    } // Il vector distrugge gli elementi e libera il proprio buffer.

    std::cout << "\n4. unique_ptr: un solo proprietario\n";
    {
        auto proprietario = std::make_unique<Libro>("Fondazione");
        // proprietario è uno smart pointer locale; Libro ha durata dinamica.
        // make_unique alloca e consegna la proprietà allo smart pointer.
        mostra(*proprietario);

        // Non è permesso copiarlo: due unique_ptr non possono possedere lo stesso Libro.
        // auto impossibile = proprietario; // Decommenta per vedere l'errore.
        auto nuovo_proprietario = std::move(proprietario);
        // Il Libro non si sposta in memoria: si trasferisce la sua proprietà.
        // Dopo questo trasferimento, il vecchio unique_ptr è vuoto.
        std::cout << "  Vecchio proprietario vuoto: "
                  << (proprietario == nullptr) << '\n';
        mostra(*nuovo_proprietario);

        const Libro* osservatore = nuovo_proprietario.get(); // Prestito, non proprietà.
        mostra_se_presente(osservatore);
        nuovo_proprietario.reset(); // Distrugge subito il Libro posseduto.
        // osservatore ora è dangling: non leggerlo/dereferenziarlo!
        osservatore = nullptr; // Azzera questa variabile, non eventuali altri alias.
    } // Nessun delete: lo smart pointer avrebbe liberato l'oggetto automaticamente.

    std::cout << "\n5. shared_ptr e weak_ptr: caso opzionale\n";
    {
        auto primo = std::make_shared<Libro>("Il nome della rosa");
        std::weak_ptr<Libro> osservatore = primo; // Non mantiene vivo il Libro.
        {
            auto secondo = primo; // Condivide lo stesso oggetto, non lo copia.
            std::cout << "  Proprietari: " << primo.use_count() << '\n'; // 2.
            secondo->presta();
            mostra(*primo); // La modifica è visibile attraverso entrambi.
        } // secondo scompare; resta primo.
        primo.reset(); // Ultimo proprietario: il Libro viene distrutto.
        if (auto temporaneo = osservatore.lock()) { // Acquisisce proprietà se vivo.
            mostra(*temporaneo);
        } else {
            std::cout << "  weak_ptr: il Libro non esiste più\n";
        }
        // Preferisci valori/unique_ptr. shared_ptr serve alla proprietà condivisa.
        // Cicli di shared_ptr possono trattenere memoria: weak_ptr può spezzarli.
    }

    return 0; // Successo: tutte le risorse sono state gestite automaticamente.
}
