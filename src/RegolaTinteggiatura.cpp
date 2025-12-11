#include "RegolaTinteggiatura.h"

std::unique_ptr<VoceCosto> RegolaTinteggiatura::creaVoce(const std::string &nomeCiclo,
                                                        double mq,
                                                        const ListinoPrezzi &listino,
                                                        GradoDifficolta grado) const {
    return std::unique_ptr<VoceCosto>(
        new VoceTinteggiatura(nomeCiclo, mq, listino, grado)
    );
}
