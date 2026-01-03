#include "ListinoPrezzi.h"
#include <stdexcept>

// Costruttore vuoto
ListinoPrezzi::ListinoPrezzi() = default;


void ListinoPrezzi::impostaPrezzoMq(const std::string &nomeCiclo, double prezzo) {
/*
    Map:Operator[]:
    -l'operatore [] inserisce la chiave se non esiste,
    -se la chiave non esiste, la crea
    -se esiste, aggiorna il valore
*/
    prezziMq_[nomeCiclo] = prezzo;
}

void ListinoPrezzi::impostaCoeff(GradoDifficolta grado, double coeff) {
    //Stessa cosa per i coefficienti: se il grado è già presente, aggiorno il valore
    coeffDifficolta_[grado] = coeff;
}

double ListinoPrezzi::getPrezzoMq(const std::string &nomeCiclo) const {
    /*
    In questo caso non utilizzo operator[] perchè se nomeCiclo non esiste, operator inserirebbe un prezzo 0.0
    Uso find() come prima e se manca lancio eccezione
    */
    std::map<std::string, double>::const_iterator it = prezziMq_.find(nomeCiclo);
    if (it == prezziMq_.end()) {
        throw std::runtime_error("Ciclo non presente nel listino: " + nomeCiclo); // nel caso non ci fosse
    }
    return it->second;
}

double ListinoPrezzi::getCoeff(GradoDifficolta grado) const {
    // stessa logica
    std::map<GradoDifficolta, double>::const_iterator it = coeffDifficolta_.find(grado);
    if (it == coeffDifficolta_.end()) {
        throw std::runtime_error("Grado di difficolta' non presente nel listino");
    }
    return it->second;
}

