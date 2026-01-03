#ifndef REGOLACOSTO_H
#define REGOLACOSTO_H

#include <string>
#include <iostream>
#include <memory>

#include "VoceCosto.h"
#include "GradoDifficolta.h"
#include "ListinoPrezzi.h"

/*
    RegolaCosto è una classe astratta per la creazione delle voci.

    - CalcolatorePreventivo non sa quali classi concrete esistono (VoceTinteggiatura, VoceCartongesso, ...)
    - Delego a una regola la responsabilità di costruire la VoceCosto corretta

    Ogni classe derivata implementa creaVoce():
    - RegolaTinteggiatura -> crea una VoceTinteggiatura
    - RegolaCartongesso -> crea una VoceCartongesso

    Return:
    - std::unique_ptr<VoceCosto> per ownership chiara (la voce avrà un solo proprietario: il Preventivo)
*/
class RegolaCosto {
    public:
        virtual ~RegolaCosto() = default;
        /*
            - prende i dati necessari (nome ciclo, mq, listino, grado)
            - costruisce e ritorna una VoceCosto concreta
        */
        virtual std::unique_ptr<VoceCosto>creaVoce(
            const std::string &nomeCiclo,
            double mq,
            const std::shared_ptr<ListinoPrezzi>& listino,
            GradoDifficolta grado) const=0;
};

#endif