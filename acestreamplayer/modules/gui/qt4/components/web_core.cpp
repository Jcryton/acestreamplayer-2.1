#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QDesktopServices>
#include <QSettings>
#include <QDateTime>
#include <QWebFrame>
#include <QFile>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDataStream>
#include <QMessageBox>

#include <p2p_object.h>
#include "components/web_core.hpp"

CookieJar::CookieJar(QObject *parent)
    : QNetworkCookieJar(parent)
    , m_loaded(false)
{
}

CookieJar::~CookieJar()
{
}

QList<QNetworkCookie> CookieJar::cookiesForUrl(const QUrl &url) const
{
    CookieJar *that = const_cast<CookieJar*>(this);
    if (!m_loaded)
        that->update();

    return QNetworkCookieJar::cookiesForUrl(url);
}

bool CookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
    if (!m_loaded)
        update();

    if(m_acceptCookies != AcceptNone) {
        QString defaultDomain = url.host();
        QString pathAndFileName = url.path();
        QString defaultPath = pathAndFileName.left(pathAndFileName.lastIndexOf(QLatin1Char('/'))+1);
        if (defaultPath.isEmpty())
            defaultPath = QLatin1Char('/');

        foreach(QNetworkCookie cookie, cookieList)
        {
            QList<QNetworkCookie> lst;

            if(cookie.path().isEmpty())
                cookie.setPath(defaultPath);

            if (cookie.domain().isEmpty())
                cookie.setDomain(defaultDomain);

            lst += cookie;
            QNetworkCookieJar::setCookiesFromUrl(lst, url);
            CookieLoader::getInstanse()->insertCookie(cookie);
        }
        return true;
    }
    else {
        return false;
    }
}

CookieJar::AcceptPolicy CookieJar::acceptPolicy() const
{
    return m_acceptCookies;
}

void CookieJar::setAcceptPolicy(CookieJar::AcceptPolicy policy)
{
    m_acceptCookies = policy;
}

void CookieJar::update()
{
    QList<QNetworkCookie> cookies;
    switch(m_acceptCookies) {
    case AcceptOur:
        cookies = CookieLoader::getInstanse()->getCookies("def");
        break;
    case AcceptAll:
        cookies = CookieLoader::getInstanse()->getCookies();
        break;
    case AcceptNone:
    default:
        break;
    }
    setAllCookies(cookies);
    m_loaded = true;
}

CookieLoader *CookieLoader::_instance = 0;
QMutex CookieLoader::_lock;
int CookieLoader::_holders_count = 0;
CookieLoader *CookieLoader::getInstanse(QObject *obj)
{
    _lock.lock();
    if(!_instance)
    {
        _instance = new CookieLoader(obj);
    }
    _lock.unlock();
    return _instance;
}

void CookieLoader::hold()
{
    _lock.lock();
    _holders_count++;
    _lock.unlock();
}

void CookieLoader::release()
{
    _lock.lock();
    _holders_count--;

    if(_holders_count <= 0 && _instance) {
        delete _instance;
        _instance = 0;
        _holders_count = 0;
    }
    _lock.unlock();
}

CookieLoader::CookieLoader(QObject *parent)
    : QObject(parent)
{
    load();
}

CookieLoader::~CookieLoader()
{
    save(true);
    m_cookies.clear();
}

CookieJar *CookieLoader::createCookieJar(CookieJar::AcceptPolicy acceptPolicy)
{
    CookieJar *cj = new CookieJar;
    cj->setAcceptPolicy(acceptPolicy);
    connect(this, SIGNAL(notifyChanged()), cj, SLOT(update()));
    return cj;
}

QList<QNetworkCookie> CookieLoader::getCookies(const QString& key)
{
    return (key == "") ? (QList<QNetworkCookie>)(m_cookies.values()) : (QList<QNetworkCookie>)(m_cookies.values(key));
}

void CookieLoader::insertCookie(QNetworkCookie cookie)
{
    m_lock.lock();
    insertCookie("def", cookie);
    m_lock.unlock();
    save();
}

