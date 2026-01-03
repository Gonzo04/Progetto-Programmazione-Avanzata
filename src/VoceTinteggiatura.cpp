#include "VoceTinteggiatura.h"

#include <utility>

VoceTinteggiatura::VoceTinteggiatura(std::string nomeCiclo,
                                     double mq,
                                     const ListinoPrezzi& listino,
                                     GradoDifficolta grado)
    : VoceCosto(
    // Uso nomeCiclo (parametro) per prendere il prezzo dal listino,
    // dato che la parte base è ancora in costruzione non posso usare getNome()

          nomeCiclo,
          "m^2",
          mq,
          listino.getPrezzoMq(nomeCiclo)
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

// Copia polimorfa: creo un nuovo oggetto dello stesso tipo concreto.
std::unique_ptr<VoceCosto> VoceTinteggiatura::clone() const {
    return std::unique_ptr<VoceCosto>(new VoceTinteggiatura(*this));
}
