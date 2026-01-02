#include "VoceTinteggiatura.h"
#include <utility> // std::move

VoceTinteggiatura::VoceTinteggiatura(std::string nomeCiclo,
                                     double mq,
                                     const ListinoPrezzi& listino,
                                     GradoDifficolta grado)
    : VoceCosto(
          std::move(nomeCiclo), // move: evito una copia della stringa nel costruttore base
          "m^2",
          mq,
          // prezzo calcolato PRIMA del move usando la stringa originale
          listino.getPrezzoMq(getNome())
      )
{
    // coefficiente legato alla difficoltà del cantiere
    coefficiente_ = listino.getCoeff(grado);
}

double VoceTinteggiatura::subtotale() const {
    return quantita_ * prezzoUnitario_ * coefficiente_;
}

std::unique_ptr<VoceCosto> VoceTinteggiatura::clone() const {
    return std::unique_ptr<VoceCosto>(new VoceTinteggiatura(*this));
}





