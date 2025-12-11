#ifndef REGOLACOSTO_H
#define REGOLACOSTO_H

#include <string>
#include <iostream>
#include <memory>

#include "VoceCosto.h"
#include "GradoDifficolta.h"
#include "ListinoPrezzi.h"

class RegolaCosto {
    public:
        virtual ~RegolaCosto() {}
        virtual std::unique_ptr<VoceCosto>
        creaVoce(const std::string &nomeCiclo,double mq,const ListinoPrezzi &listino,GradoDifficolta grado) const=0;
};

#endif