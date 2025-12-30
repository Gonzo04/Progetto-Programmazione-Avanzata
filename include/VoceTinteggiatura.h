#ifndef VOCETINTEGGIATURA_H
#define VOCETINTEGGIATURA_H

#include <string>
#include "VoceCosto.h"
#include "ListinoPrezzi.h"
#include "GradoDifficolta.h"

// VoceCosto per tinteggiatura:
// prende dal listino il prezzo €/mq e il coefficiente in base al grado di difficoltà
class VoceTinteggiatura : public VoceCosto {

// nomeCiclo: chiave del ciclo nel listino, mq: quantità, grado: coefficiente difficoltà

public:
    VoceTinteggiatura(const std::string& nomeCiclo,
                      double mq,
                      const ListinoPrezzi& listino,
                      GradoDifficolta grado);

    double subtotale() const override; // Formula del subtotale = quantità * prezzoUnitario * coefficiente
    std::unique_ptr<VoceCosto> clone() const override; // Implementazione del clone polimorfo (deep copy)
};

#endif

// VoceTinteggiatura.h definisce una voce concreta del preventivo per lavori di tinteggiatura.
// Deriva da VoceCosto per riusare i campi comuni (nome, unità, quantità, prezzo, coefficiente).
// Il costruttore inizializza la voce usando ListinoPrezzi
// subtotale() calcola il costo della voce.
// clone() permette la copia profonda polimorfa
// necessaria per copiare correttamente un Preventivo che contiene unique_ptr<VoceCosto>.