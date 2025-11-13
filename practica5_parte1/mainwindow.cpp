#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <fstream>
#include <QPen>
#include <QBrush>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , simulador(nullptr)
    , scene(nullptr)
    , view(nullptr)
    , itemCaja(nullptr)
    , timer(nullptr)
    , textoEstadisticas(nullptr)
    , simulacionActiva(false)
    , pasoActual(0)
    , btnIniciar(nullptr)
    , btnPausar(nullptr)
    , btnReset(nullptr)
    , btnExportar(nullptr)
    , labelUltimaColision(nullptr)
    , textHistorialColisiones(nullptr)
    , ultimaColisionMostrada(0)
{
    ui->setupUi(this);

    // Configurar ventana
    setWindowTitle("Simulador de Colisiones - Práctica 5 - Informática II");
    resize(1000, 750);

    // Configurar interfaz
    configurarInterfaz();

    // Configurar escena inicial
    configurarEscena();
}

MainWindow::~MainWindow()
{
    // Detener timer primero
    if (timer) {
        timer->stop();
        delete timer;
        timer = nullptr;
    }

    // Limpiar escena
    limpiarEscena();

    // Eliminar scene
    if (scene) {
        delete scene;
        scene = nullptr;
    }

    // Eliminar simulador
    if (simulador) {
        delete simulador;
        simulador = nullptr;
    }

    // UI se elimina automaticamente
    delete ui;
}

