#ifndef CALCOLATOREPREVENTIVO_H
#define CALCOLATOREPREVENTIVO_H

#include <memory>
#include <string>
#include <stdexcept>

#include "RegolaCosto.h"
#include "Preventivo.h"

class CalcolatorePreventivo {
public:
    CalcolatorePreventivo() : regola_(nullptr) {}

    // Imposta la strategia da usare
    void setRegola(const RegolaCosto* r) {
        regola_ = r;
    }

    // Crea e aggiunge una voce al preventivo usando la strategia corrente
    void aggiungiLavoro(Preventivo& preventivo,
                        const std::string& nomeCiclo,
                        double mq,
                        const ListinoPrezzi& listino,
                        GradoDifficolta grado);

private:
    const RegolaCosto* regola_;  // puntatore non-owning alla strategia
};

#endif // CALCOLATOREPREVENTIVO_H