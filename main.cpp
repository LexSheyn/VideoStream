#include <QApplication>

#include <QMainWindow>

#include "ServerPlayer.h"
#include "ClientPlayer.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    ServerPlayer* server = new ServerPlayer();
    ClientPlayer* client = new ClientPlayer();

    server->start(QUrl::fromLocalFile("/home/user/Videos/videoplayback.mp4"));
    client->start();

    QMainWindow* mainWindow = new QMainWindow();

    mainWindow->setCentralWidget(client);

    mainWindow->showMaximized();

    return application.exec();
}
