#include "Preventivo.h"
#include "VoceCosto.h"
#include <sstream>     // std::ostringstream "per costruire il riepilogo di una stringa"
#include <utility>     // std::move
#include <ctime>
#include <algorithm>
#include <iomanip>    // std::fixed, std::setprecision
#include <numeric>


static const char* descriviGrado(GradoDifficolta g) {
    switch (g) {
        case GradoDifficolta::Nuovo:
            return "Nuovo (immobile mai abitato)";
        case GradoDifficolta::Disabitato:
            return "Disabitato (vuoto, senza arredi)";
        case GradoDifficolta::Abitato:
            return "Abitato (arredato, occupato)";
        default:
            return "Stato sconosciuto";
    }
}


//Costruttore: inizializzo i membri tramite std::move per evitare copie
Preventivo::Preventivo(std::string id, std::string cliente, GradoDifficolta grado)
    : id_(std::move(id)), //sposto la stringa temporanea nel membro
      cliente_(std::move(cliente)), //idem per il nome cliente
      grado_(grado) //copio anche il grado
{}

//COPY CONSTRUCTOR: copia profonda polimorfa delle voci
Preventivo::Preventivo(const Preventivo& other)
    : id_(other.id_),
      cliente_(other.cliente_),
      grado_(other.grado_)
{
    //Pre-alloco lo spazio per le voci in modo da evitare riallocazioni multiple
    voci_.reserve(other.voci_.size());

    //Per ogni voce dell'altro preventivo, chiamo clone() (polimorfo) e inserisco il unique_ptr risultante nel mio vector
    for (const auto & vocePtr : other.voci_) {
        if (vocePtr) {
            voci_.push_back(vocePtr->clone());
        }
    }
}
//Copy Assignment Operator
Preventivo& Preventivo::operator=(const Preventivo& other) {
    //controllo autoassegnazione (a = a)
    if (this == &other) return *this;

    id_      = other.id_;
    cliente_ = other.cliente_;
    grado_   = other.grado_;
    //Svuoto le voci attuali (gli unique_ptr distruggono le vecchie voci)
    voci_.clear();
    voci_.reserve(other.voci_.size());

    //copia profonda come nel costruttore di copia
    for (const auto & vocePtr : other.voci_) {
        if (vocePtr) {
            voci_.push_back(vocePtr->clone());
        }
    }

    return *this;
}

//Move constructor: trasferisco i dati da other a this
Preventivo::Preventivo(Preventivo&& other) noexcept
    : id_(std::move(other.id_)),
      cliente_(std::move(other.cliente_)),
      grado_(other.grado_),
      voci_(std::move(other.voci_)) {}

//Move assignment operator
Preventivo& Preventivo::operator=(Preventivo&& other) noexcept {
    if (this == &other) return *this;

    id_      = std::move(other.id_);
    cliente_ = std::move(other.cliente_);
    grado_   = other.grado_;
    voci_    = std::move(other.voci_);

    return *this;
}

//Aggiunge una voce trasferendo l'ownership del unique_ptr
void Preventivo::aggiungiVoce(std::unique_ptr<VoceCosto> voce) {
    if (voce != nullptr) {
        voci_.push_back(std::move(voce)); //dopo lo std::move, "voce" è nullo
    }
}
//Sintassi alternativa: p += std::move(voce);
Preventivo& Preventivo::operator+=(std::unique_ptr<VoceCosto> voce) {
    aggiungiVoce(std::move(voce));
    return *this;
}

double Preventivo::totale() const {
    // std::accumulate(inizio, fine, valore_iniziale, operazione_binaria)
    return std::accumulate(
        voci_.begin(),
        voci_.end(),
        0.0,
        [](double acc, const std::unique_ptr<VoceCosto>& voce) {
            // La lambda somma l'accumulatore corrente col subtotale della voce
            return acc + voce->subtotale();
        }
    );
}

double Preventivo::totaleMq() const {
    // Utilizzo il mio template 'aggrega' per dimostrare la flessibilità del codice.
    // Qui aggrego le quantità (mq) invece dei prezzi.
    return aggrega(
        [](double acc, const VoceCosto& voce) {
            return acc + voce.getQuantita();
        },
        0.0 // Valore iniziale
    );
}


// Ordina le voci per nome (ordine alfabetico crescente).
// Per costruzione, in voci_ non inseriamo mai puntatori null.
void Preventivo::ordinaPerNome() {
    std::sort(
        voci_.begin(),
        voci_.end(),
        [](const std::unique_ptr<VoceCosto>& a,
           const std::unique_ptr<VoceCosto>& b) {
            // se per qualche motivo uno fosse nullo, lo mettiamo in fondo
            if (!a && !b) return false;
            if (!a)      return false; // a "dopo" b
            if (!b)      return true;  // a "prima" di b

            return a->getNome() < b->getNome();
        }
    );
}




//Riepilogo del preventivo in formato stringa
std::string Preventivo::riepilogo() const {
    std::ostringstream oss;

    // Intestazione "più da preventivo"
    oss << "EDILCOLOR - Preventivo lavori di tinteggiatura/cartongesso\n";
    oss << "============================================================\n";

    // Data di stampa (non di creazione, ma va benissimo per l'esame)
    std::time_t now = std::time(nullptr);
    std::tm* ptm = std::localtime(&now);
    char dataBuf[16];
    std::strftime(dataBuf, sizeof(dataBuf), "%d/%m/%Y", ptm);

    oss << "Data: " << dataBuf << "\n";
    oss << "Preventivo " << id_ << " - Cliente: " << cliente_ << "\n";
    oss << "Numero voci: " << voci_.size() << "\n";
    oss << "Totale Mq lavorati: " << totaleMq() << " mq\n";
    oss << "Stato dell'immobile: " << descriviGrado(grado_) << "\n\n";

    // da qui in poi voglio formattazione a 2 decimali per i numeri
    oss << std::fixed << std::setprecision(2);


    if (voci_.empty()) {
        oss << "(Nessuna voce inserita)\n";
    } else {
        oss << "Dettaglio voci:\n";
        oss << "--------------------------------------------\n";

        for (const std::unique_ptr<VoceCosto>& vocePtr : voci_) {
            if (!vocePtr) continue;

            oss << "- " << vocePtr->getNome()
                << " (" << vocePtr->getQuantita()
                << " " << vocePtr->getUnitaMisura() << ")\n";

            oss << "  Prezzo unitario: " << vocePtr->getPrezzoUnitario()
                << " euro  Subtotale: " << vocePtr->subtotale()
                << " euro\n";

        }

        oss << "--------------------------------------------\n";
    }

    // Calcolo imponibile, IVA e totale "in stile preventivo"
    double imponibile = totale();
    constexpr double ALIQUOTA_IVA = 0.22;
    double iva = imponibile * ALIQUOTA_IVA;
    double totaleIvato = imponibile + iva;

    oss << "Subtotale imponibile: " << imponibile << " euro\n";
    oss << "IVA (22%): " << iva << " euro\n";
    oss << "Totale complessivo: " << totaleIvato << " euro\n";

    return oss.str();
}




