#ifndef VOCETINTEGGIATURA_H
#define VOCETINTEGGIATURA_H

#include <string>
#include "VoceCosto.h"
#include "ListinoPrezzi.h"
#include "GradoDifficolta.h"

// Voce concreta per lavori di tinteggiatura.
// Prende il prezzo €/mq dal listino e applica un coefficiente in base alla difficoltà.
class VoceTinteggiatura : public VoceCosto {
public:
    // nomeCiclo: chiave del ciclo nel listino
    // mq: metri quadri
    // listino: fornisce prezzo e coefficiente
    // grado: determina il coefficiente di difficoltà
    VoceTinteggiatura(std::string nomeCiclo,
                      double mq,
                      const ListinoPrezzi& listino,
                      GradoDifficolta grado);
    //Calcolo del totale
    double subtotale() const override;

    // Serve per stampe/CSV senza usare dynamic_cast
    const char* tipoVoce() const override { return "Tinteggiatura"; }

    std::unique_ptr<VoceCosto> clone() const override;
};

#endif

/*
 Questa classe rappresenta una voce di preventivo per tinteggiatura.
 Inizializza prezzo e coefficiente dal listino e calcola il subtotale.
 */
