#ifndef REGOLATINTEGGIATURA_H
#define REGOLATINTEGGIATURA_H

#include "RegolaCosto.h"

class RegolaTinteggiatura : public RegolaCosto {
public:
    // [MODIFICA] Aggiornata la firma dell'override
    std::unique_ptr<VoceCosto> creaVoce(
        const std::string& nomeCiclo,
        double mq,
        const std::shared_ptr<ListinoPrezzi>& listino,
        GradoDifficolta grado
    ) const override;
};

#endif