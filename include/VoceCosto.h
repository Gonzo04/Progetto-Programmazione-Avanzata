#ifndef VOCECOSTO_H
#define VOCECOSTO_H

#include <string>
#include <memory>
#include <utility>
//VoceCosto generica del preventivo (classe base astratta)
// Le classi derivate (es. Tinteggiatura, Cartongesso) implementano il calcolo del costo.
class VoceCosto {
public:



    // Distruttore virtuale: necessario perché gli oggetti verranno gestiti tramite puntatore a base.
    virtual ~VoceCosto() = default;

    // Dati comuni a tutte le voci
    const std::string& getNome() const          { return nome_; }
    const std::string& getUnitaMisura() const   { return unitaMisura_; }
    double getQuantita() const                  { return quantita_; }
    double getPrezzoUnitario() const            { return prezzoUnitario_; }
    double getCoefficiente() const              { return coefficiente_; }

    // Setter: utili in futuro voglio per modificare una voce già creata
    void setNome(const std::string& n)          { nome_ = n; }
    void setUnitaMisura(const std::string& u)   { unitaMisura_ = u; }
    void setQuantita(double q)                  { quantita_ = q; }
    void setPrezzoUnitario(double p)            { prezzoUnitario_ = p; }
    void setCoefficiente(double c)              { coefficiente_ = c; }


    // Ogni voce concreta decide la propria formula di costo.
    virtual double subtotale() const = 0;

    // Tipo logico della voce (serve per stampe/CSV senza usare dynamic_cast).
    virtual const char* tipoVoce() const = 0;

    // Copia polimorfa: permette la "deep copy" di oggetti gestiti tramite puntatore a base.
    virtual std::unique_ptr<VoceCosto> clone() const = 0;


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

    std::string nome_;          // nome del ciclo/lavorazione
    std::string unitaMisura_;   // es. "m^2"
    double quantita_       = 0.0;
    double prezzoUnitario_ = 0.0;  // €/unità
    double coefficiente_   = 1.0;  // Coeff. Legato alla difficoltà


};

#endif

/*
  VoceCosto è la classe base astratta per una voce di preventivo.
  Il Preventivo memorizza le voci tramite std::unique_ptr<VoceCosto>,
  così nello stesso contenitore posso tenere voci di tipi diversi (es. tinteggiatura, cartongesso) usando il polimorfismo.

  Ogni classe derivata deve implementare:
  - subtotale(): la formula di calcolo del costo della voce
  - clone(): una copia polimorfa (deep copy), utile quando si vuole copiare un Preventivo,
    perché std::unique_ptr non è copiabile e il tipo concreto della voce è noto solo a runtime.

  Il costruttore è protected perché VoceCosto non deve essere creata direttamente:
  si istanzia solo tramite classi derivate.
 */
