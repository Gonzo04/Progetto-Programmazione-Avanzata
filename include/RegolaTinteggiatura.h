#ifndef REGOLATINTEGGIATURA_H
#define REGOLATINTEGGIATURA_H

#include "RegolaCosto.h"

/*
    RegolaTinteggiatura è la strategia concreta per creare voci di tinteggiatura.

    Implementa l'interfaccia RegolaCosto:
    - creaVoce(...) costruisce una VoceTinteggiatura

    Output:
    ritorna un unique_ptr<VoceCosto> così il chiamante (Calcolatore/Preventivo)
    gestisce la memoria in modo automatico.
*/

class RegolaTinteggiatura : public RegolaCosto {
public:

    std::unique_ptr<VoceCosto> creaVoce(
        const std::string& nomeCiclo,
        double mq,
        const std::shared_ptr<ListinoPrezzi>& listino,
        GradoDifficolta grado
    ) const override;
};

#endif