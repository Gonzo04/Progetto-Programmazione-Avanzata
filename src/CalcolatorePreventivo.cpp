#include "CalcolatorePreventivo.h"

void CalcolatorePreventivo::aggiungiLavoro(Preventivo &preventivo,
                                            const std::string &nomeCiclo,
                                            double mq,
                                            const ListinoPrezzi &listino,
                                            GradoDifficolta grado) {
    if (!regola_) {
        throw std::runtime_error("RegolaCosto non impostata nel CalcolatorePreventivo");
    }
    std::unique_ptr<VoceCosto> voce =
        regola_ ->creaVoce(nomeCiclo, mq, listino, grado);
    preventivo.aggiungiVoce(std::move(voce));
}
