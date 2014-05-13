#ifndef QVLC_MININGINFO_DIALOG_H_
#define QVLC_MININGINFO_DIALOG_H_ 1

#include "qt4.hpp"
#include <QWidget>

class QRadioButton;
class QComboBox;
class QLabel;
class QKeyEvent;
class MiningInfoWidget : public QWidget
{
    Q_OBJECT
public:
    static void ShowMiningDialog( QWidget *_p, intf_thread_t *p_intf, int type )
    {
        new MiningInfoWidget( _p, p_intf, type );
    }
    MiningInfoWidget( QWidget *, intf_thread_t *, int );
    
private:
    intf_thread_t *p_intf;
    int mType;
    int value0;
    int value1;
    
private slots:
    void button0Clicked();
    void button1Clicked();
};

#endif