#ifndef SIMULADOR_H
#define SIMULADOR_H

#include "Particula.h"
#include "obstaculo.h"
#include <vector>
#include <string>

struct EventoColision {
    double tiempo;
    std::string tipo;
    int id1;
    int id2;
    std::string detalles;
};

class Simulador {
private:
    std::vector<Particula*> particulas;
    std::vector<Obstaculo*> obstaculos;
    std::vector<EventoColision> eventosColision;

    double anchoBox;
    double altoBox;
    double tiempoActual;
    double dt;

    int contadorParticulas;

public:
    Simulador(double ancho, double alto, double deltaTiempo);
    ~Simulador();

    void agregarParticula(double x, double y, double vx, double vy, double masa);
    void agregarObstaculo(double x, double y, double ancho, double alto, double coefRestitucion = 0.7);
    void simularPaso();
    void detectarColisionesParedes();
    void detectarColisionesParticulas();
    void detectarColisionesObstaculos();
    const std::vector<Particula*>& getParticulas() const { return particulas; }
    const std::vector<Obstaculo*>& getObstaculos() const { return obstaculos; }
    const std::vector<EventoColision>& getEventosColision() const { return eventosColision; }
    double getTiempoActual() const { return tiempoActual; }
    double getAnchoBox() const { return anchoBox; }
    double getAltoBox() const { return altoBox; }
    int getNumParticulasActivas() const;
    void reset();
    void exportarSimulacion(const std::string& nombreArchivo) const;
};

#endif // SIMULADOR_H
