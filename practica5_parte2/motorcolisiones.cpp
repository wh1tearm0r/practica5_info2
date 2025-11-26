#include "MotorColisiones.h"
#include <cmath>

MotorColisiones::MotorColisiones(double ancho, double alto, double coefMuros)
    : limitesEscenario(0, 0, ancho, alto),
    coefRestitucionMuros(coefMuros)
{
}

InfoColision MotorColisiones::verificarColisiones(
    Proyectil& proyectil,
    const std::vector<Muro*>& muros,
    const QPointF& posRival1,
    const QPointF& posRival2)
{
    InfoColision info;

    if (!proyectil.estaActiva()) {
        return info;
    }

    //Verificar impacto con rivales (fin del juego)
    int idJugadorProyectil = proyectil.getIdJugador();

    // El jugador 1 intenta impactar al rival 2 y viceversa
    if (idJugadorProyectil == 1) {
        if (verificarImpactoRival(proyectil, posRival2)) {
            info.ocurrioColision = true;
            info.tipoColision = "rival";
            info.momentoImpacto = calcularMomentoLineal(proyectil);
            info.indiceObjetoImpactado = 2;
            info.puntoImpacto = QPointF(proyectil.getX(), proyectil.getY());

            proyectil.desactivar();
            return info;
        }
    } else if (idJugadorProyectil == 2) {
        if (verificarImpactoRival(proyectil, posRival1)) {
            info.ocurrioColision = true;
            info.tipoColision = "rival";
            info.momentoImpacto = calcularMomentoLineal(proyectil);
            info.indiceObjetoImpactado = 1;
            info.puntoImpacto = QPointF(proyectil.getX(), proyectil.getY());

            proyectil.desactivar();
            return info;
        }
    }

    // Colisiones con muros (inelásticas)
    for (size_t i = 0; i < muros.size(); i++) {
        if (muros[i]->estaDestruido()) continue;

        if (muros[i]->getIdPropietario() == idJugadorProyectil) continue;

        if (procesarColisionMuro(proyectil, *muros[i])) {
            info.ocurrioColision = true;
            info.tipoColision = "muro";
            info.momentoImpacto = calcularMomentoLineal(proyectil);
            info.indiceObjetoImpactado = static_cast<int>(i);
            info.puntoImpacto = QPointF(proyectil.getX(), proyectil.getY());
            return info;
        }
    }

    // Colisiones con paredes (elásticas)
    if (procesarColisionParedes(proyectil)) {
        info.ocurrioColision = true;
        info.tipoColision = "pared";
        info.momentoImpacto = calcularMomentoLineal(proyectil);
        info.puntoImpacto = QPointF(proyectil.getX(), proyectil.getY());

        return info;
    }

    return info;
}

bool MotorColisiones::procesarColisionParedes(Proyectil& proyectil) {
    double x = proyectil.getX();
    double y = proyectil.getY();
    double radio = proyectil.getRadio();

    bool huboRebote = false;

    if (x - radio <= limitesEscenario.left()) {
        proyectil.setX(limitesEscenario.left() + radio + 1.0);
        proyectil.rebotarParedVertical();
        huboRebote = true;
    }
    if (x + radio >= limitesEscenario.right()) {
        proyectil.setX(limitesEscenario.right() - radio - 1.0);
        proyectil.rebotarParedVertical();
        huboRebote = true;
    }
    if (y - radio <= limitesEscenario.top()) {
        proyectil.setY(limitesEscenario.top() + radio + 1.0);
        proyectil.rebotarParedHorizontal();
        huboRebote = true;
    }
    if (y + radio >= limitesEscenario.bottom()) {
        proyectil.setY(limitesEscenario.bottom() - radio - 1.0);
        proyectil.rebotarParedHorizontal();
        huboRebote = true;
    }

    return huboRebote;
}

bool MotorColisiones::procesarColisionMuro(Proyectil& proyectil, Muro& muro) {
    bool huboColision = muro.aplicarRebote(proyectil);

    return huboColision;
}

bool MotorColisiones::verificarImpactoRival(const Proyectil& proyectil,
                                            const QPointF& posRival,
                                            double radioDeteccion) const {
    if (!proyectil.estaActiva()) return false;

    double dx = proyectil.getX() - posRival.x();
    double dy = proyectil.getY() - posRival.y();
    double distancia = sqrt(dx*dx + dy*dy);

    return distancia <= (proyectil.getRadio() + radioDeteccion);
}

double MotorColisiones::calcularMomentoLineal(const Proyectil& proyectil) const {
    return proyectil.getMomentoLineal();
}

bool MotorColisiones::puntoEnRectangulo(const QPointF& punto,
                                        const QRectF& rect,
                                        double margen) const {
    return (punto.x() >= rect.left() - margen &&
            punto.x() <= rect.right() + margen &&
            punto.y() >= rect.top() - margen &&
            punto.y() <= rect.bottom() + margen);
}

