#ifndef PREVENTIVO_H
#define PREVENTIVO_H
#include <string> // per std::string
#include <vector> // per std::vector
#include <memory> // per std::unique_ptr
#include "GradoDifficolta.h"
#include "VoceCosto.h"

//Classe per rappresentare il preventivo completo del cliente
class Preventivo {
private:
    std::string id_;       // es. "P2025-001"
    std::string cliente_;  // nome del cliente
    GradoDifficolta grado_;

    // Contenitore polimorfo di voci: ogni elemento è un unique_ptr<VoceCosto>
    std::vector<std::unique_ptr<VoceCosto>> voci_;

public:
    //Costruttore di default
    Preventivo()
        : id_(), cliente_(), grado_(GradoDifficolta::Nuovo) {}

    //Costruttore base con id, nome cliente , grado difficoltà
    Preventivo(std::string id, std::string cliente, GradoDifficolta grado);

    //COPY & MOVE

    Preventivo(const Preventivo& other);
    //Copy constructor: copia profonda polimorfa delle voci tramite clone()

    Preventivo &operator=(const Preventivo &other);

    //Copy Assignment:  prima libero le voci, poi ricopio tramite clone()

    Preventivo(Preventivo&& other) noexcept;        //Move constructor: trasferisco risorse da other a this.
    Preventivo& operator=(Preventivo&& other) noexcept;
    //Move assignment: trasferisco risorse e lascio other in stato valido ma "vuoto"

    ~Preventivo() = default;                        //Distruttore di default

    //GESTIONE VOCI

    //Aggiunge una voce passando l'ownership del unique_ptr al vettore interno
    void aggiungiVoce(std::unique_ptr<VoceCosto> voce);

    //Operator +=
    Preventivo& operator+=(std::unique_ptr<VoceCosto> voce);

    //Calcolo del totale (con std::accumulate + lambda)
    double totale() const;

    double totaleMq() const;

    template <typename F>
    double aggrega (F funzione, double valoreIniziale) const {
        double acc = valoreIniziale;
        for (const std::unique_ptr<VoceCosto>& vocePtr : voci_) {
            if (vocePtr) {
                acc = funzione(acc,*vocePtr);
            }
        }
        return acc;
    }

    // Ordina le voci in base al nome (ordine alfabetico crescente)
    void ordinaPerNome();

    //Riepilogo in formato stringa (id, cliente, numero voci, totale)
    std::string riepilogo() const;

    //GETTER
    const std::string& getId() const      { return id_; }
    const std::string& getCliente() const { return cliente_; }
    GradoDifficolta getGrado() const      { return grado_; }

    //Ritorno una reference const al vettore, utile per operator+ o eventuali stampe
    const std::vector<std::unique_ptr<VoceCosto>>& getVoci() const {
        return voci_;
    }


};

// Operatore + : combina due preventivi (somma delle voci)
Preventivo operator+(const Preventivo& a, const Preventivo& b);

#endif //PROJECT_PREVENTIVO_H