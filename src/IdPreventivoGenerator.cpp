#include "IdPreventivoGenerator.h"

#include <atomic>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>

static std::atomic<int> g_contatorePreventivi(1);

// Mutex per proteggere la chiamata a std::localtime la quale non è thread-safe
static std::mutex g_timeMutex;

std::string generaIdPreventivo() {
    // Leggo la data corrente
    std::time_t now = std::time(nullptr);

    // Copio la struct tm in una variabile locale
    std::tm tmCopy;
    {
        std::lock_guard<std::mutex> lock(g_timeMutex);
        std::tm* ptm = std::localtime(&now);
        tmCopy = *ptm;
    }

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
