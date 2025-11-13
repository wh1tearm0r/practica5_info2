#include "Simulador.h"
#include <iostream>
#include <sstream>
#include <fstream>

Simulador::Simulador(double ancho, double alto, double deltaTiempo)
    : anchoBox(ancho), altoBox(alto), tiempoActual(0.0), dt(deltaTiempo), contadorParticulas(0) {
}
Simulador::~Simulador() {
    for (auto p : particulas) {
        delete p;
    }
    particulas.clear();
    for (auto o : obstaculos) {
        delete o;
    }
    obstaculos.clear();
}
void Simulador::agregarParticula(double x, double y, double vx, double vy, double masa) {
    Particula* nueva = new Particula(x, y, vx, vy, masa, contadorParticulas++);
    particulas.push_back(nueva);
}
void Simulador::agregarObstaculo(double x, double y, double ancho, double alto, double coefRestitucion) {
    Obstaculo* nuevo = new Obstaculo(x, y, ancho, alto, coefRestitucion);
    obstaculos.push_back(nuevo);
}
void Simulador::simularPaso() {
    for (auto p : particulas) {
        if (p->estaActiva()) {
            p->actualizarPosicion(dt);
        }
    }
    detectarColisionesParedes();
    detectarColisionesObstaculos();
    detectarColisionesParticulas();
    tiempoActual += dt;
}

void Simulador::detectarColisionesParedes() {
    for (auto p : particulas) {
        if (!p->estaActiva()) continue;

        double x = p->getX();
        double y = p->getY();
        double r = p->getRadio();
        bool colisionDetectada = false;

        if (x - r <= 0) {
            p->setX(r + 0.5);
            p->rebotarParedVertical();
            colisionDetectada = true;

            EventoColision evento;
            evento.tiempo = tiempoActual;
            evento.tipo = "pared_izquierda";
            evento.id1 = p->getId();
            evento.id2 = -1;
            std::ostringstream oss;
            oss << "Particula " << p->getId() << " reboto en pared izquierda";
            evento.detalles = oss.str();
            eventosColision.push_back(evento);
        }

        if (x + r >= anchoBox) {
            p->setX(anchoBox - r - 0.5);
            p->rebotarParedVertical();
            colisionDetectada = true;

            EventoColision evento;
            evento.tiempo = tiempoActual;
            evento.tipo = "pared_derecha";
            evento.id1 = p->getId();
            evento.id2 = -1;
            std::ostringstream oss;
            oss << "Particula " << p->getId() << " reboto en pared derecha";
            evento.detalles = oss.str();
            eventosColision.push_back(evento);
        }

        // Pared superior
        if (y - r <= 0) {
            p->setY(r + 0.5);
            p->rebotarParedHorizontal();
            colisionDetectada = true;

            EventoColision evento;
            evento.tiempo = tiempoActual;
            evento.tipo = "pared_superior";
            evento.id1 = p->getId();
            evento.id2 = -1;
            std::ostringstream oss;
            oss << "Particula " << p->getId() << " reboto en pared superior";
            evento.detalles = oss.str();
            eventosColision.push_back(evento);
        }

        // Pared inferior
        if (y + r >= altoBox) {
            p->setY(altoBox - r - 0.5);
            p->rebotarParedHorizontal();
            colisionDetectada = true;

            EventoColision evento;
            evento.tiempo = tiempoActual;
            evento.tipo = "pared_inferior";
            evento.id1 = p->getId();
            evento.id2 = -1;
            std::ostringstream oss;
            oss << "Particula " << p->getId() << " reboto en pared inferior";
            evento.detalles = oss.str();
            eventosColision.push_back(evento);
        }
    }
}
void Simulador::detectarColisionesParticulas() {
    for (size_t i = 0; i < particulas.size(); i++) {
        if (!particulas[i]->estaActiva()) continue;

        for (size_t j = i + 1; j < particulas.size(); j++) {
            if (!particulas[j]->estaActiva()) continue;

            if (particulas[i]->colisionaCon(*particulas[j])) {
                EventoColision evento;
                evento.tiempo = tiempoActual;
                evento.tipo = "fusion_particulas";
                evento.id1 = particulas[i]->getId();
                evento.id2 = particulas[j]->getId();
                std::ostringstream oss;
                oss << "Particulas " << particulas[i]->getId()
                    << " y " << particulas[j]->getId()
                    << " se fusionaron (masa final: "
                    << particulas[i]->getMasa() + particulas[j]->getMasa() << ")";
                evento.detalles = oss.str();
                eventosColision.push_back(evento);
                particulas[i]->fusionarCon(*particulas[j]);
            }
        }
    }
}
void Simulador::detectarColisionesObstaculos() {
    for (auto p : particulas) {
        if (!p->estaActiva()) continue;

        for (size_t i = 0; i < obstaculos.size(); i++) {
            if (obstaculos[i]->aplicarRebote(*p)) {
                // Registrar evento
                EventoColision evento;
                evento.tiempo = tiempoActual;
                evento.tipo = "obstaculo";
                evento.id1 = p->getId();
                evento.id2 = i; // Usar índice del obstáculo
                std::ostringstream oss;
                oss << "Particula " << p->getId()
                    << " rebotó en obstáculo " << i
                    << " (coef. restitución: " << obstaculos[i]->getCoefRestitucion() << ")";
                evento.detalles = oss.str();
                eventosColision.push_back(evento);
            }
        }
    }
}
// Contar partículas activas
int Simulador::getNumParticulasActivas() const {
    int count = 0;
    for (auto p : particulas) {
        if (p->estaActiva()) count++;
    }
    return count;
}

