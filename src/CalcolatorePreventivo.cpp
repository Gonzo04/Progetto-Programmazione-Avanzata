#include "CalcolatorePreventivo.h"
#include "Preventivo.h"
#include "RegolaCosto.h"
#include <stdexcept>

CalcolatorePreventivo::CalcolatorePreventivo() : regola_(nullptr) {}

void CalcolatorePreventivo::setRegola(const RegolaCosto* regola) {
    regola_ = regola;
}

void CalcolatorePreventivo::aggiungiLavoro(
    Preventivo& preventivo,
    const std::string& nomeCiclo,
    double mq,
    const std::shared_ptr<ListinoPrezzi>& listino,
    GradoDifficolta grado
) {
    // Controllo
    if (!regola_) {
        throw std::runtime_error("Regola di calcolo non impostata.");
    }

    /*
        La regola crea la voce concreta:
        - RegolaTinteggiatura crea una VoceTinteggiatura
        - RegolaCartongesso crea una VoceCartongesso

        Ritorno un unique_ptr<VoceCosto>:
        - quando la aggiungo al Preventivo, l'ownership viene trasferita al vector interno
    */

    std::unique_ptr<VoceCosto> nuovaVoce = regola_->creaVoce(nomeCiclo, mq, listino, grado);

    // Trasferisco l'ownership al Preventivo
    preventivo.aggiungiVoce(std::move(nuovaVoce));
}