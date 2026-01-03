#ifndef REGOLACARTONGESSO_H
#define REGOLACARTONGESSO_H

#include "RegolaCosto.h"
/*
    RegolaCartongesso è la strategia concreta per creare voci di cartongesso.

    Differenza rispetto a RegolaTinteggiatura:
    - qui la creazione viene delegata a un Builder (VoceCartongessoBuilder)
      per gestire costruzione a step e possibili validazioni.

    Output:
    - ritorna sempre std::unique_ptr<VoceCosto> per mantenere uniforme l'interfaccia RegolaCosto.
*/
class RegolaCartongesso : public RegolaCosto {
public:
    // [MODIFICA] Aggiornata la firma del override
    std::unique_ptr<VoceCosto> creaVoce(
        const std::string& nomeCiclo,
        double mq,
        const std::shared_ptr<ListinoPrezzi>& listino,
        GradoDifficolta grado
    ) const override;
};

#endif