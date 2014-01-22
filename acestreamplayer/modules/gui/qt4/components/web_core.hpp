#ifndef _WEB_CORE_H_H_
#define _WEB_CORE_H_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <QWebView>
#include <QNetworkReply>
#include <QNetworkCookieJar>
#include <QSslError>
#include <QList>
#include <QMultiHash>
#include <QDateTime>
#include <QMutex>

#include "main_interface.hpp"
#include "input_manager.hpp"

class CookieJar : public QNetworkCookieJar
{
    Q_OBJECT
    Q_PROPERTY(AcceptPolicy acceptPolicy READ acceptPolicy WRITE setAcceptPolicy)
public:
    enum AcceptPolicy {
        AcceptOur = 1,
        AcceptAll = 2,
        AcceptNone = 3
    };

    explicit CookieJar(QObject *parent = 0);
    ~CookieJar();

    QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);

    AcceptPolicy acceptPolicy() const;
    void setAcceptPolicy(AcceptPolicy policy);
private:
    bool m_loaded;
    AcceptPolicy m_acceptCookies;

private slots:
    void update();
};

class CookieLoader : public QObject
{
    Q_OBJECT
public:
    static CookieLoader *getInstanse(QObject *parent=0);
    static void hold();
    static void release();

    CookieJar *createCookieJar(CookieJar::AcceptPolicy = CookieJar::AcceptOur);
    QList<QNetworkCookie> getCookies(const QString& key = "");
    void insertCookie(QNetworkCookie cookie);

private:
    explicit CookieLoader(QObject *parent=0);
    ~CookieLoader();

    void load();
    inline void loadList(QDataStream &stream);
    void save(bool clear_session = false);
    inline void saveList(QDataStream &stream, const QString &key, const QList<QNetworkCookie> &list);

    void insertCookies(const QString &key, const QList<QNetworkCookie> &list);
    void insertCookie(const QString &key, const QNetworkCookie &cookie);
    
    QMultiHash<QString, QNetworkCookie>::iterator findCookie(const QNetworkCookie &value);
    void deleteKey(const QString &key);
    void clearCookies(bool clear_session);

private:
    static CookieLoader *_instance;
    static QMutex _lock;
    static int _holders_count;

    QMutex m_lock;
    QMultiHash<QString, QNetworkCookie> m_cookies;
    QDateTime m_last_import;

signals:
    void notifyChanged();
};

class JavaScriptObject : public QObject
{
    Q_OBJECT
public:
    JavaScriptObject(QObject *parent = 0) : QObject(parent) {}
    void setIsFullscreen(bool val) { m_is_fullscreen = val; }
    void setIsVisible(bool val) { m_is_visible = val; }

private:
    bool m_is_fullscreen;
    bool m_is_visible;
    
signals:
    void clicked();
    void jsoLinkOpen(QString url, bool openInNewWindow);
    void jsoPlayerPlay();
    void jsoPlayerPause();
    void jsoPlayerStop();

public slots:
    void elementClicked();
    
    void browserClose();
    void linkOpen(QString url, bool openInNewWindow);
    void playerPlay();
    void playerPause();
    void playerStop();
    bool playerIsFullscreen() { return m_is_fullscreen; }
    bool browserIsVisible() { return m_is_visible; }

    QString pluginVersion() { return ""P2P_STD_VERSION""; }
    bool flashEnabled() { return true; }
    QString getHostUserAgent() { return "AcePlayer/" + pluginVersion(); }
};

class TinyPage : public QWebPage
{
    Q_OBJECT
public:
    TinyPage(QObject *parent = 0) : QWebPage(parent) {}
    QString userAgentForUrl ( const QUrl & url ) const;

protected:
    bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type);

signals:
    void redirected();
};

class TinyBrowser : public QWebView
{
    Q_OBJECT
public:
    TinyBrowser(QWidget *parent = 0) : QWebView(parent) {
        setPage(new TinyPage(this));
        connect(page(), SIGNAL(redirected()), SIGNAL(deleteUnneed()));
    }

signals:
    void deleteUnneed();
};

class BrowserPage : public QWebPage
{
    Q_OBJECT
public:
    BrowserPage(QObject *parent = 0);
    QString userAgentForUrl ( const QUrl & url ) const;
    void setDialogsEnable(bool val) { m_enable_dialogs = val; }
    void setDialogCanShow(bool val) { m_can_show_dialog = m_enable_dialogs && val; }

protected:
    void javaScriptAlert(QWebFrame *originatingFrame, const QString &msg);
    bool javaScriptConfirm(QWebFrame *originatingFrame, const QString &msg);

private:
    bool m_enable_dialogs;
    bool m_can_show_dialog;
};

class BrowserWebView : public QWebView
{
    Q_OBJECT
public:
    BrowserWebView(intf_thread_t *intf, QWidget *parent = 0);

    void setDelegateLinks(bool);
    void setType(int val) { m_type = val; }
    int getType() { return m_type; }
    void setIsInFullscreen(bool);
    void setEmbedScript(const QString&);
    void setFlashEnabled(bool);
    void setCookiesPolicy(CookieJar::AcceptPolicy);
    
    void enableScrollBars(bool);
    void enableJSDialogs(bool);
    
    void setLoadingUrl(QUrl val) { m_loading_url = val; }
    const QUrl loadingUrl() const { return m_loading_url; }

protected:
    QWebView *createWindow(QWebPage::WebWindowType type);

    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

private:
    intf_thread_t *m_intf;
    BrowserPage *m_page;
    TinyBrowser *m_tiny;
    bool m_delegate_links;
    QList<QString> m_sslTrustedHostList;
    JavaScriptObject *m_jso;
    int m_type;
    QUrl m_loading_url;
    QString m_embed_script;

signals:
    void internalNavigation();
    void loadingFinished(bool);
    void showBrowser();
    void hideBrowser();
    void showMessageDialog();
    
    void jsCommandCloseFullscreen();
    void jsCommandPlayerPlay();
    void jsCommandPlayerPause();
    void jsCommandPlayerStop();

public slots:
    void loadAdPage(QString);
    void show();
    void close();

private slots:
    void openLink(QUrl);
    void deleteTiny();
    void namFinished(QNetworkReply*);
    void pageFinished(bool);
    void pageClose();
#ifndef QT_NO_OPENSSL
    void namSslErrors(QNetworkReply*,QList<QSslError>);
#endif

    void jsoLinkOpen(QString url, bool openInNewWindow);

    void singleFrameCreated(QWebFrame*);
    void singleFrameLoadFinished(bool);
};

#endif