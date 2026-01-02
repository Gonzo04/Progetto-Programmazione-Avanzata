#include "IdPreventivoGenerator.h"

#include <atomic>
#include <ctime>
#include <iomanip>
#include <sstream>

static std::atomic<int> g_contatorePreventivi(1);

std::string generaIdPreventivo() {
    std::time_t now = std::time(nullptr);
    std::tm* ptm = std::localtime(&now); // ok: qui siamo nel main, non in concorrenza

    char data[16];
    std::strftime(data, sizeof(data), "%Y%m%d", ptm);

    int progressivo = g_contatorePreventivi.fetch_add(1);

    std::ostringstream oss;
    oss << "P" << data << "-"
        << std::setw(3) << std::setfill('0') << progressivo;

    return oss.str();
}