void MainWindow::configurarInterfaz()
{
    // Crear widget central
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Layout principal
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(centralWidget);
    layoutPrincipal->setContentsMargins(10, 10, 10, 10);

    // Crear scene y view
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800, 600);
    scene->setBackgroundBrush(QBrush(QColor(240, 240, 240)));

    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMinimumSize(820, 620);
    view->setMaximumSize(820, 620);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layoutPrincipal->addWidget(view);

    // Panel de control
    QHBoxLayout *layoutControles = new QHBoxLayout();

    btnIniciar = new QPushButton(" Iniciar", this);
    btnPausar = new QPushButton(" Pausar", this);
    btnReset = new QPushButton(" Resetear", this);
    btnExportar = new QPushButton(" Exportar", this);

    btnIniciar->setMinimumHeight(40);
    btnPausar->setMinimumHeight(40);
    btnReset->setMinimumHeight(40);
    btnExportar->setMinimumHeight(40);

    // Estilo de botones
    QString estiloBase = "QPushButton { font-size: 14px; font-weight: bold; padding: 8px; border-radius: 5px; border: 2px solid; }";
    btnIniciar->setStyleSheet(estiloBase + "QPushButton { background-color: #4CAF50; color: white; border-color: #45a049; }");
    btnPausar->setStyleSheet(estiloBase + "QPushButton { background-color: #FF9800; color: white; border-color: #e68900; }");
    btnReset->setStyleSheet(estiloBase + "QPushButton { background-color: #2196F3; color: white; border-color: #0b7dda; }");
    btnExportar->setStyleSheet(estiloBase + "QPushButton { background-color: #9C27B0; color: white; border-color: #7b1fa2; }");

    layoutControles->addWidget(btnIniciar);
    layoutControles->addWidget(btnPausar);
    layoutControles->addWidget(btnReset);
    layoutControles->addWidget(btnExportar);
    layoutControles->addSpacing(20);

    // Separador visual
    QFrame *separador = new QFrame(this);
    separador->setFrameShape(QFrame::VLine);
    separador->setFrameShadow(QFrame::Sunken);
    layoutControles->addWidget(separador);

    layoutControles->addSpacing(20);

    // Label de escenarios
    QLabel *labelEscenarios = new QLabel("ESCENARIOS:", this);
    labelEscenarios->setStyleSheet("font-weight: bold; font-size: 12px;");
    layoutControles->addWidget(labelEscenarios);

    // Botones de escenarios
    QPushButton *btnEsc1 = new QPushButton("Paredes", this);
    QPushButton *btnEsc2 = new QPushButton("Obstaculos", this);
    QPushButton *btnEsc3 = new QPushButton("Fusion", this);
    QPushButton *btnEsc4 = new QPushButton("Completo", this);

    QString estiloEscenario = "QPushButton { font-size: 11px; padding: 5px; border-radius: 3px; background-color: #E0E0E0; }";
    btnEsc1->setStyleSheet(estiloEscenario);
    btnEsc2->setStyleSheet(estiloEscenario);
    btnEsc3->setStyleSheet(estiloEscenario);
    btnEsc4->setStyleSheet(estiloEscenario);

    layoutControles->addWidget(btnEsc1);
    layoutControles->addWidget(btnEsc2);
    layoutControles->addWidget(btnEsc3);
    layoutControles->addWidget(btnEsc4);

    // Conectar escenarios
    connect(btnEsc1, &QPushButton::clicked, [this](){ cargarEscenario(1); });
    connect(btnEsc2, &QPushButton::clicked, [this](){ cargarEscenario(2); });
    connect(btnEsc3, &QPushButton::clicked, [this](){ cargarEscenario(3); });
    connect(btnEsc4, &QPushButton::clicked, [this](){ cargarEscenario(4); });
    layoutControles->addStretch();

    layoutPrincipal->addLayout(layoutControles);

    // Panel de información de colisiones
    QHBoxLayout *layoutInfo = new QHBoxLayout();

    // Ultima colision
    QVBoxLayout *layoutUltima = new QVBoxLayout();
    QLabel *tituloUltima = new QLabel(" ULTIMA COLISION:", this);
    tituloUltima->setStyleSheet("font-weight: bold; font-size: 12px;");
    labelUltimaColision = new QLabel("Esperando colision...", this);
    labelUltimaColision->setStyleSheet("background-color: #f0f0f0; padding: 10px; border-radius: 5px; font-size: 11px;");
    labelUltimaColision->setWordWrap(true);
    labelUltimaColision->setMinimumHeight(60);
    layoutUltima->addWidget(tituloUltima);
    layoutUltima->addWidget(labelUltimaColision);

    // Historial de colisiones
    QVBoxLayout *layoutHistorial = new QVBoxLayout();
    QLabel *tituloHistorial = new QLabel(" HISTORIAL DE COLISIONES:", this);
    tituloHistorial->setStyleSheet("font-weight: bold; font-size: 12px;");
    textHistorialColisiones = new QTextEdit(this);
    textHistorialColisiones->setReadOnly(true);
    textHistorialColisiones->setMaximumHeight(100);
    textHistorialColisiones->setStyleSheet("background-color: white; font-size: 10px; font-family: 'Courier New';");
    layoutHistorial->addWidget(tituloHistorial);
    layoutHistorial->addWidget(textHistorialColisiones);

    layoutInfo->addLayout(layoutUltima, 1);
    layoutInfo->addLayout(layoutHistorial, 2);

    layoutPrincipal->addLayout(layoutInfo);

    // Conectar seniales
    connect(btnIniciar, &QPushButton::clicked, this, &MainWindow::iniciarSimulacion);
    connect(btnPausar, &QPushButton::clicked, this, &MainWindow::pausarSimulacion);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::resetearSimulacion);
    connect(btnExportar, &QPushButton::clicked, this, &MainWindow::exportarDatos);

    // Timer para animacion
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarSimulacion);
}

void MainWindow::configurarEscena()
{
    if (!scene) return;

    // Limpiar primero
    limpiarEscena();

    // Crear simulador
    if (simulador) {
        delete simulador;
    }
    simulador = new Simulador(800, 600, 0.01);

    // Dibujar limite de la caja
    itemCaja = scene->addRect(0, 0, 800, 600, QPen(Qt::black, 3), Qt::NoBrush);

    // Agregar 4 obstaculos
    simulador->agregarObstaculo(200, 150, 80, 80, 0.7);
    simulador->agregarObstaculo(520, 150, 80, 80, 0.7);
    simulador->agregarObstaculo(200, 370, 80, 80, 0.7);
    simulador->agregarObstaculo(520, 370, 80, 80, 0.7);

    // Agregar 4 particulas
    simulador->agregarParticula(100, 100, 150, 120, 1.0);
    simulador->agregarParticula(700, 100, -180, 140, 1.5);
    simulador->agregarParticula(100, 500, 160, -130, 1.2);
    simulador->agregarParticula(700, 500, -170, -110, 0.8);

    // Dibujar elementos
    dibujarObstaculos();
    crearParticulas();

    // Texto de estadisticas
    textoEstadisticas = scene->addText("");
    QFont fuente("Arial", 10, QFont::Bold);
    textoEstadisticas->setFont(fuente);
    textoEstadisticas->setPos(10, 10);
    textoEstadisticas->setDefaultTextColor(Qt::black);
    textoEstadisticas->setZValue(100); // Encima de todo

    actualizarEstadisticas();
    pasoActual = 0;
}

