#include "ListinoPrezzi.h"
#include <stdexcept>

ListinoPrezzi::ListinoPrezzi() = default;
// Costruttore vuoto

void ListinoPrezzi::impostaPrezzoMq(const std::string &nomeCiclo, double prezzo) {
    /* [STL MAP] L'operatore [] inserisce la chiave se non esiste,
     oppure aggiorna il valore se esiste già */
    prezziMq_[nomeCiclo] = prezzo;
}

void ListinoPrezzi::impostaCoeff(GradoDifficolta grado, double coeff) {
    //Stessa cosa per i coefficienti: se il grado è già presente, aggiorno il valore
    coeffDifficolta_[grado] = coeff;
}

double ListinoPrezzi::getPrezzoMq(const std::string &nomeCiclo) const {

    std::map<std::string, double>::const_iterator it = prezziMq_.find(nomeCiclo);
    if (it == prezziMq_.end()) {
        throw std::runtime_error("Ciclo non presente nel listino: " + nomeCiclo); // nel caso non ci fosse
    }
    return it->second;
}

double ListinoPrezzi::getCoeff(GradoDifficolta grado) const {
    std::map<GradoDifficolta, double>::const_iterator it = coeffDifficolta_.find(grado);
    if (it == coeffDifficolta_.end()) {
        throw std::runtime_error("Grado di difficolta' non presente nel listino");
    }
    return it->second;
}
