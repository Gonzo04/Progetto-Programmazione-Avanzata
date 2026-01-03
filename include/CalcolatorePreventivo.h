#ifndef __CALCOLATOREPREVENTIVO_H__
#define __CALCOLATOREPREVENTIVO_H__

#include <string>
#include <memory>
#include "GradoDifficolta.h"

class Preventivo;
class ListinoPrezzi; // Forward declaration
class RegolaCosto;   // Forward declaration

class CalcolatorePreventivo {

private:
    const RegolaCosto* regola_;

public:
    CalcolatorePreventivo();

    void setRegola(const RegolaCosto* regola);


    void aggiungiLavoro(
        Preventivo& preventivo,
        const std::string& nomeCiclo,
        double mq,
        const std::shared_ptr<ListinoPrezzi>& listino,
        GradoDifficolta grado
    );
};


#endif
