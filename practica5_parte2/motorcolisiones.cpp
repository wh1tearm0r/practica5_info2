#include "motorcolisiones.h"
#include <cmath>

MotorColisiones::MotorColisiones(double ancho, double alto, double coefPared)
    : anchoEscenario(ancho), altoEscenario(alto), coefRestitucionParedes(coefPared) {
}

InfoColision MotorColisiones::verificarColisiones(Proyectil &p,
                                                  std::vector<Muro*> &muros,
                                                  const QPointF &rivalJ1,
                                                  const QPointF &rivalJ2,
                                                  int jugadorAtacante)
{
    InfoColision info;
    info.ocurrioColision = false;

    double px = p.getX();
    double py = p.getY();
    double radio = p.getRadio();

    // COLISION CON PAREDES (ELASTICA)

    // Pared izquierda
    if (px - radio <= 0) {
        p.setX(radio + 0.5);
        p.rebotarParedVertical();
        info.ocurrioColision = true;
        info.tipoColision = "pared";
        return info;
    }

    // Pared derecha
    if (px + radio >= anchoEscenario) {
        p.setX(anchoEscenario - radio - 0.5);
        p.rebotarParedVertical();
        info.ocurrioColision = true;
        info.tipoColision = "pared";
        return info;
    }

    // Pared superior
    if (py - radio <= 0) {
        p.setY(radio + 0.5);
        p.rebotarParedHorizontal();
        info.ocurrioColision = true;
        info.tipoColision = "pared";
        return info;
    }

    // Pared inferior (SUELO)
    if (py + radio >= altoEscenario) {
        p.setY(altoEscenario - radio - 0.5);
        p.rebotarParedHorizontal();
        info.ocurrioColision = true;
        info.tipoColision = "pared";
        return info;
    }

    // COLISION CON MUROS (INELASTICA)
    // SOLO CON MUROS DEL ENEMIGO

    for (size_t i = 0; i < muros.size(); i++) {
        if (muros[i] && !muros[i]->estaDestruido()) {

            // Solo colisionar con muros del OTRO jugador
            int propietarioMuro = muros[i]->getIdPropietario();

            // Si el muro es del atacante, IGNORAR
            if (propietarioMuro == jugadorAtacante) {
                continue;  // Saltar este muro
            }

            // Si llegamos aqui, el muro es del enemigo
            if (muros[i]->colisionaCon(p)) {
                muros[i]->aplicarRebote(p);

                double momento = p.getMomentoLineal();

                info.ocurrioColision = true;
                info.tipoColision = "muro";
                info.momentoImpacto = momento;
                info.indiceObjetoImpactado = static_cast<int>(i);
                info.puntoImpacto = QPointF(px, py);

                return info;
            }
        }
    }

    // COLISION CON RIVALES

    // Si es turno del Jugador 1, solo puede impactar al Rival 2
    if (jugadorAtacante == 1) {
        double distRival2 = std::sqrt(std::pow(px - rivalJ2.x(), 2) +
                                      std::pow(py - (rivalJ2.y() + 20), 2));
        if (distRival2 <= radio + 20) {
            p.desactivar();
            info.ocurrioColision = true;
            info.tipoColision = "rival";
            info.indiceObjetoImpactado = 2;
            return info;
        }
    }

    // Si es turno del Jugador 2, solo puede impactar al Rival 1
    if (jugadorAtacante == 2) {
        double distRival1 = std::sqrt(std::pow(px - rivalJ1.x(), 2) +
                                      std::pow(py - (rivalJ1.y() + 20), 2));
        if (distRival1 <= radio + 20) {
            p.desactivar();
            info.ocurrioColision = true;
            info.tipoColision = "rival";
            info.indiceObjetoImpactado = 1;
            return info;
        }
    }

    return info;
}
