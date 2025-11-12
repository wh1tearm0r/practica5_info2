#include "Obstaculo.h"
#include "Particula.h"
#include <cmath>
#include <algorithm>

// Constructor
Obstaculo::Obstaculo(double x0, double y0, double w, double h, double e)
    : x(x0), y(y0), ancho(w), alto(h), coefRestitucion(e) {
}

// Detectar colisión con partícula
bool Obstaculo::colisionaCon(const Particula& p) const {
    if (!p.estaActiva()) return false;

    double px = p.getX();
    double py = p.getY();
    double radio = p.getRadio();

    // Encontrar el punto más cercano del rectángulo al centro de la partícula
    double puntoMasCercanoX = std::max(x, std::min(px, x + ancho));
    double puntoMasCercanoY = std::max(y, std::min(py, y + alto));

    // Calcular distancia del centro de la partícula al punto más cercano
    double dx = px - puntoMasCercanoX;
    double dy = py - puntoMasCercanoY;
    double distancia = sqrt(dx*dx + dy*dy);

    // Hay colisión si la distancia es menor o igual al radio
    return distancia <= radio;
}

// Determinar con qué lado del obstáculo colisionó la partícula
Obstaculo::Lado Obstaculo::determinarLadoColision(const Particula& p) const {
    double px = p.getX();
    double py = p.getY();
    double radio = p.getRadio();

    // Calcular distancias a cada lado
    double distIzq = std::abs(px - x);
    double distDer = std::abs(px - (x + ancho));
    double distSup = std::abs(py - y);
    double distInf = std::abs(py - (y + alto));

    // Encontrar la distancia mínima
    double minDist = std::min({distIzq, distDer, distSup, distInf});

    // Determinar qué lado es
    if (minDist == distIzq && px < x) return IZQUIERDO;
    if (minDist == distDer && px > x + ancho) return DERECHO;
    if (minDist == distSup && py < y) return SUPERIOR;
    if (minDist == distInf && py > y + alto) return INFERIOR;

    return NINGUNO;
}

// Aplicar rebote inelástico
bool Obstaculo::aplicarRebote(Particula& p) const {
    if (!colisionaCon(p)) return false;

    // Determinar el lado de colisión
    Lado lado = determinarLadoColision(p);

    double vx = p.getVx();
    double vy = p.getVy();

    switch(lado) {
    case IZQUIERDO:
    case DERECHO:
        // Colisión vertical - afecta velocidad en X
        // v'_perpendicular = -e * v_perpendicular
        // v'_paralela = v_paralela
        p.setVx(-coefRestitucion * vx);  // Componente perpendicular
        // vy se mantiene igual (componente paralela)
        break;

    case SUPERIOR:
    case INFERIOR:
        // Colisión horizontal - afecta velocidad en Y
        p.setVy(-coefRestitucion * vy);  // Componente perpendicular
        // vx se mantiene igual (componente paralela)
        break;

    case NINGUNO:
        // No debería llegar aquí, pero por si acaso
        break;
    }

    return true;
}
