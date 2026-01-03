#include "VoceCartongessoBuilder.h"
#include "ListinoPrezzi.h"
#include "VoceCartongesso.h"

#include <stdexcept>

VoceCartongessoBuilder::VoceCartongessoBuilder()
    : nomeCiclo_()
    , mq_(-1.0)
    , listino_(nullptr)
    , grado_(GradoDifficolta::Nuovo)
{
}

VoceCartongessoBuilder& VoceCartongessoBuilder::setNomeCiclo(const std::string& nome)
{
    nomeCiclo_ = nome;
    return *this;
}

VoceCartongessoBuilder& VoceCartongessoBuilder::setMq(double mq)
{
    // Controllo
    if (mq <= 0.0) {
        throw std::runtime_error("Mq deve essere positivo");
    }

    mq_ = mq;
    return *this;
}

VoceCartongessoBuilder& VoceCartongessoBuilder::setListino(const std::shared_ptr<ListinoPrezzi>& listino)
{
    // Il builder salva il listino come shared_ptr per garantire che sia valido fino a build()
    listino_ = listino;
    return *this;
}

VoceCartongessoBuilder& VoceCartongessoBuilder::setGrado(GradoDifficolta grado)
{
    grado_ = grado;
    return *this;
}

std::unique_ptr<VoceCosto> VoceCartongessoBuilder::build() const
{
    if (nomeCiclo_.empty()) {
        throw std::runtime_error("Nome ciclo non impostato nel builder");
    }

    if (mq_ <= 0.0) {
        throw std::runtime_error("Mq non valido nel builder (deve essere > 0)");
    }

    if (!listino_) {
        throw std::runtime_error("Listino non impostato nel builder");
    }

    /*
       Creazione della voce concreta.
       - dereferenzio lo shared_ptr (*listino_) perché VoceCartongesso prende il listino
         per riferimento const (non-owning)
   */
    return std::unique_ptr<VoceCosto>(
        new VoceCartongesso(nomeCiclo_, mq_, *listino_, grado_)
    );
}
