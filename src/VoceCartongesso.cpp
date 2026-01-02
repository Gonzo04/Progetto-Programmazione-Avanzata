#include "VoceCartongesso.h"

#include <utility> // std::move

VoceCartongesso::VoceCartongesso(std::string nomeCiclo,
                                 double mq,
                                 const ListinoPrezzi& listino,
                                 GradoDifficolta grado)
    : VoceCosto(
          nomeCiclo,
          "m^2",
          mq,
          listino.getPrezzoMq(nomeCiclo)
      )
{
    coefficiente_ = listino.getCoeff(grado);
    nome_ = std::move(nomeCiclo);
}

double VoceCartongesso::subtotale() const {
    return quantita_ * prezzoUnitario_ * coefficiente_;
}

std::unique_ptr<VoceCosto> VoceCartongesso::clone() const {
    return std::unique_ptr<VoceCosto>(new VoceCartongesso(*this));
}