void CookieLoader::load()
{
    QMutexLocker ml(&m_lock);

    char *psz_datadir = config_GetUserDir( VLC_DATA_DIR );
    char *psz_file;
    if( asprintf( &psz_file, "%s" DIR_SEP "cookies", psz_datadir ) == -1 )
        psz_file = NULL;
    free( psz_datadir );
    if( psz_file == NULL ) return;
    QString filename(psz_file);
    free( psz_file );
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_7);

    loadList(in); // def
    loadList(in); // ff
    loadList(in); // ch
    loadList(in); // opwk

    in >> m_last_import;
}

void CookieLoader::loadList(QDataStream &stream)
{
    QString key;
    stream >> key;
    quint32 cooksize;
    stream >> cooksize;
    QList<QNetworkCookie> list;
    for(quint32 i = 0; i < cooksize; ++i)
    {
        QByteArray value;
        stream >> value;
        QList<QNetworkCookie> ck = QNetworkCookie::parseCookies(value);
        list.append(ck);
        if(stream.atEnd()) break;
    }
    insertCookies(key, list);
}

void CookieLoader::save(bool clear_session)
{
    QMutexLocker ml(&m_lock);
    clearCookies(clear_session);

    char *psz_datadir = config_GetUserDir( VLC_DATA_DIR );
    char *psz_file;
    if( asprintf( &psz_file, "%s" DIR_SEP "cookies", psz_datadir ) == -1 )
        psz_file = NULL;
    free( psz_datadir );
    if( psz_file == NULL ) return;
    QString filename(psz_file);
    free( psz_file );
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_7);

    saveList(out, "def", m_cookies.values("def"));
    saveList(out, "ff", m_cookies.values("ff"));
    saveList(out, "ch", m_cookies.values("ch"));
    saveList(out, "opwk", m_cookies.values("opwk"));
    out << m_last_import;
    file.close();
}

void CookieLoader::saveList(QDataStream &stream, const QString &key, const QList<QNetworkCookie> &list)
{
    stream << key;
    stream << (quint32)list.size();
    foreach (QNetworkCookie ck, list) {
        stream << ck.toRawForm();
    }
}

void CookieLoader::insertCookies(const QString &key, const QList<QNetworkCookie> &list)
{
    int d_cnt = 0;
    foreach (QNetworkCookie value, list)
    {
        QMultiHash<QString, QNetworkCookie>::iterator it = findCookie(value);
        if(it != m_cookies.end())
        {
            const QNetworkCookie &dup = (const QNetworkCookie)(it.value());
            if( dup.expirationDate().isValid()
                    && dup.expirationDate() > value.expirationDate() )
            {
                continue;
            }
            d_cnt++;
            m_cookies.erase(it);
        }
        m_cookies.insert(key, value);
    }
}

void CookieLoader::insertCookie(const QString &key, const QNetworkCookie &cookie)
{
    QMultiHash<QString, QNetworkCookie>::iterator it = findCookie(cookie);
    if(it != m_cookies.end())
    {
        //const QNetworkCookie &dup = (const QNetworkCookie)(it.value());
        m_cookies.erase(it);
    }
    m_cookies.insert(key, cookie);
    emit notifyChanged();
}

QMultiHash<QString, QNetworkCookie>::iterator CookieLoader::findCookie(const QNetworkCookie &value)
{
    QMultiHash<QString, QNetworkCookie>::iterator it;
    for(it = m_cookies.begin(); it != m_cookies.end(); ++it)
    {
        const QNetworkCookie &cur = (const QNetworkCookie)(it.value());
        if (value.name() == cur.name() &&
            value.domain() == cur.domain() &&
            value.path() == cur.path() &&
            value.isSecure() == cur.isSecure() &&
            value.isHttpOnly() == cur.isHttpOnly())
        {
            break;
        }
    }
    return it;
}

