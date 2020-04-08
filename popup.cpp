#include "popup.h"
#include "ui_popup.h"

Popup::Popup(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint| Qt::Dialog| Qt::WindowStaysOnTopHint )
    , ui(new Ui::Popup)
{
    ui->setupUi(this);
    move(settings.value("popup/position", QPoint(50,50)).value<QPoint>());
    createTrayIcon();
    connect(ui->pushOpen, &QPushButton::clicked, this, &Popup::openBrowser);
}

Popup::~Popup()
{
    delete ui;
}


void Popup::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_OSX
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    hide();
    event->ignore();
}

void Popup::openBrowser(){
    bool ok = QDesktopServices::openUrl ( current_link );
    if (!ok) {
        qDebug() << "failed open url";
    }
}

void Popup::open(QJsonObject jobject)
{
    if (jobject.value("action") == "invite") {
        current_link = jobject["link"].toString();
        QString room = jobject["link"].toString().split('/', QString::SkipEmptyParts).takeLast();
        ui->labelInvite->setText(jobject["name"].toString() + tr(" приглашает в конференцию ") + room);
        show();
        raise();
    }
}


void Popup::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Popup::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
        settings.setValue("popup/position", pos());
    }
}

void Popup::showInvite()
{
    bool ok;
    QString text;
    QJsonObject jobject;
    text = QInputDialog::getText(
                this,
                tr("Приглашение"),
                tr("Ссылка на конференцию:              "),
                QLineEdit::Normal,
                settings.value("user/link", "https://meet.ivjh.ru:8555/FlyingRabbits").toString(),
                &ok
                );
    if (ok) {
        settings.setValue("user/link", text);
        jobject["link"] = text;
        jobject["name"] = settings.value("user/name", tr("Пользователь")).toString();
        emit sendInvite(jobject);
    }
}

void Popup::showSettings()
{
    bool ok;
    QString text;
    text = QInputDialog::getText(
                this,
                tr("Настройки"),
                ("Сервер уведомлений:"),
                QLineEdit::Normal,
                settings.value("transport/server", "popups.eri.su").toString(),
                &ok
                );
    if (ok) {
        settings.setValue("transport/server", text);
        emit changedServer(text);
    };
    text = QInputDialog::getText(
                nullptr,
                tr("Настройки"),
                tr("Ваше имя:"),
                QLineEdit::Normal,
                settings.value("user/name", tr("Пользователь")).toString(),
                &ok
                );
    if (ok) settings.setValue("user/name", text);
}

void Popup::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);

    QAction *inviteAction = new QAction(tr("Пригласить"), this);
    connect(inviteAction, &QAction::triggered, this, &Popup::showInvite);

    QAction *settingsAction = new QAction(tr("Настройки"), this);
    connect(settingsAction, &QAction::triggered, this, &Popup::showSettings);

    QAction *quitAction = new QAction(tr("Выход"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    QMenu *trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(inviteAction);
    trayIconMenu->addAction(settingsAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icons/icon.icns"));
    trayIcon->show();
}

