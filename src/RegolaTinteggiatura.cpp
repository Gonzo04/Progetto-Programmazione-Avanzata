#include "RegolaTinteggiatura.h"
#include "VoceTinteggiatura.h"

#include <stdexcept>

std::unique_ptr<VoceCosto> RegolaTinteggiatura::creaVoce(
    const std::string& nomeCiclo,
    double mq,
    const std::shared_ptr<ListinoPrezzi>& listino,
    GradoDifficolta grado
) const {
    /*
    Uso il listino in sola lettura per recuperare prezzo e coefficiente.
    */
    if (!listino) {
        throw std::runtime_error("Listino non valido (nullptr) nella regola tinteggiatura.");
    }

    return std::unique_ptr<VoceCosto>(
        new VoceTinteggiatura(nomeCiclo, mq, *listino, grado)
    );
}
