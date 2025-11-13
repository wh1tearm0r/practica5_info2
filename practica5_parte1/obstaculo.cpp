#include "Obstaculo.h"
#include "Particula.h"
#include <cmath>
#include <algorithm>

Obstaculo::Obstaculo(double x0, double y0, double w, double h, double e)
    : x(x0), y(y0), ancho(w), alto(h), coefRestitucion(e) {
}
bool Obstaculo::colisionaCon(const Particula& p) const {
    if (!p.estaActiva()) return false;

    double px = p.getX();
    double py = p.getY();
    double radio = p.getRadio();

    double puntoMasCercanoX = std::max(x, std::min(px, x + ancho));
    double puntoMasCercanoY = std::max(y, std::min(py, y + alto));
    double dx = px - puntoMasCercanoX;
    double dy = py - puntoMasCercanoY;
    double distancia = sqrt(dx*dx + dy*dy);
    return distancia <= radio;
}
Obstaculo::Lado Obstaculo::determinarLadoColision(const Particula& p) const {
    double px = p.getX();
    double py = p.getY();
    double radio = p.getRadio();

    double distIzq = std::abs(px - x);
    double distDer = std::abs(px - (x + ancho));
    double distSup = std::abs(py - y);
    double distInf = std::abs(py - (y + alto));
    double minDist = std::min({distIzq, distDer, distSup, distInf});

    if (minDist == distIzq && px < x) return IZQUIERDO;
    if (minDist == distDer && px > x + ancho) return DERECHO;
    if (minDist == distSup && py < y) return SUPERIOR;
    if (minDist == distInf && py > y + alto) return INFERIOR;

    return NINGUNO;
}

bool Obstaculo::aplicarRebote(Particula& p) const {
    if (!colisionaCon(p)) return false;

    Lado lado = determinarLadoColision(p);

    double vx = p.getVx();
    double vy = p.getVy();

    switch(lado) {
    case IZQUIERDO:
    case DERECHO:
        p.setVx(-coefRestitucion * vx);
        break;

    case SUPERIOR:
    case INFERIOR:

        p.setVy(-coefRestitucion * vy);
        break;

    case NINGUNO:
        break;
    }

    return true;
}
