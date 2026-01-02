#ifndef VOCECARTONGESSO_H
#define VOCECARTONGESSO_H

#include <string>
#include <memory>

#include "VoceCosto.h"
#include "ListinoPrezzi.h"
#include "GradoDifficolta.h"

// Voce concreta per lavori di cartongesso.
// Prezzo €/mq dal listino + coefficiente in base alla difficoltà.
class VoceCartongesso : public VoceCosto {
public:
    VoceCartongesso(std::string nomeCiclo,
                    double mq,
                    const ListinoPrezzi& listino,
                    GradoDifficolta grado);

    double subtotale() const override;

    const char* tipoVoce() const override { return "Cartongesso"; }

    std::unique_ptr<VoceCosto> clone() const override;
};

#endif
