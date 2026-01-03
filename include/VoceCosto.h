#ifndef VOCECOSTO_H
#define VOCECOSTO_H

#include <string>
#include <memory>
#include <utility>

//VoceCosto generica del preventivo (classe base astratta)
// Le classi derivate (es. Tinteggiatura, Cartongesso) implementano il calcolo del costo.

class VoceCosto {

protected:
    // protected: VoceCosto non si istanzia direttamente, ma solo tramite classi derivate

    VoceCosto(std::string  nome,
              std::string  unita,
              double quantita,
              double prezzo)
        : nome_(std::move(nome)),
          unitaMisura_(std::move(unita)),
          quantita_(quantita),
          prezzoUnitario_(prezzo),
          coefficiente_(1.0) {}

    // Dati comuni a tutte le voci (condivisi da tinteggiatura/cartongesso/altre future)
    std::string nome_;          // nome del ciclo/lavorazione (es. "Idropittura lavabile")
    std::string unitaMisura_;   // unità di misura (es. "m^2")
    double quantita_       = 0.0; // quantità inserita (es. mq)
    double prezzoUnitario_ = 0.0; // prezzo base per unità (€/mq)
    double coefficiente_   = 1.0; // maggiorazione legata al grado di difficoltà (Nuovo/Disabitato/Abitato)

public:



    // Distruttore virtuale: necessario perché gli oggetti verranno gestiti tramite puntatore a base.
    virtual ~VoceCosto() = default;


    // Il get da accesso in sola lettura ai dati comuni.
    const std::string& getNome() const          { return nome_; }
    const std::string& getUnitaMisura() const   { return unitaMisura_; }
    double getQuantita() const                  { return quantita_; }
    double getPrezzoUnitario() const            { return prezzoUnitario_; }
    double getCoefficiente() const              { return coefficiente_; }

    // Setter perchè utile in futuro se voglio modificare una voce già creata
    void setNome(const std::string& n)          { nome_ = n; }
    void setUnitaMisura(const std::string& u)   { unitaMisura_ = u; }
    void setQuantita(double q)                  { quantita_ = q; }
    void setPrezzoUnitario(double p)            { prezzoUnitario_ = p; }
    void setCoefficiente(double c)              { coefficiente_ = c; }


    // Ogni voce concreta decide la propria formula di costo.
    virtual double subtotale() const = 0;

    // Tipo logico della voce (serve per stampe/CSV senza usare dynamic_cast).
    virtual const char* tipoVoce() const = 0;

    // Copia polimorfa ("virtual constructor"): permette di copiare una voce concreta tramite puntatore a base.2
    virtual std::unique_ptr<VoceCosto> clone() const = 0;

};

#endif

