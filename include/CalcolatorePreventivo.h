#ifndef __CALCOLATOREPREVENTIVO_H__
#define __CALCOLATOREPREVENTIVO_H__

#include <string>
#include <memory>
#include "GradoDifficolta.h"

class Preventivo;
class ListinoPrezzi; // Forward declaration
class RegolaCosto;   // Forward declaration

class CalcolatorePreventivo {
public:
    CalcolatorePreventivo();

    void setRegola(const RegolaCosto* regola);

    // [MODIFICA] Cambiato const ListinoPrezzi& in const std::shared_ptr<ListinoPrezzi>&
    void aggiungiLavoro(
        Preventivo& preventivo,
        const std::string& nomeCiclo,
        double mq,
        const std::shared_ptr<ListinoPrezzi>& listino,
        GradoDifficolta grado
    );

private:
    const RegolaCosto* regola_;
};

#endif
