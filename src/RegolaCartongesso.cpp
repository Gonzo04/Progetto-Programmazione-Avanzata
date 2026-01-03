#include "RegolaCartongesso.h"
#include "VoceCartongessoBuilder.h"

// [MODIFICA] Aggiornata la firma dell'implementazione
std::unique_ptr<VoceCosto> RegolaCartongesso::creaVoce(
    const std::string& nomeCiclo,
    double mq,
    const std::shared_ptr<ListinoPrezzi>& listino,
    GradoDifficolta grado
) const {
    /*
    Il listino è una risorsa condivisa a livello applicativo (shared_ptr).
    La regola lo usa solo in lettura per permettere al builder di ricavare
    prezzo unitario e coefficiente.
    */

    // Controllo
    if (!listino) {
        throw std::runtime_error("Listino nullo nella regola cartongesso.");
    }

    /*
    Builder:
   - raccoglie i parametri necessari alla costruzione
   - build() crea e ritorna la voce concreta (unique_ptr<VoceCosto>)
   */

    VoceCartongessoBuilder builder;

    return builder.setNomeCiclo(nomeCiclo)
                  .setMq(mq)
                  .setListino(listino)
                  .setGrado(grado)
                  .build();
}