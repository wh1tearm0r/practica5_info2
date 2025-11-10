#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QGraphicsScene * scene = new QGraphicsScene();
    QGraphicsEllipseItem * particula = new QGraphicsEllipseItem();
    particula->setRect(0, 0, 50, 50);
    scene->addItem(particula);
    QGraphicsView * view = new QGraphicsView(scene);
    view->show();

    return a.exec();
}
