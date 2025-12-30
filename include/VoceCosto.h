#ifndef VOCECOSTO_H
#define VOCECOSTO_H

#include <string>             //std::string
#include <memory>             //std::unique_ptr
//Voce di Costo generica del preventivo (classe base astratta)
// Classe base astratta per una voce di costo nel preventivo
// Le classi derivate (es. tinteggiatura, cartongesso) implementano il calcolo del costo.
class VoceCosto {
public:



    //distruttore virtuale derivato dall'uso polimorfo
    virtual ~VoceCosto() = default;

    // Getter: per le stringhe uso const& per evitare copie

    const std::string& getNome() const          { return nome_; }
    const std::string& getUnitaMisura() const   { return unitaMisura_; }
    double getQuantita() const                  { return quantita_; }
    double getPrezzoUnitario() const            { return prezzoUnitario_; }
    double getCoefficiente() const              { return coefficiente_; }

    // Setter: in caso di modifica alla voce dopo la creazione

    void setNome(const std::string& n)          { nome_ = n; }
    void setUnitaMisura(const std::string& u)   { unitaMisura_ = u; }
    void setQuantita(double q)                  { quantita_ = q; }
    void setPrezzoUnitario(double p)            { prezzoUnitario_ = p; }
    void setCoefficiente(double c)              { coefficiente_ = c; }

    // Ogni sottoclasse definisce come calcolare il costo della voce
    //Es: tinteggiatura: mq * prezzo * coefficiente.
    virtual double subtotale() const = 0;

    // Copia polimorfa: serve quando devo copiare un preventivo con voci di tipo diverso
    virtual std::unique_ptr<VoceCosto> clone() const = 0;


protected:
    // protected: VoceCosto non si istanzia direttamente, ma solo tramite classi derivate

    VoceCosto(const std::string& nome,
              const std::string& unita,
              double quantita,
              double prezzo)
        : nome_(nome),
          unitaMisura_(unita),
          quantita_(quantita),
          prezzoUnitario_(prezzo),
          coefficiente_(1.0) {}

    std::string nome_;          // nome del ciclo/lavorazione
    std::string unitaMisura_;   // es. "m^2"
    double quantita_       = 0.0;
    double prezzoUnitario_ = 0.0;  // €/unità
    double coefficiente_   = 1.0;  // coeff. legato alla difficoltà


};

#endif

// Questo file definisce la classe astratta VoceCosto,
// che rappresenta una singola voce di costo dentro un preventivo.
// La classe viene usata in modo polimorfo: il preventivo conserva le voci come std::unique_ptr<VoceCosto>
// così posso mettere nello stesso contenitore voci di tipi diversi (es. tinteggiatura e cartongesso).
// Ogni classe derivata deve implementare subtotale() per calcolare il costo della voce e clone()
// per permettere la copia profonda polimorfa
// (necessaria quando si copia un Preventivo, dato che unique_ptr non è copiabile e non posso copiare oggetti senza conoscere il tipo concreto).
// Il costruttore è protected perché VoceCosto non deve essere istanziata direttamente ma solo tramite classi derivate.