#ifndef VOCETINTEGGIATURA_H
#define VOCETINTEGGIATURA_H

#include <string>
#include "VoceCosto.h"
#include "ListinoPrezzi.h"
#include "GradoDifficolta.h"

//Voce concreta di costo per la tinteggiatura
//Prende dal listino il prezzo/mq e il coefficiente in base al grado di difficoltà
class VoceTinteggiatura : public VoceCosto {
public:
    VoceTinteggiatura(const std::string& nomeCiclo, //nome commerciale del ciclo
                      double mq, //metriquadri da tinteggiare
                      const ListinoPrezzi& listino, //listino da cui recupero i prezzi e coefficienti
                      GradoDifficolta grado); //grado per il coefficiente

    double subtotale() const override; //Formula del subtotale = quantità * prezzoUnitario * coefficiente
    std::unique_ptr<VoceCosto> clone() const override; //Implementazione del clone polimorfo (deep copy)
};

#endif
