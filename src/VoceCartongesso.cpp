#include "VoceCartongesso.h"
#include <utility> // std::move

VoceCartongesso::VoceCartongesso(std::string nomeCiclo,
                                 double mq,
                                 const ListinoPrezzi& listino,
                                 GradoDifficolta grado)
    : VoceCosto(
          std::move(nomeCiclo),                 // move: evito copia della stringa
          "m^2",
          mq,
          listino.getPrezzoMq(getNome())        // uso il nome salvato nella base
      )
{
    coefficiente_ = listino.getCoeff(grado);
}

double VoceCartongesso::subtotale() const {
    return quantita_ * prezzoUnitario_ * coefficiente_;
}

std::unique_ptr<VoceCosto> VoceCartongesso::clone() const {
    return std::unique_ptr<VoceCosto>(new VoceCartongesso(*this));
}
