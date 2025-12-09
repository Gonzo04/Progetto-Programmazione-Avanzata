#ifndef LISTINOPREZZI_H
#define LISTINOPREZZI_H

#include <map>
#include <string>
#include "GradoDifficolta.h"

class ListinoPrezzi {
   private:
    std::map<std::string, double>prezziMq_; //Uso una std::map<std::string,double> per associare il nome del ciclo al suo prezzo unitario al metro quadro
    std::map<GradoDifficolta, double>coeffDifficolta_; //Uso una std::map<GradoDifficolta,double> per parametrizzare il sovrapprezzo dovuto alla difficoltà del cantiere

    public:
    ListinoPrezzi();
    void impostaPrezzoMq(const std::string& nomeCiclo,double prezzo); //per impostare o aggiornare il costo al metroquadro di un ciclo
    void impostaCoeff(GradoDifficolta grado, double coeff); //imposto o aggiorno il coefficiente per un certo grado di difficoltà
    double getPrezzoMq(const std::string& nomeCiclo) const; //Restituisce il prezzo €/mq del ciclo richiesto
    double getCoeff(GradoDifficolta grado) const; //Restituisce il coefficiente associato al grado
};

#endif
