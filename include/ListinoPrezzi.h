#ifndef LISTINOPREZZI_H
#define LISTINOPREZZI_H

#include <map>
#include <string>
#include "GradoDifficolta.h"

class ListinoPrezzi {
   private:
    // associo il nome del ciclo al prezzo unitario (€/mq)
    std::map<std::string, double>prezziMq_;

    //Associo il grado di difficoltà ad un coefficiente moltiplicativo
    std::map<GradoDifficolta, double> coeffDifficolta_;


    public:
    ListinoPrezzi();

    // Inserisce o aggiorna il prezzo €/mq di un ciclo (nomeCiclo è la chiave)
    void impostaPrezzoMq(const std::string& nomeCiclo,double prezzo);

    // Inserisce o aggiorna il coefficiente per una difficoltà
    void impostaCoeff(GradoDifficolta grado, double coeff);

    //Getter: uso find() e non operator[] perchè operator[] creerebbe valori di default se la chiave manca
    //e se manca una chiave, lancio eccezione: è un errore di configurazione/listino
    double getPrezzoMq(const std::string& nomeCiclo) const;

    //Restituisce il coefficiente associato al grado
    double getCoeff(GradoDifficolta grado) const;
};

#endif

