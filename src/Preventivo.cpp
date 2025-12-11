#include "Preventivo.h"

#include <numeric>     // std::accumulate
#include <sstream>     // std::ostringstream "per costruire il riepilogo di una stringa"
#include <utility>     // std::move

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
    for (std::size_t i = 0; i < other.voci_.size(); ++i) {
        const std::unique_ptr<VoceCosto>& vocePtr = other.voci_[i];
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
    for (std::size_t i = 0; i < other.voci_.size(); ++i) {
        const std::unique_ptr<VoceCosto>& vocePtr = other.voci_[i];
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

//TOTALE: calcolo del totale usando std::accumulate e una lambda ( la quale somma i subtototali di tutte le voci non nulle)
double Preventivo::totale() const {
    return std::accumulate(
        voci_.begin(),
        voci_.end(),
        0.0, // valore iniziale dell'accumulatore
        // lambda che prende l'accumulatore e il unique_ptr alla voce
        [](double acc, const std::unique_ptr<VoceCosto>& vocePtr)->double {
            return vocePtr ? acc + vocePtr->subtotale() : acc;
    }
    );
}

//Riepilogo del preventivo in formato stringa
std::string Preventivo::riepilogo() const {
    std::ostringstream oss;

    // Intestazione
    oss << "Preventivo " << id_ << " - Cliente: " << cliente_ << "\n";
    oss << "Numero voci: " << voci_.size() << "\n\n";

    if (voci_.empty()) {
        oss << "(Nessuna voce inserita)\n";
    } else {
        oss << "Dettaglio voci:\n";
        oss << "--------------------------------------------\n";

        // Per ogni voce stampo i dati principali
        for (const auto& vocePtr : voci_) {
            if (!vocePtr) {
                continue; // sicurezza: salto eventuali puntatori null
            }

            oss << "- " << vocePtr->getNome()
                << " (" << vocePtr->getQuantita() << " "
                << vocePtr->getUnitaMisura() << ")\n";

            oss << "  Prezzo unitario: " << vocePtr->getPrezzoUnitario()
                << "  Coefficiente: "   << vocePtr->getCoefficiente()
                << "  Subtotale: "      << vocePtr->subtotale()
                << " euro\n";
        }

        oss << "--------------------------------------------\n";
    }

    // Totale complessivo
    oss << "Totale: " << totale() << " euro\n";

    return oss.str();
}


//Operator +: restituisce un nuovo preventivo che contiene le voci di "a" + una copia (clone) delle voci di "b".
Preventivo operator+(const Preventivo& a, const Preventivo& b) {
    Preventivo risultato = a; // usa il copy constructor per partire da "a"

    // Per ogni voce di "b" faccio un clone e lo aggiungo al risultato.
    const std::vector<std::unique_ptr<VoceCosto> >& vociB = b.getVoci();
    for (std::size_t i = 0; i < vociB.size(); ++i) {
        const std::unique_ptr<VoceCosto>& vocePtr = vociB[i];
        if (vocePtr) {
            risultato.aggiungiVoce(vocePtr->clone());
        }
    }

    return risultato;
}
