#include "VoceCartongesso.h"

#include <utility>

VoceCartongesso::VoceCartongesso(std::string nomeCiclo,
                                 double mq,
                                 const ListinoPrezzi& listino,
                                 GradoDifficolta grado)
    : VoceCosto(
    // Prendo il prezzo usando il parametro nomeCiclo.
    // Evito di usare getNome() qui, perché la base è ancora in costruzione.
          nomeCiclo,
          "m^2",
          mq,
          listino.getPrezzoMq(nomeCiclo)
      )
{
    // La parte base è pronta: ora imposto la maggiorazione in base alla difficoltà
    coefficiente_ = listino.getCoeff(grado);

    // Salvo il nome nella base (sposto la stringa per evitare una copia extra)
    nome_ = std::move(nomeCiclo);
}

double VoceCartongesso::subtotale() const {
    return quantita_ * prezzoUnitario_ * coefficiente_;
}
// Copia polimorfa: creo un nuovo oggetto dello stesso tipo concreto

std::unique_ptr<VoceCosto> VoceCartongesso::clone() const {
    return std::unique_ptr<VoceCosto>(new VoceCartongesso(*this));
}
