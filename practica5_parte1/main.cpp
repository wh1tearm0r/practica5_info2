#include "Simulador.h"
#include <iostream>

int main() {
    std::cout << "=== PRUEBA ===" << std::endl;

    Simulador sim(800, 600, 0.01);
    sim.agregarParticula(100, 100, 150, 120, 1.0);
    sim.agregarParticula(700, 100, -180, 140, 1.5);

    for(int i = 0; i < 100; i++) {
        sim.simularPaso();
    }

    std::cout << "Particulas activas: " << sim.getNumParticulasActivas() << std::endl;
    std::cout << "Colisiones: " << sim.getEventosColision().size() << std::endl;
    std::cout << "Prueba exitosa!" << std::endl;

    return 0;
}
