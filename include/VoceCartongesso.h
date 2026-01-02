#ifndef VOCECARTONGESSO_H
#define VOCECARTONGESSO_H

#include <memory>
#include <string>

#include "VoceCosto.h"
#include "ListinoPrezzi.h"
#include "GradoDifficolta.h"

// Voce concreta del preventivo per lavori di cartongesso.
// Inizializza prezzo €/mq dal ListinoPrezzi e coefficiente in base al GradoDifficolta.
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

    // Etichetta logica usata per stampe/CSV senza RTTI (niente dynamic_cast)
    const char* tipoVoce() const override { return "Cartongesso"; }

    // Copia polimorfa (deep copy) necessaria quando si copia un Preventivo con unique_ptr<VoceCosto>
    std::unique_ptr<VoceCosto> clone() const override;
};

#endif // VOCECARTONGESSO_H
