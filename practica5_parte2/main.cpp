#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Configurar estilo
    app.setStyle("Fusion");

    // Crear ventana principal
    MainWindow ventana;
    ventana.setWindowTitle("Juego de Estrategia Militar - Practica 5");
    ventana.setFixedSize(1300, 700);
    ventana.show();

    return app.exec();
}
