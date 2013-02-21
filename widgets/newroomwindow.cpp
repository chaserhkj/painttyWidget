#include "newroomwindow.h"
#include "ui_newroomwindow.h"

NewRoomWindow::NewRoomWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewRoomWindow)
{
    ui->setupUi(this);
    ui->buttonBox->
            button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(ui->lineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(onNameChanged(const QString &)));
    connect(ui->buttonBox->button(QDialogButtonBox::Ok),
            SIGNAL(clicked()),
            this, SLOT(onOk()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel),
            SIGNAL(clicked()),
            this, SLOT(onCancel()));
}

NewRoomWindow::~NewRoomWindow()
{
    delete ui;
}

void NewRoomWindow::onServerResponse(const QVariantMap &m)
{
    qDebug()<<"server: response"<<m;
    if(m["response"] != "newroom"
            || !m.contains("result"))
        return;
    if(m["result"].toBool()){
        // TODO
        QString msg = tr("Succeed!");
        QMessageBox::information(this, tr("Go get your room!"),
                                 msg,
                                 QMessageBox::Ok);
        this->accept();
    }

    if(!m.contains("errcode")){
        return;
    }else{
        int errcode = m["errcode"].toInt();
        QString errmsg = tr("Error: ")+QString::number(errcode)+
                tr("\nDo you want to retry?");
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(this, tr("Error!"),
                                      errmsg,
                                      QMessageBox::Retry|
                                      QMessageBox::Abort);
        if(reply == QMessageBox::Retry){
            this->onOk();
        }else{
            return;
        }
//        switch (errcode){
//        // TODO: complete
//        case 200:
//        case 201:
//        case 202:
//        case 203:
//        case 204:
//        case 205:
//        case 206:
//        case 207:
//        case 208:
//        case 209:
//        default:
//            break;
//        }
    }
}

void NewRoomWindow::onOk()
{
    QString s = ui->lineEdit->text();
    if(s.isEmpty() ||
            s.length() > 16){
        QString errmsg = tr("Your room name is empty or too long!");
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(this, tr("Error!"),
                                      errmsg,
                                      QMessageBox::Abort);
        if(reply == QMessageBox::Abort){
            return;
        }
    }
    s = ui->plainTextEdit->toPlainText();
    if(s.length()>40){
        QString errmsg = tr("Your welcome message is too long!");
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(this, tr("Error!"),
                                      errmsg,
                                      QMessageBox::Abort);
        if(reply == QMessageBox::Abort){
            return;
        }
    }

    QString pw;
    if(ui->checkBox->isChecked()){
        pw = ui->lineEdit_2->text();
    }

    QVariantMap map;
    map.insert("name", ui->lineEdit->text());
    map.insert("welcomemsg", ui->plainTextEdit->toPlainText());
    map.insert("maxload", ui->spinBox->value());
    map.insert("password", pw);
    map.insert("emptyclose", ui->checkBox_2->isChecked());
    // TODO: send to server
    qDebug()<<"client: request"<<map;
    ui->progressBar->setValue(30);
    emit newRoom(map);
}

void NewRoomWindow::onCancel()
{
    QMessageBox::StandardButton reply;
    QString msg = tr("Do you really want to cancel?");
    reply = QMessageBox::question(this, tr("Cancel?"),
                                  msg,
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
        this->reject();
}

void NewRoomWindow::onNameChanged(const QString &name)
{
    ui->progressBar->setValue(0);
    if(!name.isEmpty()){
        ui->buttonBox->
                button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void NewRoomWindow::disableEdit(bool t)
{
    this->setDisabled(t);
    ui->buttonBox->setEnabled(true);
}
