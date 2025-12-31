#ifndef REGOLACARTONGESSO_H
#define REGOLACARTONGESSO_H

#include "RegolaCosto.h"

class RegolaCartongesso : public RegolaCosto {
public:
    // [MODIFICA] Aggiornata la firma del override
    std::unique_ptr<VoceCosto> creaVoce(
        const std::string& nomeCiclo,
        double mq,
        const std::shared_ptr<ListinoPrezzi>& listino,
        GradoDifficolta grado
    ) const override;
};

#endif