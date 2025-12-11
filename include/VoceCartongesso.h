#ifndef VOCECARTONGESSO_H //Voce per il calcolo dei costi dei vari tipi di cartongessi
#define VOCECARTONGESSO_H

#include <string>
#include <memory>

#include "ListinoPrezzi.h" //Per il parametro del costruttore
#include "VoceCosto.h" //classe da cui eredito
#include "GradoDifficolta.h"//per il tipo dell'argomento grado

class VoceCartongesso : public VoceCosto {
// :public VoceCosto perchè eredita da VoceCosto
public:
    VoceCartongesso(const std::string& NomeCiclo,double mq, const ListinoPrezzi& listino,GradoDifficolta grado);

    virtual double subtotale() const override;
    virtual std::unique_ptr<VoceCosto> clone() const override;
};
#endif