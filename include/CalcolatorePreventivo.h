#ifndef __CALCOLATOREPREVENTIVO_H__
#define __CALCOLATOREPREVENTIVO_H__

#include <string>
#include <memory>
#include "GradoDifficolta.h"
/*
    Il CalcolatorePreventivo costruisce le voci e le inserisce nel Preventivo.

    - NON contiene i prezzi (sono nel ListinoPrezzi)
    - coordina le varie fasi: prende l'input (nome ciclo, mq, grado)
    -> chiede alla regola di creare la voce -> la aggiunge al preventivo

    Pattern Strategy: la regola di calcolo è intercambiabile (RegolaCosto + derivate)
*/

class Preventivo;
class ListinoPrezzi;
class RegolaCosto;

class CalcolatorePreventivo {

private:
    /*
        Puntatore alla strategia corrente:
        - è un raw pointer const perché il Calcolatore NON possiede la regola
        - const: la regola non viene modificata, viene solo usata per costruire voci.
    */
    const RegolaCosto* regola_;

public:
    CalcolatorePreventivo();

    // Seleziona quale strategia usare (tinteggiatura o cartongesso)
    void setRegola(const RegolaCosto* regola);

    /*
        Crea una voce di preventivo usando la regola corrente e la aggiunge al Preventivo.

        - shared_ptr perché lo stesso listino può essere condiviso tra più componenti
          (voci, calcolatore, ecc.), senza gestione manuale della memoria.
    */
    void aggiungiLavoro(
        Preventivo& preventivo,
        const std::string& nomeCiclo,
        double mq,
        const std::shared_ptr<ListinoPrezzi>& listino,
        GradoDifficolta grado
    );
};


#endif
