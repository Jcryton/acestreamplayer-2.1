#ifndef AWB_BROWSERMANAGER_H
#define AWB_BROWSERMANAGER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <QObject>

#include "acewebbrowser/loaditem.hpp"
#include "acewebbrowser/browser.hpp"

namespace AceWebBrowser {

class BrowserManager : public QObject
{
    Q_OBJECT
public:
    explicit BrowserManager(QObject *parent = 0);
    ~BrowserManager();

    Browser *createBrowser(const LoadItem &item, QWidget *parent);
    void closeBrowser(BrowserType type);

    bool isVisible() const;
    bool hasBrowser(BrowserType type);

    Browser *getBrowser(BrowserType type);
    Browser *getVisibleBrowser();
    
    void updateBrowsersOnVoutChanged(bool);

private:
    void deleteBrowser(int index);


    int getBrowserIndex(BrowserType type);

private:
    QList<Browser *> mBrowsers;

private slots:
    void handleBrowserClosed(BrowserType type);
};

}

#endif // AWB_BROWSERMANAGER_H
