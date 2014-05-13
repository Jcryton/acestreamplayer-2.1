#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "dialogs/mininginfo.hpp"

#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>

MiningInfoWidget::MiningInfoWidget(QWidget *_p, intf_thread_t *_p_intf, int _type ) : 
    QWidget( _p )
  , p_intf( _p_intf )
  , mType(_type)
{
    setWindowTitle("");
    setWindowModality( Qt::ApplicationModal );
    setWindowFlags( Qt::Dialog | Qt::CustomizeWindowHint );
    setAttribute( Qt::WA_DeleteOnClose );
    
    QVBoxLayout *main = new QVBoxLayout(this);
    QLabel *lblText = new QLabel(this);
    QHBoxLayout *buttons = new QHBoxLayout(this);
    QPushButton *button0 = new QPushButton(this);
    QPushButton *button1 = new QPushButton(this);
    if(mType == 1)  {
        lblText->setText(qtr("Buy account or activate mining to use AceStream."));
        button0->setText(qtr("Buy"));
        button1->setText(qtr("Activate"));
    }
    else if(mType == 2) {
        lblText->setText(qtr("Activate mining to get bonuses."));
        button0->setText(qtr("Activate"));
        button1->setText(qtr("Skip"));
    }
    main->addWidget(lblText);
    buttons->addWidget(button0);
    buttons->addWidget(button1);
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
    if(mType == 1) {
        QDesktopServices::openUrl(QUrl("https://accounts.acestream.net/wizard/noads"));
    }
    else {
        p2p_UserDataMining(THEP2P, 1);
    }
    close();
}

void MiningInfoWidget::button1Clicked()
{
    p2p_UserDataMining(THEP2P, mType == 1 ? 1 : 0);
    close();
}