// Reiniciar simulación
void Simulador::reset() {
    for (auto p : particulas) {
        delete p;
    }
    particulas.clear();
    for (auto o : obstaculos) {
        delete o;
    }
    obstaculos.clear();
    eventosColision.clear();
    tiempoActual = 0.0;
    contadorParticulas = 0;
}

void Simulador::exportarSimulacion(const std::string& nombreArchivo) const {
    std::ofstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << std::endl;
        return;
    }

    // Informaciin general
    archivo << "# SIMULACION DE COLISIONES MULTIPLES\n";
    archivo << "# Practica 5 - Informatica II\n";
    archivo << "#\n";
    archivo << "# Box: " << anchoBox << " x " << altoBox << "\n";
    archivo << "# Tiempo: " << tiempoActual << " s\n";
    archivo << "# Particulas activas: " << getNumParticulasActivas() << "/" << particulas.size() << "\n";
    archivo << "# Total colisiones: " << eventosColision.size() << "\n";
    archivo << "#\n";

    // Obstáculos
    archivo << "# OBSTACULOS\n";
    for (size_t i = 0; i < obstaculos.size(); i++) {
        archivo << "obstaculo," << i << ","
                << obstaculos[i]->getX() << ","
                << obstaculos[i]->getY() << ","
                << obstaculos[i]->getAncho() << ","
                << obstaculos[i]->getAlto() << "\n";
    }
    archivo << "#\n";

    // Partículas
    archivo << "# PARTICULAS\n";
    for (const auto& p : particulas) {
        archivo << "particula," << p->getId() << ","
                << p->getX() << "," << p->getY() << ","
                << p->getMasa() << "," << p->getRadio() << ","
                << (p->estaActiva() ? 1 : 0) << "\n";
    }
    archivo << "#\n";

    // Colisiones
    archivo << "# COLISIONES\n";
    for (const auto& evento : eventosColision) {
        archivo << "colision," << evento.tiempo << ","
                << evento.tipo << "," << evento.id1 << ","
                << evento.id2 << "\n";
    }

    archivo.close();
    std::cout << " Datos exportados a: " << nombreArchivo << std::endl;
}
