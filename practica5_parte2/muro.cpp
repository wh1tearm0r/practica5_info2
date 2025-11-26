#include "Muro.h"

Muro::Muro(double x, double y, double ancho, double alto,
           double resistenciaInicial, int propietario,
           double coefRestitucion)
    : Obstaculo(x, y, ancho, alto, coefRestitucion),
    resistencia(resistenciaInicial),
    resistenciaMaxima(resistenciaInicial),
    idJugadorPropietario(propietario)
{
}

void Muro::recibirDanio(double danio) {
    resistencia -= danio;

    if (resistencia < 0.0) {
        resistencia = 0.0;
    }
}

double Muro::getPorcentajeVida() const {
    if (resistenciaMaxima <= 0.0) return 0.0;

    double porcentaje = resistencia / resistenciaMaxima;

    if (porcentaje < 0.0) return 0.0;
    if (porcentaje > 1.0) return 1.0;

    return porcentaje;
}

void Muro::restaurar() {
    resistencia = resistenciaMaxima;
}
