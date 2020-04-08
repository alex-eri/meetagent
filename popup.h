#ifndef POPUP_H
#define POPUP_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QJsonObject>
#include <QSettings>
#include <QAction>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QInputDialog>
#include <QHostAddress>
#include <QMessageBox>
#include <QDesktopServices>

QT_BEGIN_NAMESPACE
namespace Ui { class Popup; }
QT_END_NAMESPACE

class Popup : public QMainWindow
{
    Q_OBJECT

public:
    Popup(QWidget *parent = nullptr);
    ~Popup();
    void open(QJsonObject jobject);
    void showSettings();
    void showInvite();
public slots:
    void openBrowser();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

signals:
    void changedServer(QString address);
    void sendInvite(QJsonObject jobject);

private:
    QUrl current_link;
    Ui::Popup *ui;
    QPoint dragPosition;
    QSettings settings;
    QSystemTrayIcon *trayIcon;
    void createTrayIcon();
};
#endif // POPUP_H
