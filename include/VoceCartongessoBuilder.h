#ifndef VOCECARTONGESSOBUILDER_H
#define VOCECARTONGESSOBUILDER_H

#include <memory>
#include <string>

#include "GradoDifficolta.h"
/*
 NOTE DI DESIGN: PATTERN BUILDER
 Il Builder è stato implementato specificamente per la classe VoceCartongesso
 in previsione di una maggiore complessità di configurazione
 (es. parametri futuri come isolamento termico, certificazione REI, tipologia lastra).

 Per la classe VoceTinteggiatura, data la sua semplicità strutturale (pochi parametri),
 abbiamo optato per un costruttore standard per evitare over-engineering.
 */

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
    VoceCartongessoBuilder& setListino(const std::shared_ptr<ListinoPrezzi> & listino);
    VoceCartongessoBuilder& setGrado(GradoDifficolta grado);

    // Costruisce l'oggetto finale (VoceCartongesso) e lo ritorna come VoceCosto
    std::unique_ptr<VoceCosto> build() const;

private:
    std::string nomeCiclo_;
    double mq_;
    std::shared_ptr<ListinoPrezzi> listino_;
    GradoDifficolta grado_;
};


#endif