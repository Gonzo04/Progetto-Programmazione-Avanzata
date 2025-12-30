#ifndef LISTINOPREZZI_H
#define LISTINOPREZZI_H

#include <map>
#include <string>
#include "GradoDifficolta.h"

class ListinoPrezzi {
   private:
    std::map<std::string, double>prezziMq_;
    //Uso una std::map<std::string,double>
    //per associare il nome del ciclo al suo prezzo unitario al metro quadro

    std::map<GradoDifficolta, double> coeffDifficolta_;
    //Uso una std::map<GradoDifficolta,double>
    //per parametrizzare il sovrapprezzo dovuto alla difficoltà del cantiere

    public:
    ListinoPrezzi();
    void impostaPrezzoMq(const std::string& nomeCiclo,double prezzo); //per impostare o aggiornare il costo al metroquadro di un ciclo
    void impostaCoeff(GradoDifficolta grado, double coeff); //imposto o aggiorno il coefficiente per un certo grado di difficoltà
    double getPrezzoMq(const std::string& nomeCiclo) const; //Restituisce il prezzo €/mq del ciclo richiesto
    double getCoeff(GradoDifficolta grado) const; //Restituisce il coefficiente associato al grado
};

#endif


//ListinoPrezzi è una classe di servizio
//centralizza i prezzi unitari e i coefficienti di difficoltà utilizzati dalle voci di preventivo
//Utilizza contenitori STL (std::map) per associare il nome di un ciclo al prezzo €/mq
//e il grado di difficoltà al relativo coefficiente
//Le voci concrete (es. tinteggiatura) accedono al listino tramite un’associazione
//evitando duplicazioni di dati e mantenendo separata la logica di calcolo dai dati economici
//I metodi di accesso sono const e incapsulano l’uso delle mappe
//in caso di richieste non valide, il listino può segnalare l’errore tramite eccezioni.