void MainWindow::dibujarObstaculos()
{
    if (!scene || !simulador) return;

    // Limpiar obstaculos anteriores
    for (auto item : itemsObstaculos) {
        if (item) {
            scene->removeItem(item);
            delete item;
        }
    }
    itemsObstaculos.clear();

    // Dibujar nuevos obstaculos
    QColor colorObstaculo(100, 100, 100, 180);
    QPen penObstaculo(Qt::black, 2);

    const auto& obstaculos = simulador->getObstaculos();
    for (size_t i = 0; i < obstaculos.size(); i++) {
        const auto& obs = obstaculos[i];
        if (obs) {
            QGraphicsRectItem *rect = scene->addRect(
                obs->getX(),
                obs->getY(),
                obs->getAncho(),
                obs->getAlto(),
                penObstaculo,
                QBrush(colorObstaculo)
                );
            itemsObstaculos.push_back(rect);
        }
    }
}

void MainWindow::crearParticulas()
{
    if (!scene || !simulador) return;

    // Limpiar particulas anteriores
    for (auto item : itemsParticulas) {
        if (item) {
            scene->removeItem(item);
            delete item;
        }
    }
    itemsParticulas.clear();

    // Colores para cada particula
    QList<QColor> colores = {
        QColor(255, 50, 50),   // Rojo
        QColor(50, 50, 255),   // Azul
        QColor(50, 255, 50),   // Verde
        QColor(255, 165, 0)    // Naranja
    };

    // Crear circulos para cada particula
    const auto& particulas = simulador->getParticulas();
    for (size_t i = 0; i < particulas.size(); i++) {
        QColor color = colores[i % colores.size()];
        QPen pen(Qt::black, 2);

        QGraphicsEllipseItem *item = scene->addEllipse(
            0, 0, 10, 10,  // Valores iniciales (se actualizaran)
            pen,
            QBrush(color)
            );

        itemsParticulas.push_back(item);
    }

    // Actualizar posiciones iniciales
    actualizarVisualizacion();
}

void MainWindow::actualizarVisualizacion()
{
    if (!simulador) return;

    const auto& particulas = simulador->getParticulas();

    for (size_t i = 0; i < particulas.size() && i < itemsParticulas.size(); i++) {
        if (particulas[i] && itemsParticulas[i]) {
            if (particulas[i]->estaActiva()) {
                double x = particulas[i]->getX();
                double y = particulas[i]->getY();
                double radio = particulas[i]->getRadio();

                // Actualizar posicion y tamanio
                itemsParticulas[i]->setRect(
                    x - radio,
                    y - radio,
                    radio * 2,
                    radio * 2
                    );
                itemsParticulas[i]->setVisible(true);
            } else {
                // Ocultar particulas inactivas
                itemsParticulas[i]->setVisible(false);
            }
        }
    }
}

void MainWindow::actualizarEstadisticas()
{
    if (!textoEstadisticas || !simulador) return;

    QString stats = QString(
                        " Tiempo: %1 s |  Activas: %2/%3 |  Colisiones: %4 |  Pasos: %5"
                        ).arg(simulador->getTiempoActual(), 0, 'f', 2)
                        .arg(simulador->getNumParticulasActivas())
                        .arg(simulador->getParticulas().size())
                        .arg(simulador->getEventosColision().size())
                        .arg(pasoActual);

    textoEstadisticas->setPlainText(stats);
}

void MainWindow::iniciarSimulacion()
{
    if (!simulacionActiva && timer) {
        simulacionActiva = true;
        timer->start(16);  // ~60 FPS
        if (ui && ui->statusbar) {
            ui->statusbar->showMessage(" Simulacion en ejecucion...", 2000);
        }
    }
}

void MainWindow::pausarSimulacion()
{
    if (simulacionActiva && timer) {
        simulacionActiva = false;
        timer->stop();
        if (ui && ui->statusbar) {
            ui->statusbar->showMessage(" Simulacion pausada", 2000);
        }
    }
}

