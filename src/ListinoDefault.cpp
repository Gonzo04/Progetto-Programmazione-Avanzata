#include "ListinoDefault.h"
#include "CatalogoCicli.h"
#include "GradoDifficolta.h"

void caricaListinoDefault(ListinoPrezzi& listino) {
    // Prezzi €/mq presi dal catalogo statico dei cicli
    for (std::size_t i = 0; i < getNumeroCicli(); ++i) {
        const CicloInfo& c = getCiclo(i);
        listino.impostaPrezzoMq(c.nome, c.prezzoMq);
    }

    // Coefficienti standard per difficoltà cantiere
    listino.impostaCoeff(GradoDifficolta::Nuovo,      1.00);
    listino.impostaCoeff(GradoDifficolta::Disabitato, 1.05);
    listino.impostaCoeff(GradoDifficolta::Abitato,    1.15);
}