void CookieLoader::deleteKey(const QString &key)
{
    m_cookies.remove(key);
}

void CookieLoader::clearCookies(bool clear_session)
{
    QDateTime now = QDateTime::currentDateTime();
    QMultiHash<QString, QNetworkCookie>::iterator it;
    for(it = m_cookies.begin(); it != m_cookies.end();)
    {
        const QNetworkCookie &cur = (const QNetworkCookie)(it.value());
        if( cur.isSessionCookie() && clear_session )
        {
            it = m_cookies.erase(it);
        }
        else
        {
            if( !cur.isSessionCookie() && cur.expirationDate() < now )
            {
                it = m_cookies.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

void JavaScriptObject::elementClicked()
{
    emit clicked();
}

void JavaScriptObject::browserClose()
{
    emit clicked();
}

void JavaScriptObject::linkOpen(QString url, bool openInNewWindow)
{
    emit jsoLinkOpen(url, openInNewWindow);
}

void JavaScriptObject::playerPlay()
{
    emit jsoPlayerPlay();
}

void JavaScriptObject::playerPause()
{
    emit jsoPlayerPause();
}

void JavaScriptObject::playerStop()
{
    emit jsoPlayerStop();
}

bool TinyPage::acceptNavigationRequest(QWebFrame *,
                                       const QNetworkRequest &request,
                                       QWebPage::NavigationType)
{
    QDesktopServices::openUrl(request.url());
    emit redirected();
    return false;
}

QString TinyPage::userAgentForUrl(const QUrl &url) const
{
    (void)url;
    return "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/534.34 (KHTML, like Gecko) Qt/4.8.2 AceStream/"P2P_STD_VERSION"";
}

BrowserPage::BrowserPage(QObject *parent)
    : QWebPage(parent)
    , m_enable_dialogs(false)
    , m_can_show_dialog(false)
{}

QString BrowserPage::userAgentForUrl(const QUrl &url) const
{
    (void)url;
    return "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/534.34 (KHTML, like Gecko) Qt/4.8.2 AceStream/"P2P_STD_VERSION"";
}

void BrowserPage::javaScriptAlert(QWebFrame *originatingFrame, const QString &msg)
{
    Q_UNUSED(originatingFrame);
    if(m_can_show_dialog) {
        QMessageBox mb;
        mb.setText(msg);
        mb.exec();
    }
}

bool BrowserPage::javaScriptConfirm(QWebFrame *originatingFrame, const QString &msg)
{
    Q_UNUSED(originatingFrame);
    bool result = true;
    if(m_can_show_dialog) {
        QMessageBox mb;
        mb.addButton(QMessageBox::Yes);
        mb.addButton(QMessageBox::No);
        mb.setText(msg);
        result = (mb.exec() == QMessageBox::Yes);
    }
    return result;
}

#define ADD_JS_SCRIPT_JS "" \
    "var jsref = document.createElement('script');" \
    "jsref.setAttribute('type','text/javascript');" \
    "jsref.setAttribute('src', '%SCRIPT_URL%');" \
    "document.getElementsByTagName('head')[0].appendChild(jsref);"

BrowserWebView::BrowserWebView(intf_thread_t *intf, QWidget *parent)
    : QWebView(parent)
    , m_intf(intf)
    , m_tiny(0)
    , m_delegate_links(false)
    , m_jso(new JavaScriptObject(this))
    , m_page(new BrowserPage(this))
    , m_type(-1)
    , m_loading_url("about:blank")
    , m_embed_script("")
    , m_response_code(0)
{
    setObjectName("BrowserWebView");
   
    setPage(m_page);
    settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);

    //settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    page()->networkAccessManager()->setCookieJar(CookieLoader::getInstanse()->createCookieJar());
    setContextMenuPolicy(Qt::NoContextMenu);
    setVisible(false);

    m_sslTrustedHostList.append("acestream.net");

    connect(m_jso, SIGNAL(clicked()), SLOT(pageClose()));
    connect(m_jso, SIGNAL(jsoLinkOpen(QString,bool)), SLOT(jsoLinkOpen(QString,bool)));
    connect(m_jso, SIGNAL(jsoPlayerPause()), SIGNAL(jsCommandPlayerPause()));
    connect(m_jso, SIGNAL(jsoPlayerPlay()), SIGNAL(jsCommandPlayerPlay()));
    connect(m_jso, SIGNAL(jsoPlayerStop()), SIGNAL(jsCommandPlayerStop()));
    connect(page(), SIGNAL(windowCloseRequested()), SLOT(pageClose()));
    connect(page(), SIGNAL(linkClicked(QUrl)), SLOT(openLink(QUrl)));
    connect(page(), SIGNAL(loadFinished(bool)), SLOT(pageFinished(bool)));
    connect(page(), SIGNAL(frameCreated(QWebFrame*)), SLOT(singleFrameCreated(QWebFrame*)));

#ifndef QT_NO_OPENSSL
    connect(page()->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
            SLOT(namSslErrors(QNetworkReply*,QList<QSslError>)));
#endif
    connect(page()->networkAccessManager(), SIGNAL(finished(QNetworkReply*)),
            SLOT(namFinished(QNetworkReply*)));
}

QWebView *BrowserWebView::createWindow(QWebPage::WebWindowType)
{
    if(m_delegate_links) {
        if(!m_tiny) {
            m_tiny = new TinyBrowser(this);
            connect(m_tiny, SIGNAL(deleteUnneed()), SLOT(deleteTiny()));
        }
        return m_tiny;
    }
    else {
        page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAsNeeded);
        page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
        emit internalNavigation();
        return this;
    }
}

void BrowserWebView::showEvent(QShowEvent *e)
{
    m_jso->setIsVisible(true);
}

void BrowserWebView::hideEvent(QHideEvent *e)
{
    m_jso->setIsVisible(false);
}

void BrowserWebView::loadAdPage(QString url)
{
    QUrl url_str = QUrl(QUrl::fromPercentEncoding(url.toUtf8()));

    if(m_type == P2P_URL_NOTIFICATION || m_type == P2P_URL_SERVICE) {
        QFile html(":/loader");
        html.open(QIODevice::ReadOnly);
        setHtml(QString::fromUtf8(html.readAll().constData()));
    }

    m_loading_url = url_str;
    m_response_code = 0;
    load( url_str );
    //page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    //page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);

    if(m_type == P2P_URL_NOTIFICATION || m_type == P2P_URL_SERVICE) {
        emit showBrowser();
        show();
        setFocus(Qt::ActiveWindowFocusReason);
    }
}

void BrowserWebView::setDelegateLinks(bool val)
{
    m_delegate_links = val;
    page()->setLinkDelegationPolicy(m_delegate_links ? QWebPage::DelegateAllLinks : QWebPage::DontDelegateLinks );
}

void BrowserWebView::setIsInFullscreen(bool val)
{
    if(m_jso) {
        m_jso->setIsFullscreen(val);
    }
}

void BrowserWebView::setEmbedScript(const QString& embed_script)
{
    m_embed_script = QString(embed_script);
}

void BrowserWebView::setFlashEnabled(bool val)
{
    settings()->setAttribute(QWebSettings::PluginsEnabled, val);
}

void BrowserWebView::setCookiesPolicy(CookieJar::AcceptPolicy val)
{
    page()->networkAccessManager()->setCookieJar(CookieLoader::getInstanse()->createCookieJar(val));
}

void BrowserWebView::enableScrollBars(bool val)
{
    if(val) {
        page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAsNeeded);
        page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
    }
    else {
        page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
        page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    }
}

void BrowserWebView::enableJSDialogs(bool val)
{
    BrowserPage *p = qobject_cast<BrowserPage *>(page());
    p->setDialogsEnable(val);
}

void BrowserWebView::show()
{
    BrowserPage *p = qobject_cast<BrowserPage *>(page());
    p->setDialogCanShow(true);
    QWebView::show();
}

void BrowserWebView::close()
{
    BrowserPage *p = qobject_cast<BrowserPage *>(page());
    p->setDialogCanShow(false);
    QWebView::close();
}

void BrowserWebView::namFinished(QNetworkReply *reply)
{
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(reply->error() != QNetworkReply::NoError) {
        QString httpStatusMessage = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

        msg_P2PLog(m_intf, "[Browser] NAM errors: %s Status: %d Message: %s", qtu(reply->url().toString()), httpStatus, qtu(httpStatusMessage));
    }

    if(m_response_code == 0 || httpStatus >= 400)
        m_response_code = httpStatus;

    
    if( reply->url() == loadingUrl() ) {
        msg_P2PLog(m_intf, "[Browser] NAM finished loading %s", qtu(reply->url().toString()));
    }
}

void BrowserWebView::pageFinished(bool val)
{
    emit loadingFinished(val);
    msg_P2PLog(m_intf, "[Browser] Page %s loading status %d", qtu(loadingUrl().toString()), m_response_code);
    if(val) {
        page()->currentFrame()->addToJavaScriptWindowObject("AceStreamBrowser", m_jso);
        if(m_embed_script != "") {
            QString js = QString(ADD_JS_SCRIPT_JS).replace("%SCRIPT_URL%", m_embed_script);
            page()->currentFrame()->evaluateJavaScript(js);
        }
        //if(m_type == P2P_URL_NOTIFICATION || m_type == P2P_URL_SERVICE || m_type == P2P_URL_OVERLAY) {
        //}
    }
    else {
        m_loading_url = QUrl("about:blank");
        emit showMessageDialog();
    }
}

void BrowserWebView::pageClose()
{
    emit hideBrowser();
}

void BrowserWebView::openLink(QUrl url)
{
    if(m_delegate_links) {
        QDesktopServices::openUrl(url);
    }
    else {
        load(url);
        page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAsNeeded);
        page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
        emit internalNavigation();
    }
}