void MainWindow::resetearSimulacion()
{
    pausarSimulacion();

    ultimaColisionMostrada = 0;
    if (labelUltimaColision) {
        labelUltimaColision->setText("Esperando colisión...");
    }
    if (textHistorialColisiones) {
        textHistorialColisiones->clear();
    }

    configurarEscena();
    if (ui && ui->statusbar) {
        ui->statusbar->showMessage(" Simulacion reseteada", 2000);
    }
}

void MainWindow::actualizarSimulacion()
{
    if (!simulador) return;

    // Ejecutar paso
    simulador->simularPaso();
    pasoActual++;

    // Actualizar visualizacion
    actualizarVisualizacion();
    actualizarEstadisticas();
    mostrarNuevasColisiones();
}

void MainWindow::exportarDatos()
{
    if (!simulador) {
        QMessageBox::warning(this, "Error", "No hay simulacion para exportar");
        return;
    }

    QString nombreArchivo = QFileDialog::getSaveFileName(
        this,
        "Guardar resultados",
        "simulacion.txt",
        "Archivos de texto (*.txt)"
        );

    if (nombreArchivo.isEmpty()) {
        return;
    }

    // Exportar
    simulador->exportarSimulacion(nombreArchivo.toStdString());

    QMessageBox::information(
        this,
        " Exportacion exitosa",
        "Datos guardados en:\n" + nombreArchivo
        );

    if (ui && ui->statusbar) {
        ui->statusbar->showMessage(" Datos exportados", 3000);
    }
}

void MainWindow::mostrarNuevasColisiones()
{
    if (!simulador) return;

    const auto& colisiones = simulador->getEventosColision();

    // Si hay nuevas colisiones
    if (colisiones.size() > static_cast<size_t>(ultimaColisionMostrada)) {
        for (size_t i = static_cast<size_t>(ultimaColisionMostrada); i < colisiones.size(); i++) {
            const auto& col = colisiones[i];

            // Determinar emoji y color segun tipo
            QString emoji;
            QString color;
            QString nombreTipo;

            // Convertir std::string a QString una sola vez
            QString tipoStr = QString::fromStdString(col.tipo);
            QString detallesStr = QString::fromStdString(col.detalles);

            if (tipoStr.contains("pared")) {
                emoji = "🟢";
                color = "#4CAF50";
                nombreTipo = "PARED (Elastica)";
            } else if (tipoStr.contains("obstaculo")) {
                emoji = "🟠";
                color = "#FF9800";
                nombreTipo = "OBSTACULO (Inelastica)";
            } else if (tipoStr.contains("fusion")) {
                emoji = "🔴";
                color = "#F44336";
                nombreTipo = "FUSION (Completamente Inelastica)";
            } else {
                emoji = "⚪";
                color = "#9E9E9E";
                nombreTipo = "Otra";
            }

            // Actualizar ultima colisión
            QString textoUltima = QString(
                                      "<b style='color: %1;'>%2 %3</b><br>"
                                      "<small>t = %4 s - %5</small>"
                                      ).arg(color)
                                      .arg(emoji)
                                      .arg(nombreTipo)  // ← SIN fromStdString()
                                      .arg(col.tiempo, 0, 'f', 2)
                                      .arg(detallesStr);  // ← Ya convertido arriba

            if (labelUltimaColision) {
                labelUltimaColision->setText(textoUltima);
            }

            // Agregar al historial
            QString lineaHistorial = QString("[%1s] %2 %3")
                                         .arg(col.tiempo, 6, 'f', 2)
                                         .arg(emoji)
                                         .arg(tipoStr);

            if (textHistorialColisiones) {
                textHistorialColisiones->append(lineaHistorial);

                // Auto-scroll al final
                QScrollBar *scrollBar = textHistorialColisiones->verticalScrollBar();
                if (scrollBar) {
                    scrollBar->setValue(scrollBar->maximum());
                }
            }
        }

        ultimaColisionMostrada = static_cast<int>(colisiones.size());
    }
}

