#ifndef VOCECARTONGESSOBUILDER_H
#define VOCECARTONGESSOBUILDER_H

#include <memory>
#include <string>

#include "GradoDifficolta.h"

/*
    VoceCartongessoBuilder (Pattern Builder)


    - raccoglie i parametri necessari per costruire una VoceCartongesso
    - valida i parametri (es. mq > 0, listino presente, nome ciclo non vuoto)
    - crea l'oggetto finale tramite build()

    Perché Builder solo per cartongesso:
    - cartongesso è previsto diventi più complesso (parametri futuri)
    mentre tinteggiatura è più semplice

    Output:
    - build() ritorna std::unique_ptr<VoceCosto>
    per inserirlo nel Preventivo senza gestione manuale della memoria.
*/
class ListinoPrezzi;
class VoceCosto;

class VoceCartongessoBuilder {

private:
    std::string nomeCiclo_;                 // nome del ciclo scelto dall'utente
    double mq_;                             // metri quadri
    std::shared_ptr<ListinoPrezzi> listino_; // risorsa condivisa: garantisce lifetime fino a build()
    GradoDifficolta grado_;                 // difficoltà del cantiere (coefficiente)

public:
    // Inizializza un builder in stato "incompleto" che verra completato tramite i set
    VoceCartongessoBuilder();

    // Metodi di configurazione: impostano un parametro e ritornano *this per chaining
    VoceCartongessoBuilder& setNomeCiclo(const std::string& nome);
    VoceCartongessoBuilder& setMq(double mq);
    VoceCartongessoBuilder& setListino(const std::shared_ptr<ListinoPrezzi> & listino);
    VoceCartongessoBuilder& setGrado(GradoDifficolta grado);

    // Valida i parametri e costruisce la VoceCartongesso concreta
    std::unique_ptr<VoceCosto> build() const;

};


#endif