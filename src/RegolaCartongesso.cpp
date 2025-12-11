#include "RegolaCartongesso.h"
#include "VoceCartongessoBuilder.h"

std::unique_ptr<VoceCosto> RegolaCartongesso::creaVoce(const std::string& nomeCiclo,
                            double mq,
                            const ListinoPrezzi& listino,
                            GradoDifficolta grado) const
{
    VoceCartongessoBuilder builder;

    builder.setNomeCiclo(nomeCiclo)
           .setMq(mq)
           .setListino(listino)
           .setGrado(grado);

    return builder.build();
}
