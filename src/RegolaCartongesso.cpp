#include "RegolaCartongesso.h"
#include "VoceCartongessoBuilder.h"

// [MODIFICA] Aggiornata la firma dell'implementazione
std::unique_ptr<VoceCosto> RegolaCartongesso::creaVoce(
    const std::string& nomeCiclo,
    double mq,
    const std::shared_ptr<ListinoPrezzi>& listino,
    GradoDifficolta grado
) const {

    VoceCartongessoBuilder builder;

    // Ora 'listino' è uno shared_ptr, quindi setListino(listino) funziona perfettamente!
    return builder.setNomeCiclo(nomeCiclo)
                  .setMq(mq)
                  .setListino(listino)
                  .setGrado(grado)
                  .build();
}