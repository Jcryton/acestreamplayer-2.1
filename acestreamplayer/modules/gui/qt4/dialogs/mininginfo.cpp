#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "dialogs/mininginfo.hpp"
#include "actions_manager.hpp" 

#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>

MiningInfoWidget::MiningInfoWidget(QWidget *_p, intf_thread_t *_p_intf, int _type, const QString &_text, const QString &_text1, const QString &_url1, const QString &_text2, const QString &_url2 ) : 
    QWidget( _p )
  , p_intf( _p_intf )
  , mType(_type)
  , mBtn1Url(_url1)
  , mBtn2Url(_url2)
{
    setWindowTitle("");
    setWindowModality( Qt::ApplicationModal );
    setWindowFlags( Qt::Dialog | Qt::CustomizeWindowHint );
    setAttribute( Qt::WA_DeleteOnClose );
    
    QVBoxLayout *main = new QVBoxLayout(this);
    QLabel *lblText = new QLabel(this);
    lblText->setMaximumWidth(250);
    QHBoxLayout *buttons = new QHBoxLayout(this);
    QPushButton *button0 = new QPushButton(this);
    QPushButton *button1 = new QPushButton(this);
    lblText->setText(_text);
    button0->setText(_text1);
    button1->setText(_text2);
    main->addWidget(lblText);
    if(mType < P2P_INFOW_TYPE_5) {
        buttons->addWidget(button0);
        buttons->addWidget(button1);
    }
    else if(mType < P2P_INFOW_TYPE_6) {
        buttons->addWidget(button0);
    }
    main->addLayout(buttons);
    
    setLayout(main);
    CONNECT(button0, clicked(), this, button0Clicked());
    CONNECT(button1, clicked(), this, button1Clicked());
    setVisible(true);

    if( _p ) {
        move(_p->x() + _p->width() / 2 - width() / 2, _p->y() + _p->height() / 2 - height() / 2);
    }
}

void MiningInfoWidget::button0Clicked()
{
    switch(mType) {
    case P2P_INFOW_TYPE_1:
        QDesktopServices::openUrl(QUrl(mBtn1Url));
        close();
        break;
    case P2P_INFOW_TYPE_2:
        QDesktopServices::openUrl(QUrl(mBtn1Url));
        close();
        THEAM->play();
        break;
    case P2P_INFOW_TYPE_3:
        p2p_UserDataMining(THEP2P, 1);
        close();
        THEAM->play();
        break;
    case P2P_INFOW_TYPE_4:
    case P2P_INFOW_TYPE_5:
        QDesktopServices::openUrl(QUrl(mBtn1Url));
        break;
    default:
        break;
    }
}

void MiningInfoWidget::button1Clicked()
{
    switch(mType) {
    case P2P_INFOW_TYPE_1:
    case P2P_INFOW_TYPE_2:
        p2p_UserDataMining(THEP2P, 1);
        close();
        THEAM->play();
        break;
    case P2P_INFOW_TYPE_3:
        close();
        THEAM->play();
        break;
    case P2P_INFOW_TYPE_4:
        QDesktopServices::openUrl(QUrl(mBtn1Url));
        break;
    default:
        break;
    }
}