void MainWindow::cargarEscenario(int numero)
{
    pausarSimulacion();

    if (!simulador) return;

    limpiarEscena();

    // Recrear simulador
    delete simulador;
    simulador = new Simulador(800, 600, 0.01);

    // Dibujar limite
    itemCaja = scene->addRect(0, 0, 800, 600, QPen(Qt::black, 3), Qt::NoBrush);

    QString mensaje;

    switch(numero) {
    case 1: // ESCENARIO 1: Prueba de PAREDES (colisiones elasticas)
        mensaje = " ESCENARIO 1: Prueba de colisiones con PAREDES (elasticas)";
        // UNA partícula rebotando en las paredes
        simulador->agregarParticula(400, 300, 200, 150, 1.0);
        break;

    case 2: // ESCENARIO 2: Prueba de OBSTACULOS (colisiones inelasticas)
        mensaje = " ESCENARIO 2: Prueba de colisiones con OBSTACULOS (inelasticas, coef=0.7)";
        // Agregar obstaculos en el centro
        simulador->agregarObstaculo(300, 250, 200, 100, 0.7);
        // Particulas apuntando al obstaculo
        simulador->agregarParticula(100, 300, 150, 0, 1.0);
        simulador->agregarParticula(700, 300, -150, 0, 1.0);
        break;

    case 3: // ESCENARIO 3: Prueba de FUSION entre particulas
        mensaje = " ESCENARIO 3: Prueba de FUSION entre particulas (completamente inelastica)";
        // Dos particulas en curso de colision frontal
        simulador->agregarParticula(200, 300, 100, 0, 1.0);
        simulador->agregarParticula(600, 300, -100, 0, 1.0);
        // Dos mas en diagonal
        simulador->agregarParticula(300, 150, 50, 100, 0.8);
        simulador->agregarParticula(500, 450, -50, -100, 0.8);
        break;

    case 4: // ESCENARIO 4: COMPLETO - Todos los tipos
        mensaje = " ESCENARIO 4: Configuracion COMPLETA (todos los tipos de colision)";
        // Obstaculos
        simulador->agregarObstaculo(200, 150, 80, 80, 0.7);
        simulador->agregarObstaculo(520, 150, 80, 80, 0.7);
        simulador->agregarObstaculo(200, 370, 80, 80, 0.7);
        simulador->agregarObstaculo(520, 370, 80, 80, 0.7);
        // Particulas
        simulador->agregarParticula(100, 100, 150, 120, 1.0);
        simulador->agregarParticula(700, 100, -180, 140, 1.5);
        simulador->agregarParticula(100, 500, 160, -130, 1.2);
        simulador->agregarParticula(700, 500, -170, -110, 0.8);
        break;
    }

    // Redibujar
    dibujarObstaculos();
    crearParticulas();

    // Resetear estadisticas
    if (textoEstadisticas) {
        scene->removeItem(textoEstadisticas);
        delete textoEstadisticas;
    }
    textoEstadisticas = scene->addText("");
    QFont fuente("Arial", 10, QFont::Bold);
    textoEstadisticas->setFont(fuente);
    textoEstadisticas->setPos(10, 10);
    textoEstadisticas->setDefaultTextColor(Qt::black);
    textoEstadisticas->setZValue(100);

    actualizarEstadisticas();
    pasoActual = 0;
    ultimaColisionMostrada = 0;

    if (labelUltimaColision) {
        labelUltimaColision->setText("Esperando colisión...");
    }
    if (textHistorialColisiones) {
        textHistorialColisiones->clear();
    }

    if (ui && ui->statusbar) {
        ui->statusbar->showMessage(mensaje, 5000);
    }
}

void MainWindow::limpiarEscena()
{
    if (!scene) return;

    // Limpiar partículas
    for (auto item : itemsParticulas) {
        if (item) {
            scene->removeItem(item);
            delete item;
        }
    }
    itemsParticulas.clear();

    // Limpiar obstaculos
    for (auto item : itemsObstaculos) {
        if (item) {
            scene->removeItem(item);
            delete item;
        }
    }
    itemsObstaculos.clear();

    // Limpiar caja
    if (itemCaja) {
        scene->removeItem(itemCaja);
        delete itemCaja;
        itemCaja = nullptr;
    }

    // Limpiar texto
    if (textoEstadisticas) {
        scene->removeItem(textoEstadisticas);
        delete textoEstadisticas;
        textoEstadisticas = nullptr;
    }
}
