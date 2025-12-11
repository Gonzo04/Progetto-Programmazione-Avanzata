#ifndef REGOLACARTONGESSO_H
#define REGOLACARTONGESSO_H

#include "RegolaCosto.h"
#include "VoceCartongesso.h"

class RegolaCartongesso : public RegolaCosto {
    public:
    virtual std::unique_ptr<VoceCosto>
    creaVoce(const std::string &nomeCiclo,
        double mq,
        const ListinoPrezzi &listino,
        GradoDifficolta grado) const override;
};


#endif