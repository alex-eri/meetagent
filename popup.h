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
#include <QStringListModel>


QT_BEGIN_NAMESPACE
namespace Ui {
    class Popup;
    class Settings;
}
QT_END_NAMESPACE

class Settings : public QDialog
{
    Q_OBJECT
public:
    Settings(QWidget *parent = nullptr);
    ~Settings();

    //QStringListModel *groups = new QStringListModel();
    QStringList groups;
    QStringListModel groupsModel;
protected:
    void showEvent(QShowEvent * event) override;

public slots:
    void appendGroup();
    void saveSettings();
    void load();
signals:
    void changedServer(QString server);
private:
    Ui::Settings *ui;
    QSettings settings;


};


class Popup : public QWidget
{
    Q_OBJECT

public:
    Popup(QWidget *parent = nullptr);
    ~Popup();
    void open(QJsonObject jobject);
    void showInvite();
    Settings *settingsDialog;

public slots:
    void openBrowser();

protected:
//    void mouseMoveEvent(QMouseEvent *event) override;
//    void mousePressEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void sendInvite(QJsonObject jobject);

private:
    QUrl current_link;
    Ui::Popup *ui;
    QPoint dragPosition;
    QSettings settings;
    QSystemTrayIcon *trayIcon;
    void createTrayIcon();
private slots:
    void clickedTrayIcon(QSystemTrayIcon::ActivationReason reason);
};




#endif // POPUP_H
