#ifndef VOCECARTONGESSOBUILDER_H
#define VOCECARTONGESSOBUILDER_H

#include <memory>
#include <string>

#include "GradoDifficolta.h"

// Forward declaration per evitare include pesanti qui
class ListinoPrezzi;
class VoceCosto;

class VoceCartongessoBuilder {
public:
    // Costruttore: inizializza con valori "non validi"
    VoceCartongessoBuilder();

    // Metodi di configurazione (ritornano *this per permettere il chaining)
    VoceCartongessoBuilder& setNomeCiclo(const std::string& nome);
    VoceCartongessoBuilder& setMq(double mq);
    VoceCartongessoBuilder& setListino(const ListinoPrezzi& listino);
    VoceCartongessoBuilder& setGrado(GradoDifficolta grado);

    // Costruisce l'oggetto finale (VoceCartongesso) e lo ritorna come VoceCosto
    std::unique_ptr<VoceCosto> build() const;

private:
    std::string nomeCiclo_;
    double mq_;
    const ListinoPrezzi* listino_;   // non owning
    GradoDifficolta grado_;
};


#endif