#include "SalvataggioPreventivo.h"
#include "Preventivo.h"
#include "VoceCartongesso.h"

#include <fstream>
#include <iomanip>
#include <stdexcept>


void salvaPreventivoSuTxt(const Preventivo& p, const std::string& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        throw std::runtime_error("Impossibile aprire file TXT: " + filename);
    }
    out << p.riepilogo();
}

void salvaPreventivoSuCsv(const Preventivo& p, const std::string& filename) {
    std::ofstream out(filename.c_str());
    if (!out) {
        throw std::runtime_error("Impossibile aprire file CSV: " + filename);
    }

    out << std::fixed << std::setprecision(2);
    out << "TipoVoce;Nome;Unita;Quantita;PrezzoUnitario;Coefficiente;Subtotale\n";

    const auto& voci = p.getVoci();
    for (const auto& vocePtr : voci) {
        if (!vocePtr) continue;

        // Commento da studente:
        // Qui uso dynamic_cast per capire se la voce concreta è Cartongesso o Tinteggiatura.
        // Alternativa più OO: metodo virtuale tipo getTipoVoce(), ma per ora manteniamo questo semplice.
        const auto* vc = dynamic_cast<const VoceCartongesso*>(vocePtr.get());
        const std::string tipo = (vc ? "Cartongesso" : "Tinteggiatura");

        out << tipo << ";"
            << vocePtr->getNome() << ";"
            << vocePtr->getUnitaMisura() << ";"
            << vocePtr->getQuantita() << ";"
            << vocePtr->getPrezzoUnitario() << ";"
            << vocePtr->getCoefficiente() << ";"
            << vocePtr->subtotale() << "\n";
    }

    double imp = p.totale();
    out << "TotaleImponibile;;;;;" << imp << "\n";
    out << "IVA(22%);;;;;" << imp * 0.22 << "\n";
    out << "TotaleComplessivo;;;;;" << imp * 1.22 << "\n";
}
