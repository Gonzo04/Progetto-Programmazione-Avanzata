#include "CalcolatorePreventivo.h"
#include "Preventivo.h"
#include "RegolaCosto.h"
#include <stdexcept>

CalcolatorePreventivo::CalcolatorePreventivo() : regola_(nullptr) {}

void CalcolatorePreventivo::setRegola(const RegolaCosto* regola) {
    regola_ = regola;
}

// [MODIFICA] Aggiornata firma
void CalcolatorePreventivo::aggiungiLavoro(
    Preventivo& preventivo,
    const std::string& nomeCiclo,
    double mq,
    const std::shared_ptr<ListinoPrezzi>& listino,
    GradoDifficolta grado
) {
    if (!regola_) {
        throw std::runtime_error("Regola di calcolo non impostata.");
    }

    // Passo lo shared_ptr direttamente alla regola
    std::unique_ptr<VoceCosto> nuovaVoce = regola_->creaVoce(nomeCiclo, mq, listino, grado);
    preventivo.aggiungiVoce(std::move(nuovaVoce));
}