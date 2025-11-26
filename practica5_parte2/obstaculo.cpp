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
    return distancia < radio;
}
Obstaculo::Lado Obstaculo::determinarLadoColision(const Particula& p) const {
    double px = p.getX();
    double py = p.getY();

    double distIzq = std::abs(px - x);
    double distDer = std::abs(px - (x + ancho));
    double distSup = std::abs(py - y);
    double distInf = std::abs(py - (y + alto));
    double minDist = std::min({distIzq, distDer, distSup, distInf});

    if (minDist == distIzq && px < x + ancho/2) return IZQUIERDO;
    if (minDist == distDer && px > x + ancho/2) return DERECHO;
    if (minDist == distSup && py < y + alto/2) return SUPERIOR;
    if (minDist == distInf && py > y + alto/2) return INFERIOR;

    return NINGUNO;
}

bool Obstaculo::aplicarRebote(Particula& p) const {
    if (!colisionaCon(p)) return false;

    Lado lado = determinarLadoColision(p);

    if (lado == NINGUNO) return false;

    double vx = p.getVx();
    double vy = p.getVy();
    double radio = p.getRadio();

    switch(lado) {
    case IZQUIERDO:
        p.setX(x - radio - 1.0);
        p.setVx(-coefRestitucion * std::abs(vx));
        break;
    case DERECHO:
        p.setX(x + ancho + radio + 1.0);
        p.setVx(coefRestitucion * std::abs(vx));
        break;

    case SUPERIOR:
        p.setY(y - radio - 1.0);
        p.setVy(-coefRestitucion * std::abs(vy));
        break;
    case INFERIOR:
        p.setY(y + alto + radio + 1.0);
        p.setVy(coefRestitucion * std::abs(vy));
        break;

    case NINGUNO:
        break;
    }

    return true;
}
