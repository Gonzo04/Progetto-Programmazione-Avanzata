#ifndef PREVENTIVO_H
#define PREVENTIVO_H
#include <string> // per std::string
#include <vector> // per std::vector
#include <memory> // per std::unique_ptr
#include <set>    // per std::set (no duplicati)
#include "GradoDifficolta.h"
#include "VoceCosto.h"

//Classe per rappresentare il preventivo completo del cliente
class Preventivo {
private:
    std::string id_;       // Id Preventivo generato es. "P2025-001"
    std::string cliente_;  // nome del cliente
    GradoDifficolta grado_;

    // Contenitore polimorfo di voci: ogni voce è un oggetto derivato da VoceCosto
    std::vector<std::unique_ptr<VoceCosto>> voci_;

     /*
        Set di chiavi logiche (tipo|nome).
        Uso il set come "guardia" per sapere se una lavorazione è già presente.
        Poi il vector resta il contenitore reale delle voci (serve per ordinare e stampare).
    */
    std::set<std::string> chiaviVoci_;

public:
    //Costruttore di default
    Preventivo()
        : id_(), cliente_(), grado_(GradoDifficolta::Nuovo) {}

    //Costruttore base con id, nome cliente , grado difficoltà
    Preventivo(std::string id, std::string cliente, GradoDifficolta grado);

    //COPY & MOVE

    //Copy constructor: copia profonda polimorfa delle voci tramite clone()
    Preventivo(const Preventivo& other);
    //Copy Assignment: prima libero le voci, poi ricopio tramite clone()
    Preventivo &operator=(const Preventivo &other);


    //Move constructor: trasferisco risorse da other a this.
    Preventivo(Preventivo&& other) noexcept;
    Preventivo& operator=(Preventivo&& other) noexcept;

    ~Preventivo() = default;                        //Distruttore di default

    //GESTIONE VOCI

    // Aggiunge una voce passando ownership (unique_ptr)
    // Se la voce esiste già (stesso tipo+nome), unisco le quantità.
    void aggiungiVoce(std::unique_ptr<VoceCosto> voce);

    //Operator +=
    Preventivo& operator+=(std::unique_ptr<VoceCosto> voce);

    //Calcolo del totale (con std::accumulate + lambda)
    double totale() const;

    // Totale mq: sommo le quantità (uso un template)
    double totaleMq() const;

    // Funzione Template generica
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

    //GETTER (per salvataggio e stampa)
    const std::string& getId() const      { return id_; }
    const std::string& getCliente() const { return cliente_; }
    GradoDifficolta getGrado() const      { return grado_; }

    // Accesso in sola lettura alle voci
    const std::vector<std::unique_ptr<VoceCosto>>& getVoci() const {
        return voci_;
    }


};


#endif
