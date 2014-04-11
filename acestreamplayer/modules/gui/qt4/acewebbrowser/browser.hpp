#ifndef AWB_BROWSER_H
#define AWB_BROWSER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <QWidget>

#include "acewebbrowser/loaditem.hpp"

QT_BEGIN_NAMESPACE
class QToolBar;
class QTimer;
QT_END_NAMESPACE

namespace AceWebBrowser {
class JavaScriptObject;
class WebView;
class BrowserManager;
class Browser : public QWidget
{
    Q_OBJECT

    enum BrowserState {
        BS_UNLOADED,
        BS_LOADING,
        BS_LOADED
    };

    enum BrowserAction {
        BA_UNDEF,
        BA_SHOW,
        BA_HIDE,
        BA_CLOSE
    };

    typedef bool (*BrowserCondition)(const Browser*);

public:
    Browser(const LoadItem &item, BrowserManager *manager, QWidget *parent = 0);
    ~Browser();
    
    void setPlayerFullscreen(bool state);
    void setPlayerState(int state, bool isAd);

    QString baseUrl() const;
    BrowserType type();
    BrowserCookies cookiesType();
    QStringList embedScripts();
    QString embedCode();

    void setShowAvailable(bool);
    bool showAvailable() const;
    void setVisiabilityProcessingEnable(bool);
    
    bool dieing() const;
    bool isBrowserModeEnabled() const;
    bool isFullSized();

    JavaScriptObject *javaScriptObject();
    BrowserManager *manager() const;
    bool showOnlyOnPlayerStateChagned() const;

    bool isAlreadyLoading(const LoadItem &item);
    void load(const LoadItem &item);

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    inline QString setupButtonStyle(const QString &name, int width, int height, const QString &horalign = "left", bool hasDisableState = true);
    void createNavigationBar();
    inline void enableNavigationButton(QAction *action, bool value);

    void updateSizing(const QSize &size);
    void updateSizing();
    void updatePosition();
    void updateScrollbars();

    void createWebView();
    void deleteWebView();
    void configureWebPage();

    void processPlayerActiveStatesAfterLoading();
    void processPlayerActiveStatesAfterStateChanged();

    void doAction(BrowserAction action, BrowserCondition condition);

    void load();

private:
    QLayout *mLayout;
    WebView *mWebView;
    JavaScriptObject *mJSO;
    BrowserManager *mManager;
    LoadItem mItem;

    QToolBar *mNavigationBar;
    QAction *mBackAction;
    QAction *mFwdAction;
    QAction *mCloseAction;

    bool mIsContentRequested;
    BrowserState mState;
    bool mIsAd;
    bool mDieing;

    int mParentState;
    bool mParentFullscreen;

    bool mBrowserModeEnabled;
    bool mShowOnPlayerStateChangedOnly;
    bool mShowAvailable;
    bool mVisiabilityProcessingEnable;
    
    QTimer *mCloseAfterTimer;
    QTimer *mHideTimer;
    QTimer *mDeferredTimer;

signals:
    void notifyBrowserClosed(); // for manager
    void notifyBrowserVisiabilityChanged(AceWebBrowser::BrowserType type, bool isVisible);
    void notifyBrowserSizeChanged(const QSize &);
    void notifyNeedExitFullscreen();
    void notifyBrowserPosition(AceWebBrowser::BrowserType, const QPoint&, const QSize&);

    void registerBrowserShownEvent(AceWebBrowser::BrowserType type, QString id);
    void registerBrowserHideEvent(AceWebBrowser::BrowserType type, QString id);
    void registerBrowserClosedEvent(AceWebBrowser::BrowserType type, QString id, bool failed, bool isInBrowserMode);
    void registerBrowserErrorEvent(AceWebBrowser::BrowserType type, QString id);
    void registerSendEvent(AceWebBrowser::BrowserType type, QString event_name, QString id);

    void gotFocus();
    void notifyParentCommandToShow(AceWebBrowser::BrowserType type);

private slots:
    void backActionTriggered();
    void fwdActionTriggered();
    void closeActionTriggered();
    void updateActionButtons();

    void handleGotFocus();
    void activateBrowserMode(bool activate = true);
    void handleWindowOpen();
    void handleWindowClose();

    void pageLoadStarted();
    void pageLoadFinished(bool status);
    void openUrl(const QUrl& url);
    void openUrl(QString url, bool inNewWindow);

    void handleJSOFillParentSizeCommand();
    void handleJSOResizeCommand(QSize);
    void handleJSOSendEvent(QString event_name);
    
public slots:
    // actions
    void showBrowser();
    void hideBrowser();
    void closeBrowser(bool failed = false);
    void deferredCloseBrowser(unsigned int);

    void handleParentSize(const QSize &size);
    void handleParentFullscreen(bool isFullscreen);
    void handleParentState(int state, bool isAd);
    void handleParentPauseClicked();
    void handleParentStopClicked();
};

}

#endif // AWB_BROWSER_H
