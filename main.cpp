#include "popup.h"
#include "transport.h"

#include <QApplication>
#include <QObject>

#ifdef Q_OS_WIN32
#include <QDir>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("ivjh");
    QCoreApplication::setOrganizationDomain("eri.su");
    QCoreApplication::setApplicationName("Popups");
    QApplication::setQuitOnLastWindowClosed(false);
    a.setWindowIcon(QIcon(":/icons/icon.ico"));

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }

    #ifdef Q_OS_WIN32
        QSettings autorun("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        autorun.setValue("meetagent", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        autorun.sync();
    #endif

    QSettings settings;
    Popup w;
    Transport t;

    QString server = settings.value("transport/server", "popups.eri.su").toString();
    qDebug() << "server" << server;
    t.setServer(server);
    QObject::connect(&t, &Transport::onMessage, &w, &Popup::open);
    QObject::connect(&w, &Popup::sendInvite, &t, &Transport::sendInvite);
  //  QObject::connect(&w, &Popup::changedServer, &t, &Transport::setServer);

//    w.show();

    return a.exec();
}
