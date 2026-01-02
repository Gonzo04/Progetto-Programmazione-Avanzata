#include "VoceTinteggiatura.h"


// Costruttore: inizializzo qui tutti i membri protetti ereditati da VoceCosto e poi la parte di VoceCosto:
// ovvero nome, unità di misura, mq e prezzo €/mq

VoceTinteggiatura::VoceTinteggiatura(std::string nomeCiclo,double mq,
    const ListinoPrezzi& listino,GradoDifficolta grado)

: VoceCosto(nomeCiclo, "m^2", mq, listino.getPrezzoMq(nomeCiclo))
{
    coefficiente_ = listino.getCoeff(grado); // coefficiente in base al grado
}

// Formula del subtotale: quantità * prezzo * coefficiente.
double VoceTinteggiatura::subtotale() const {
        return quantita_ * prezzoUnitario_ * coefficiente_;
}

// clone() restituisce una copia profonda della voce, incapsulata in unique_ptr.
std::unique_ptr<VoceCosto> VoceTinteggiatura::clone() const {

return std::unique_ptr<VoceCosto>(new VoceTinteggiatura(*this));
}

//VoceTinteggiatura.cpp implementa la logica della voce di costo per la tinteggiatura.
//Nel costruttore inizializza i campi comuni ereditati da VoceCosto
//(nome ciclo, unità di misura, quantità e prezzo unitario) usando il ListinoPrezzi
//Successivamente imposta il coefficiente legato al grado di difficoltà.
//Il metodo subtotale() calcola il costo della voce con la formula quantità × prezzo unitario × coefficiente.
//Il metodo clone() restituisce una copia polimorfa della voce
//Necessaria per copiare correttamente un Preventivo che contiene voci gestite tramite std::unique_ptr<VoceCosto>