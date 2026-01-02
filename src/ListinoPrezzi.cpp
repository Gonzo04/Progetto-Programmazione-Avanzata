#include "ListinoPrezzi.h"
#include <stdexcept>

ListinoPrezzi::ListinoPrezzi() = default;
// Costruttore vuoto

void ListinoPrezzi::impostaPrezzoMq(const std::string &nomeCiclo, double prezzo) {
/*
L'operatore [] inserisce la chiave se non esiste,
oppure aggiorna il valore se esiste già
*/
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

/*
ListinoPrezzi centralizza i dati economici del preventivo:
prezzo €/mq dei cicli e coefficienti legati alla difficoltà del cantiere.
Usa std::map per associare chiavi a valori (nome ciclo → prezzo, grado → coefficiente),
garantendo lookup efficiente e chiavi univoche.
I metodi imposta* aggiornano/inseriscono valori tramite operator[]
I metodi get* effettuano lookup con find
e, se la chiave non è presente, segnalano l’errore tramite eccezioni,
evitando la creazione involontaria di valori di default.
*/