#include "popup.h"
#include "ui_popup.h"
#include "ui_settings.h"


Popup::Popup(QWidget *parent)
    : QWidget(parent,  Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint )
    , settingsDialog(new Settings)
    , ui(new Ui::Popup)
{
    ui->setupUi(this);
    move(settings.value("popup/position", QPoint(50,50)).value<QPoint>());
    createTrayIcon();
    connect(ui->pushOpen, &QPushButton::clicked, this, &Popup::openBrowser);
    setAttribute(Qt::WA_ShowWithoutActivating);
    qApp->installEventFilter(this);
}

void Settings::showEvent(QShowEvent *event) {
    QDialog::showEvent(event);
    load();
}

void Settings::load(){
    qDebug() << "load";
    ui->link->setText(settings.value("user/link", "http://meet.ivjh.ru:8555/").toString());
    ui->name->setText(settings.value("user/name", tr("Пользователь")).toString());
    ui->server->setText(settings.value("transport/server", "popups.eri.su").toString());
    QStringList selected = settings.value("user/groups").toStringList();

    groups = (selected + groups);
    groups.removeDuplicates();
    qDebug() << groups;
    groupsModel.setStringList(groups);
}


Settings::Settings(QWidget *parent)
    : QDialog(parent )
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
    connect(this, &QDialog::accepted, this, &Settings::saveSettings);
//    groups = new QStringListModel(this);
//    groupsSelected = new QStringListModel(this);
    groupsModel.setStringList(groups);
    ui->groups->setModel(&groupsModel);

    connect(ui->append, &QPushButton::clicked,this, &Settings::appendGroup);
    load();
}


Settings::~Settings()
{
    delete ui;
}

void Settings::appendGroup() {
    groups << ui->group->text();
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
    hide();
}

void Popup::open(QJsonObject jobject)
{
    qDebug() << "open" << jobject;
    if (jobject.value("action") == "invite") {
        qDebug() << "invite" ;
        current_link = jobject["link"].toString();
        QString room = jobject["link"].toString().split('/', QString::SkipEmptyParts).takeLast();
        ui->labelInvite->setText(jobject["name"].toString() + tr(" приглашает в конференцию ") + room);
        show();
        //raise();
    }
}


bool Popup::eventFilter(QObject *obj, QEvent *rawevent)
{
    QMouseEvent *event;
    if (rawevent->type() == QEvent::MouseMove) {
        event = static_cast<QMouseEvent*>(rawevent);
        if (event->buttons() & Qt::LeftButton) {
            move(event->globalPos() - dragPosition);
            event->accept();
            settings.setValue("popup/position", pos());
        }
    }
    if (rawevent->type() == QEvent::MouseButtonPress) {
        event = static_cast<QMouseEvent*>(rawevent);
        if (event->button() == Qt::LeftButton) {
            dragPosition = event->globalPos() - frameGeometry().topLeft();
            event->accept();
        }
    }
    return false;
}



void Popup::showInvite()
{
    qDebug() << "showInvite";
    bool ok;
    QString text;
    QJsonObject jobject;
    text = QInputDialog::getText(
                this,
                tr("Приглашение"),
                tr("Конференция:                           "),
                QLineEdit::Normal,
                settings.value("user/room", "Планёрка").toString(),
                &ok
                );
    if (ok) {
        settings.setValue("user/room", text);
        jobject["link"] = settings.value("user/link", "http:/").toString() + "/"+ text;
        jobject["name"] = settings.value("user/name", tr("Пользователь")).toString();
        emit sendInvite(jobject);
    }
}

void Settings::saveSettings()
{
    settings.setValue("user/name", ui->name->text());
    settings.setValue("user/link", ui->link->text());
    settings.setValue("transport/server", ui->server->text());
    settings.setValue("user/groups", ((QStringListModel*) ui->groups->selectionModel()->model())->stringList());
    //emit changedServer(ui->server->text());
}

void Popup::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);

    QAction *inviteAction = new QAction(tr("Пригласить"), this);
    connect(inviteAction, &QAction::triggered, this, &Popup::showInvite);

    QAction *settingsAction = new QAction(tr("Настройки"), this);
    connect(settingsAction, &QAction::triggered, settingsDialog, &Settings::show);


    QMessageBox *aboutBox = new QMessageBox(this);
    aboutBox->setText(tr("<h3>Агент конференций</h3>"));
    aboutBox->setInformativeText(tr("ⓒ 2020, Aleksandr Stepanov <sa@eri.su>"));
    aboutBox->setStandardButtons(QMessageBox::Ok);
    //aboutBox->setIcon(QMessageBox::Information);

    QAction *aboutAction = new QAction(tr("О программе"), this);
    connect(aboutAction, &QAction::triggered, aboutBox, &QMessageBox::exec);

    QAction *quitAction = new QAction(tr("Выход"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    QMenu *trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(inviteAction);
    trayIconMenu->addAction(settingsAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icons/icon.ico"));
    //connect(trayIcon, &QSystemTrayIcon::activated,trayIconMenu,&QMenu::show);
    //connect(trayIcon, &QSystemTrayIcon::activated,this,&Popup::showInvite);
    trayIcon->show();

}

