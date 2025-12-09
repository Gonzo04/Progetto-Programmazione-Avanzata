#include "VoceTinteggiatura.h"


//Costruttore: inizializzo qui tutti i membri protetti ereditati da VoceCosto
VoceTinteggiatura::VoceTinteggiatura(const std::string& nomeCiclo,
                                     double mq,
                                     const ListinoPrezzi& listino,
                                     GradoDifficolta grado)
//inizializzo subito la parte "VoceCosto": nome, unità di misura, mq e prezzo/mq
    : VoceCosto(nomeCiclo, "m^2", mq, listino.getPrezzoMq(nomeCiclo))
{
    coefficiente_   = listino.getCoeff(grado);          // coeff da GradoDifficolta
}

// Formula del subtotale: quantità * prezzo * coefficiente.
double VoceTinteggiatura::subtotale() const {
    return quantita_ * prezzoUnitario_ * coefficiente_;
}

// clone() restituisce una copia profonda della voce,incapsulata in unique_ptr.
std::unique_ptr<VoceCosto> VoceTinteggiatura::clone() const {

    return std::unique_ptr<VoceCosto>(new VoceTinteggiatura(*this));
}
