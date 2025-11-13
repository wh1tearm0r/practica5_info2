#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Configurar estilo de la aplicacion
    app.setStyle("Fusion");

    // Crear y mostrar ventana principal
    MainWindow ventana;
    ventana.show();

    return app.exec();
}
