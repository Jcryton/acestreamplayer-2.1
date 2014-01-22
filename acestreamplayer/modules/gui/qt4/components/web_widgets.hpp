#ifndef _WEB_WIDGETS_H_H_
#define _WEB_WIDGETS_H_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "main_interface.hpp"
#include "input_manager.hpp"

#include <QWidget>

class BrowserWebView;
class QToolBar;
class AbstractBrowser : public QWidget
{
    Q_OBJECT
public:
    AbstractBrowser(intf_thread_t *intf, QWidget *parent = 0);

    void setDelegateLinks(bool);
    void setViewSize(int, int);
    void updateViewSize(int left = -10, int top = -10, int right = -10, int bottom = -10);
    void setIsInFullscreenInJSO(bool);
    void setScrollbarsEnabled(bool);
    void setJSDialogsEnable(bool);
    void setEmbedScript(const QString&);
    void setCookiesPolicy(int);
    void setFlashEnable(bool);
    
    virtual void load(QString strurl);

protected:
    intf_thread_t *m_intf;
    BrowserWebView *m_wv;
    QToolBar *m_navigation;
    QAction *m_backward;
    QAction *m_forward;
    QAction *m_close;
    QWidget *m_spacer;
    int m_top;
    int m_bottom;
    int m_left;
    int m_right;
    int m_width;
    int m_height;
    bool m_stretch_width;
    bool m_stretch_height;

private:
    void setupNavigationBar();
    virtual void contextMenuEvent( QContextMenuEvent *event );
    QString navigationBarButtonStyle(const QString &name, int width, int height, QString horalign = "left", bool hasDisableState = true);
    void enableHistory(QAction *action, bool value);

signals:
    void raiseControls();
    
    void jsCommandCloseFullscreen();
    void jsCommandPlayerPlay();
    void jsCommandPlayerPause();
    void jsCommandPlayerUnPause();
    void jsCommandPlayerStop();

public slots:
    virtual void showBrowser();
    virtual void hideBrowser();
    virtual void clear();

private slots:
    void resizeAndEnableToolbar();
    void backBrowser();
    void nextBrowser();
    void buttonHideBrowser();

protected slots:
    virtual void pageLoaded(bool);
};

class CommonInTimeBrowser : public AbstractBrowser
{
    Q_OBJECT
public:
    CommonInTimeBrowser(intf_thread_t *intf, QWidget *parent = 0);

    virtual void load(QString strurl);

    int bottom() const;
    void setAdType(int);
    int getAdType();
    bool enabled();

private:
    bool m_enabled;

public slots:
    virtual void showBrowser();
    virtual void hideBrowser();

private slots:
    virtual void pageLoaded(bool);
    
signals:
    void showLoadFailDialog();
};

class InteractivePausePreloadBrowser : public AbstractBrowser
{
    Q_OBJECT
public:
    InteractivePausePreloadBrowser(intf_thread_t *intf, QWidget *parent = 0);
    
    void load(QString strurl, QString id);

    bool isAlreadyLoaded(QString, QString);
    bool isAlreadyLoading(QString url, QString id);

    int bottom() const;
    bool ready() const;

private:
    QString m_preload_id;
    bool m_has_page;

signals:
    void registerShowing(QString);

public slots:
    virtual void showBrowser();
    virtual void hideBrowser();
    virtual void clear();

private slots:
    virtual void pageLoaded(bool);
};

class OverlayPreloadAdBrowser : public AbstractBrowser
{
    Q_OBJECT
public:
    OverlayPreloadAdBrowser(intf_thread_t *intf, QWidget *parent = 0);
    
    void load(QString strurl, QString id);

    bool isAlreadyLoading(QString url, QString id);

    int bottom() const;
    bool ready() const;

private:
    bool m_closed;
    QString m_preload_id;
    bool m_has_page;

signals:
    void overlayLoaded();
    void closeNonLinearBrowser(QString);
    void registerShowing(QString);

public slots:
    virtual void showBrowser();
    virtual void hideBrowser();
    virtual void clear();
    void tempHideBrowser();

private slots:
    virtual void pageLoaded(bool);
    void closeNonLinear();
};

#endif