#include "VoceTinteggiatura.h"

#include <utility> // std::move

VoceTinteggiatura::VoceTinteggiatura(std::string nomeCiclo,
                                     double mq,
                                     const ListinoPrezzi& listino,
                                     GradoDifficolta grado)
    : VoceCosto(
          nomeCiclo,                         // per calcolare prezzo in modo sicuro
          "m^2",
          mq,
          listino.getPrezzoMq(nomeCiclo)     // usa il parametro, NON getNome()
      )
{
    // ora che la base è costruita posso impostare il coefficiente
    coefficiente_ = listino.getCoeff(grado);

    // e adesso posso anche “spostare” il nome nella base senza problemi
    nome_ = std::move(nomeCiclo);
}

double VoceTinteggiatura::subtotale() const {
    return quantita_ * prezzoUnitario_ * coefficiente_;
}

std::unique_ptr<VoceCosto> VoceTinteggiatura::clone() const {
    return std::unique_ptr<VoceCosto>(new VoceTinteggiatura(*this));
}
