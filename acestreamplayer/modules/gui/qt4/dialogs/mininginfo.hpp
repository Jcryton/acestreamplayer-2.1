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
    static void ShowMiningDialog( QWidget *_p, intf_thread_t *p_intf, int type, const QString &text, const QString &text1, const QString &url1, const QString &text2, const QString &url2 )
    {
        new MiningInfoWidget( _p, p_intf, type, text, text1, url1, text2, url2 );
    }
    MiningInfoWidget( QWidget *, intf_thread_t *, int, const QString&, const QString&, const QString&, const QString&, const QString& );
    
private:
    intf_thread_t *p_intf;
    int mType;
    QString mBtn1Url;
    QString mBtn2Url;
    
private slots:
    void button0Clicked();
    void button1Clicked();
};

#endif