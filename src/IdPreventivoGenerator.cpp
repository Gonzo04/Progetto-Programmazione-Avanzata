#include "IdPreventivoGenerator.h"
#include "Utils.h"

#include <atomic>
#include <iomanip>
#include <sstream>


static std::atomic<int> g_contatorePreventivi(1);

std::string generaIdPreventivo() {

    // Recupero della data in modo sicuro da Utils
    std::tm tmCopy = getLocalTimeSafe();

    // Trasformo la data in stringa YYYYMMDD usando la copia
    char data[16];
    std::strftime(data, sizeof(data), "%Y%m%d", &tmCopy);

    int progressivo = g_contatorePreventivi.fetch_add(1);

    // Compongo l'ID finale: PYYYYMMDD-XXX (XXX a 3 cifre con zeri davanti)
    std::ostringstream oss;
    oss << "P" << data << "-"
        << std::setw(3) << std::setfill('0') << progressivo;

    return oss.str();
}
