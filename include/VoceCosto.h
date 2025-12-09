#ifndef VOCECOSTO_H
#define VOCECOSTO_H

#include <string>             //std::string
#include <memory>             //std::unique_ptr

//Classe astratta che rappresenta una voce generica di preventivo.
//Tutte le voci concrete (tinteggiatura, cartongesso, ecc.) erediteranno da qui
class VoceCosto {
public:
    //Distruttore virtuale: necessario per la classe polimorfa
    //tramite puntatori (std::unique_ptr<VoceCosto>)
    virtual ~VoceCosto() = default;

    //GETTER
    //Restituiscono una referenza costante: non viene copiata la stringa
    const std::string& getNome() const          { return nome_; }
    const std::string& getUnitaMisura() const   { return unitaMisura_; }
    double getQuantita() const                  { return quantita_; }
    double getPrezzoUnitario() const            { return prezzoUnitario_; }
    double getCoefficiente() const              { return coefficiente_; }

    //SETTER
    //Permettono di impostare i valori con semplici assegnazioni
    void setNome(const std::string& n)          { nome_ = n; }
    void setUnitaMisura(const std::string& u)   { unitaMisura_ = u; }
    void setQuantita(double q)                  { quantita_ = q; }
    void setPrezzoUnitario(double p)            { prezzoUnitario_ = p; }
    void setCoefficiente(double c)              { coefficiente_ = c; }

    //Ogni sottoclasse deve definire la propria formula di subtotale
    //Es: tinteggiatura: mq * prezzo * coefficiente.
    virtual double subtotale() const = 0;


    virtual std::unique_ptr<VoceCosto> clone() const = 0; //usato per fare copie polimorfe delle voci (deep copy)


protected:
    //PROTECTED: accessibili dalle derivate ma non dall'esterno.
    //dall'esterno si passa per il getter e setter
    VoceCosto(const std::string& nome,
              const std::string& unita,
              double quantita,
              double prezzo)
        : nome_(nome),
          unitaMisura_(unita),
          quantita_(quantita),
          prezzoUnitario_(prezzo),
          coefficiente_(1.0) {}

    std::string nome_;          // es. "Ciclo idropittura traspirante..."
    std::string unitaMisura_;   // es. "m^2"
    double quantita_       = 0.0;  // es. metri quadri
    double prezzoUnitario_ = 0.0;  // prezzo per unità (€/m^2)
    double coefficiente_   = 1.0;  // coeff. (nuovo / disabitato / abitato)

};

#endif
