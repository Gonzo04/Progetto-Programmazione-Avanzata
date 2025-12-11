#ifndef REGOLATINTEGGIATURA_H
#define REGOLATINTEGGIATURA_H

#include "RegolaCosto.h"
#include "VoceTinteggiatura.h"

class RegolaTinteggiatura : public RegolaCosto {
    public:
            virtual std::unique_ptr<VoceCosto>
            creaVoce(const std::string &nomeCiclo,
                    double mq,
                    const ListinoPrezzi &listino,
                    GradoDifficolta grado) const override;
};

#endif