void BrowserWebView::deleteTiny()
{
    if(m_tiny)
        delete m_tiny;
    m_tiny = 0;
}

#ifndef QT_NO_OPENSSL
void BrowserWebView::namSslErrors(QNetworkReply *reply, QList<QSslError> error)
{
    QString replyHost = reply->url().host();
    QStringList errorStrings;
    for (int i = 0; i < error.count(); ++i)
        errorStrings += error.at(i).errorString();
    QString errors = errorStrings.join(QLatin1String("\n"));

    msg_P2PLog(m_intf, "[Browser] SSL error: %s %s %s", qtu(replyHost), qtu(reply->url().toString()), qtu(errors));

    foreach( QString s, m_sslTrustedHostList ) {
        if(replyHost.indexOf(s) != -1) {
            msg_P2PLog(m_intf, "[Browser] Ignore ssl errors fro this domain");
            reply->ignoreSslErrors();
            break;
        }
    }
}
#endif

void BrowserWebView::jsoLinkOpen(QString url, bool openInNewWindow)
{
    if(openInNewWindow) {
        emit jsCommandCloseFullscreen();
        QDesktopServices::openUrl(url);
    }
    else {
        openLink(QUrl(url));
    }
}

void BrowserWebView::singleFrameCreated(QWebFrame *frame)
{
    connect(frame, SIGNAL(loadFinished(bool)), SLOT(singleFrameLoadFinished(bool)));
}

void BrowserWebView::singleFrameLoadFinished(bool val)
{
    if(val) {
        QWebFrame *senderFrame = qobject_cast<QWebFrame*>(QObject::sender());
        if(senderFrame) {
            senderFrame->addToJavaScriptWindowObject("AceStreamBrowser", m_jso);
            if(m_embed_script != "") {
                QString js = QString(ADD_JS_SCRIPT_JS).replace("%SCRIPT_URL%", m_embed_script);
                senderFrame->evaluateJavaScript(js);
            }
        }
    }
}