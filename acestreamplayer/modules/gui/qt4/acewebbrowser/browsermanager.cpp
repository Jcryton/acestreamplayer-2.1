#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "qt4.hpp"

#include "acewebbrowser/browsermanager.hpp"
#include "acewebbrowser/cookiemanager.hpp"

using namespace AceWebBrowser;

BrowserManager::BrowserManager(QObject *parent) :
    QObject(parent)
{
    CookieManager *cookieManager = CookieManager::getInstanse();
    CookieManager::holdInstanse(cookieManager);
}

BrowserManager::~BrowserManager()
{
    for(int i = mBrowsers.size()-1; i >= 0; --i) {
        deleteBrowser(i);
    }
    CookieManager *cookieManager = CookieManager::getInstanse();
    CookieManager::releaseInstance(cookieManager);
}

Browser *BrowserManager::createBrowser(const LoadItem &item, QWidget *parent)
{
    Browser *browser = getBrowser(item.type());
    if(browser)
        return browser;

    Browser *newBrowser = new Browser(item, this, parent);
    connect(newBrowser, SIGNAL(notifyBrowserClosed(BrowserType)), SLOT(handleBrowserClosed(BrowserType)));

    mBrowsers.append(newBrowser);
    qDebug() << "BrowserManager::createBrowser: Adding new browser to manager. Type" << item.type() << "Size" << mBrowsers.size();
    return newBrowser;
}

void BrowserManager::closeBrowser(BrowserType type)
{
    qDebug() << "BrowserManager::closeBrowser: type" << type;
    int index = getBrowserIndex(type);
    deleteBrowser(index);
}

Browser *BrowserManager::getBrowser(BrowserType type)
{
    Browser *browser = NULL;
    foreach (Browser *b, mBrowsers) {
        if(b->type() == type && !b->dieing()) {
            browser = b;
            break;
        }
    }
    return browser;
}

void BrowserManager::updateBrowsersOnVoutChanged(bool hasVout)
{
    if(hasVout) {
        qDebug() << "BrowserManager::updateBrowsersOnVoutChanged: updating browser visiability on vout appeares";
        Browser *overlay = getBrowser(BTYPE_OVERLAY);
        if(overlay) {
            overlay->showBrowser();
        }
        Browser *slider = getBrowser(BTYPE_SLIDER);
        if(slider) {
            slider->showBrowser();
        }
    }
}

Browser *BrowserManager::getVisibleBrowser()
{
    Browser *browser = NULL;
    foreach (Browser *b, mBrowsers) {
        if(b->isVisible()) {
            browser = b;
            break;
        }
    }
    return browser;
}

int BrowserManager::getBrowserIndex(BrowserType type)
{
    int index = -1;
    for(int i = 0; i < mBrowsers.size(); ++i) {
        if(mBrowsers.at(i)->type() == type) {
            index = i;
            break;
        }
    }
    return index;
}

void BrowserManager::deleteBrowser(int index)
{
    if(index >= 0 && index < mBrowsers.size()) {
        Browser *browser = mBrowsers.at(index);
        mBrowsers.removeAt(index);
        qDebug() << "BrowserManager::deleteBrowser: Removing browser from manager. Type" << browser->type() << "Size" << mBrowsers.size();
        browser->deleteLater();
    }
}

bool BrowserManager::isVisible() const
{
    bool vis = false;
    foreach (Browser *b, mBrowsers) {
        if(b->isVisible()) {
            vis = true;
            break;
        }
    }
    return vis;
}

bool BrowserManager::hasBrowser(BrowserType type)
{
    Browser *browser = getBrowser(type);
    return browser && !browser->dieing();
}

void BrowserManager::handleBrowserClosed(BrowserType type)
{
    qDebug() << "BrowserManager::handleBrowserClosed: type" << type;
    int index = getBrowserIndex(type);
    deleteBrowser(index);
}
