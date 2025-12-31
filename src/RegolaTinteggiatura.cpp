#include "RegolaTinteggiatura.h"
#include "VoceTinteggiatura.h"

// [MODIFICA] Aggiornata la firma dell'implementazione
std::unique_ptr<VoceCosto> RegolaTinteggiatura::creaVoce(
    const std::string& nomeCiclo,
    double mq,
    const std::shared_ptr<ListinoPrezzi>& listino,
    GradoDifficolta grado
) const {

    // VoceTinteggiatura vuole ancora il riferimento classico (ListinoPrezzi&).
    // Quindi dereferenziamo lo shared_ptr con l'asterisco (*listino).
    return std::unique_ptr<VoceCosto>(
        new VoceTinteggiatura(nomeCiclo, mq, *listino, grado)
    );
}