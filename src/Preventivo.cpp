#include "Preventivo.h"
#include "VoceCosto.h"
#include <sstream>     // std::ostringstream "per costruire il riepilogo di una stringa"
#include <utility>     // std::move
#include <ctime>
#include <algorithm>
#include <iomanip>    // std::fixed, std::setprecision
#include <numeric>
#include <set>


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
    : id_(std::move(id)),
      cliente_(std::move(cliente)),
      grado_(grado)
{}

//COPY CONSTRUCTOR: copia profonda polimorfa delle voci
Preventivo::Preventivo(const Preventivo& other)
    : id_(other.id_),
      cliente_(other.cliente_),
      grado_(other.grado_),
      chiaviVoci_(other.chiaviVoci_)
{
    //Pre-alloco lo spazio per le voci in modo da evitare riallocazioni multiple
    voci_.reserve(other.voci_.size());
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
    chiaviVoci_ = other.chiaviVoci_;
    //Svuoto le voci attuali
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
      voci_(std::move(other.voci_)),
      chiaviVoci_(std::move(other.chiaviVoci_))
{}

//Move assignment operator
Preventivo& Preventivo::operator=(Preventivo&& other) noexcept {
    if (this == &other) return *this;

    id_      = std::move(other.id_);
    cliente_ = std::move(other.cliente_);
    grado_   = other.grado_;
    voci_    = std::move(other.voci_);
    chiaviVoci_ = std::move(other.chiaviVoci_);


    return *this;
}

void Preventivo::aggiungiVoce(std::unique_ptr<VoceCosto> voce) {

    if (!voce) return;

    // Creo una chiave logica: stesso tipo + stesso nome => stessa lavorazione
    // evitando di unire voci diverse che magari hanno lo stesso nome.
    const std::string key = std::string(voce->tipoVoce()) + "|" + voce->getNome();

    // Il set controlla che sia la prima volta a vedere questa voce
    // insert() ritorna pair<iterator, bool>, dove bool è true solo se inserisce davvero.
    auto inserimento = chiaviVoci_.insert(key);

    // Caso 1: nuova lavorazione -> la salvo nel vector (ownership trasferita)
    if (inserimento.second) {
        voci_.push_back(std::move(voce));
        return;
    }

    // Caso 2: lavorazione già presente -> cerco nel vector la voce reale e faccio merge quantità
    for (auto& voceEsistente : voci_) {
        if (!voceEsistente) continue;

        const bool stessoTipo = (std::string(voceEsistente->tipoVoce()) == voce->tipoVoce());
        const bool stessoNome = (voceEsistente->getNome() == voce->getNome());

        if (stessoTipo && stessoNome) {
            voceEsistente->setQuantita(voceEsistente->getQuantita() + voce->getQuantita());
            return;
        }
    }


    // Fallback: se per qualche motivo non trovo la voce, la aggiungo comunque.
    voci_.push_back(std::move(voce));
}


Preventivo& Preventivo::operator+=(std::unique_ptr<VoceCosto> voce) {
    aggiungiVoce(std::move(voce));
    return *this;
}

double Preventivo::totale() const {
    // Sommo i subtotali delle voci Accumulate + Lambda
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
    // Template 'aggrega'
    // Qui aggrego le quantità (mq)
    return aggrega(
        [](double acc, const VoceCosto& voce) {
            return acc + voce.getQuantita();
        },
        0.0 // Valore iniziale
    );
}


// Ordina le voci per nome (ordine alfabetico crescente).
void Preventivo::ordinaPerNome() {
    std::sort(
        voci_.begin(),
        voci_.end(),
        [](const std::unique_ptr<VoceCosto>& a,
           const std::unique_ptr<VoceCosto>& b) {
            // se per qualche motivo uno fosse nullo, lo metto in fondo
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

    // Intestazione
    oss << "EDILCOLOR - Preventivo lavori di tinteggiatura/cartongesso\n";
    oss << "============================================================\n";

    // Data di stampa
    std::time_t now = std::time(nullptr);
    std::tm* ptm = std::localtime(&now);
    char dataBuf[16];
    std::strftime(dataBuf, sizeof(dataBuf), "%d/%m/%Y", ptm);

    oss << "Data: " << dataBuf << "\n";
    oss << "Preventivo " << id_ << " - Cliente: " << cliente_ << "\n";
    oss << "Numero voci: " << voci_.size() << "\n";
    oss << "Totale Mq lavorati: " << totaleMq() << " mq\n";
    oss << "Stato dell'immobile: " << descriviGrado(grado_) << "\n\n";

    // Formatazzione dei numeri
    oss << std::fixed << std::setprecision(2);


    if (voci_.empty()) {
        oss << "(Nessuna voce inserita)\n";
    } else {
        oss << "Dettaglio voci:\n";
        oss << "--------------------------------------------\n";

        // Polimorfismo: vocePtr è VoceCosto* ma subtotale() è virtual
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

    // Calcolo imponibile, IVA e totale
    double imponibile = totale();
    constexpr double ALIQUOTA_IVA = 0.22;
    double iva = imponibile * ALIQUOTA_IVA;
    double totaleIvato = imponibile + iva;

    oss << "Subtotale imponibile: " << imponibile << " euro\n";
    oss << "IVA (22%): " << iva << " euro\n";
    oss << "Totale complessivo: " << totaleIvato << " euro\n";

    return oss.str();
}




