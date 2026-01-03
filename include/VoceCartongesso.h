#ifndef VOCECARTONGESSO_H
#define VOCECARTONGESSO_H

#include <memory>
#include <string>

#include "VoceCosto.h"
#include "ListinoPrezzi.h"
#include "GradoDifficolta.h"
/*
    VoceCartongesso = implementazione di VoceCosto per lavorazioni di cartongesso.

    - inizializza i dati economici a partire dal listino (prezzo €/mq + coefficiente difficoltà)
    - fornire la formula del costo tramite subtotale()
    - supportare la copia polimorfa tramite clone()
    - il listino viene passato per const reference
*/

class VoceCartongesso : public VoceCosto {
public:
    // nomeCiclo: nome del ciclo/lavorazione presente nel listino
    // mq: quantità di metri quadri
    // listino: sorgente dei prezzi e dei coefficienti
    // grado: usato per applicare il coefficiente di difficoltà
    VoceCartongesso(std::string nomeCiclo,
                    double mq,
                    const ListinoPrezzi& listino,
                    GradoDifficolta grado);

    // Subtotale = quantità * prezzoUnitario * coefficiente
    double subtotale() const override;

    // Etichetta logica usata per stampe/CSV (niente dynamic_cast)
    const char* tipoVoce() const override { return "Cartongesso"; }
    // Copia polimorfa: quando copio un Preventivo devo clonare le voci (unique_ptr non è copiabile)
    std::unique_ptr<VoceCosto> clone() const override;
};

#endif // VOCECARTONGESSO_H
