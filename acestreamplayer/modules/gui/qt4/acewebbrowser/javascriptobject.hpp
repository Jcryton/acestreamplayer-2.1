#ifndef AWB_JAVASCRIPTOBJECT_H
#define AWB_JAVASCRIPTOBJECT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <QObject>
#include <QSize>

namespace AceWebBrowser {

class JavaScriptObject : public QObject
{
    Q_OBJECT
public:
    explicit JavaScriptObject(QObject *parent = 0);

    void setIsInFullscreen(bool value);
    void setFlashEnabled(bool value);
    void setVisiability(bool value);
    void setHostUserAgent(const QString &value);

private:
    bool mPlayerInFullscreen;
    bool mFlashEnabled;
    bool mBrowserIsVisible;
    QString mHostUserAgent;
    QSize mBrowserSize;
    QSize mPlayerSize;

signals:
    void jsoCloseBrowser();
    void jsoLinkOpen(QString url, bool openInNewWindow);
    void jsoPlayerPlay();
    void jsoPlayerPause();
    void jsoPlayerStop();
    void jsoLinkOpenLater(QString url, bool high_priority);
    void jsoFillPlayerSize();
    void jsoPlayerSetFullscreen(bool);
    void jsoBrowserSetSize(const QSize&);

public slots:
    void browserClose();
    void linkOpen(QString url, bool openInNewWindow);
    void playerPlay();
    void playerPause();
    void playerStop();
    void linkOpenLater(QString url, bool high_priority);
    void fillPlayerSize();
    void playerSetFullscreen(bool);
    void browserSetSize(unsigned int, unsigned int);

    bool playerIsFullscreen();
    bool browserIsVisible();
    QString pluginVersion();
    bool flashEnabled();
    QString getHostUserAgent();
    unsigned int playerWidth() { return mPlayerSize.width(); }
    unsigned int playerHeight() { return mPlayerSize.height(); }
    unsigned int browserWidth() { return mBrowserSize.width(); }
    unsigned int browserHeight() { return mBrowserSize.height(); }
    
    void debug(QString msg);
    
    void handlePlayerSizeChanged(const QSize &);
    void handleBrowserSizeChanged(const QSize &);
};

}

#endif // AWB_JAVASCRIPTOBJECT_H
