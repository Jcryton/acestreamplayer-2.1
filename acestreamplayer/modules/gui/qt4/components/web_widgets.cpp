#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "components/web_widgets.hpp"
#include "components/web_core.hpp"

#include "menus.hpp"

#include <QLayout>
#include <QToolBar>
#include <QAction>
#include <QStyle>
#include <QToolButton>
#include <QWebHistory>


AbstractBrowser::AbstractBrowser(intf_thread_t *intf, QWidget *parent) :
    QWidget(parent)
  , m_intf(intf)
  , m_wv(new BrowserWebView(intf, this))
  , m_navigation(0)
  , m_forward(0)
  , m_backward(0)
  , m_close(0)
  , m_spacer(0)
  , m_top(-1)
  , m_bottom(-1)
  , m_left(-1)
  , m_right(-1)
  , m_stretch_width(false)
  , m_stretch_height(false)
  , m_width(0)
  , m_height(0)
{
    setObjectName("AbstractBrowser");

    QPalette p = palette();
    p.setColor(QPalette::Window, QColor("#404040"));
    p.setColor(QPalette::WindowText, QColor("#000000"));
    setPalette(p);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);

    setupNavigationBar();
    layout->addWidget(m_navigation);
    layout->addWidget(m_wv);

    setLayout(layout);
    setVisible(false);

    connect(m_wv, SIGNAL(internalNavigation()), SLOT(resizeAndEnableToolbar()));
    connect(m_wv, SIGNAL(loadingFinished(bool)), SLOT(pageLoaded(bool)));
    connect(m_wv, SIGNAL(hideBrowser()), SLOT(hideBrowser()));
    connect(m_wv, SIGNAL(jsCommandCloseFullscreen()), SIGNAL(jsCommandCloseFullscreen()));
    connect(m_wv, SIGNAL(jsCommandPlayerPlay()), SIGNAL(jsCommandPlayerPlay()));
    connect(m_wv, SIGNAL(jsCommandPlayerPause()), SIGNAL(jsCommandPlayerPause()));
    connect(m_wv, SIGNAL(jsCommandPlayerStop()), SIGNAL(jsCommandPlayerStop()));
}

void AbstractBrowser::setDelegateLinks(bool val)
{
    m_wv->setDelegateLinks(val);
}

void AbstractBrowser::setViewSize(int width, int height)
{
    m_stretch_width = false;
    m_stretch_height = false;
    m_width = width;
    m_height = height;
    if(width) {
        setMinimumWidth(width);
        setMaximumWidth(width);
    }
    else {
        m_stretch_width = true;
        setMinimumWidth(parentWidget()->size().width());
        setMaximumWidth(parentWidget()->size().width());
    }
    if(height) {
        setMinimumHeight(height);
        setMaximumHeight(height);
    }
    else {
        m_stretch_height = true;
        setMinimumHeight(parentWidget()->size().height());
        setMaximumHeight(parentWidget()->size().height());
    }
}

void AbstractBrowser::contextMenuEvent( QContextMenuEvent *event )
{
    VLCMenuBar::PopupMenu( m_intf, true );
    event->accept();
}

void AbstractBrowser::updateViewSize(int left, int top, int right, int bottom)
{
    m_top = (top == -10) ? m_top : top;
    m_bottom = (bottom == -10) ? m_bottom : bottom;
    m_left = (left == -10) ? m_left : left;
    m_right = (right == -10) ? m_right : right;

    int pWidth = parentWidget()->size().width();
    int pHeight = parentWidget()->size().height();
    int x = -1; int y = -1;
    
    bool enable_scrollbars = false;
    if(width() >= pWidth || (pWidth > width() && m_stretch_width)) {
        setMaximumWidth(pWidth);
        setMinimumWidth(pWidth);
        enable_scrollbars = true;
    }

    if(height() >= pHeight || (pHeight > height() && m_stretch_height)) {
        if(m_bottom > 0) {
            setMaximumHeight(pHeight - m_bottom);
            setMinimumHeight(pHeight - m_bottom);
        }
        else {
            setMaximumHeight(pHeight);
            setMinimumHeight(pHeight);
        }
        enable_scrollbars &= true;
    }
    else {
        enable_scrollbars &= false;
    }

    setScrollbarsEnabled(enable_scrollbars);
    
    if( pWidth > width() && width() != m_width && !m_stretch_width) {
        setMaximumWidth(m_width);
        setMinimumWidth(m_width);
    }
    if( pHeight > height() && height() != m_height && !m_stretch_height) {
        setMaximumHeight(m_height);
        setMinimumHeight(m_height);
    }

    if(m_top >= 0 && m_bottom >= 0) {
        setMaximumHeight(pHeight - m_top - m_bottom);
        y = m_top;
    }
    else if(m_top >= 0) {
        y = m_top;
    }
    else if(m_bottom >= 0) {
        y = pHeight - maximumHeight() - m_bottom;
    }

    if(m_left >= 0 && m_right >= 0) {
        setMaximumWidth(pWidth - m_left - m_right);
        x = m_left;
    }
    else if(m_left >= 0) {
        x = m_left;
    }
    else if(m_right >= 0) {
        x = pWidth - m_right;
    }

    if(x == -1)
        x = (pWidth > maximumWidth()) ? (pWidth - maximumWidth()) / 2 : 0;
    if(y == -1)
        y = (pHeight > maximumHeight()) ? (pHeight - maximumHeight()) / 2 : 0;
    move(x, y);
}

void AbstractBrowser::setIsInFullscreenInJSO(bool val)
{
    m_wv->setIsInFullscreen(val);
}

void AbstractBrowser::setScrollbarsEnabled(bool val)
{
    m_wv->enableScrollBars(val);
}

void AbstractBrowser::setJSDialogsEnable(bool val)
{
    m_wv->enableJSDialogs(val);
}

void AbstractBrowser::setEmbedScript(const QString& val)
{
    m_wv->setEmbedScript(val);
}

void AbstractBrowser::setCookiesPolicy(int cp)
{
    m_wv->setCookiesPolicy((CookieJar::AcceptPolicy)cp);
}

void AbstractBrowser::setFlashEnable(bool val)
{
    m_wv->setFlashEnabled(val);
}

void AbstractBrowser::load(QString strurl)
{
    msg_P2PLog(m_intf, "Open page: %s", qtu(strurl));
    m_wv->loadAdPage(strurl);
}

QString AbstractBrowser::navigationBarButtonStyle(const QString &name, int width, int height, QString horalign, bool hasDisableState) {
    QString ss(
        "QToolButton {\n"
        "   border: none;\n"
        "   background: url(:/images/" + name + ") top " + horalign +" no-repeat;\n"
        "   margin: 0px;\n"
        "   width: " + QString::number(width) + "px;\n"
        "   height: " + QString::number(height) + "px;\n"
        "}\n"
        "QToolButton:hover {\n"
        "   background: url(:/images/" + name + "_h) top " + horalign +" no-repeat;\n"
        "}\n" );
    if(hasDisableState) {
        ss.append(
        "QToolButton:disabled {\n"
        "   background: url(:/images/" + name + "_d) top " + horalign +" no-repeat;\n"
        "}" );
    }
    return ss;
}

void AbstractBrowser::setupNavigationBar()
{
    if(m_navigation) {
        delete m_spacer;
        delete m_backward;
        delete m_forward;
        delete m_close;
        delete m_navigation;
    }

    m_navigation = new QToolBar(this);
    m_navigation->setStyleSheet("QToolBar { border: none; background-color: #434343; }");
    m_navigation->setMinimumHeight(36);
    m_navigation->setMaximumHeight(36);

    m_backward = new QAction(this);
    connect(m_backward, SIGNAL(triggered()), SLOT(backBrowser()));
    m_navigation->addAction(m_backward);
    QToolButton *backButton =(QToolButton*)m_navigation->widgetForAction(m_backward);
    if(backButton) {
        backButton->setStyleSheet(navigationBarButtonStyle("back", 42, 36));
        backButton->setToolTip(qtr("Back"));
        backButton->setEnabled(false);
    }

    m_forward = new QAction(this);
    connect(m_forward, SIGNAL(triggered()), SLOT(nextBrowser()));

    m_navigation->addAction(m_forward);
    QToolButton *nextButton =(QToolButton*)m_navigation->widgetForAction(m_forward);
    if(nextButton) {
        nextButton->setStyleSheet(navigationBarButtonStyle("next", 42, 36, "center"));
        nextButton->setToolTip(qtr("Next"));
        nextButton->setEnabled(false);
    }

    m_spacer = new QWidget(this);
    m_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_navigation->addWidget(m_spacer);

    m_close = new QAction(this);

    connect(m_close, SIGNAL(triggered()), SLOT(buttonHideBrowser()));
    m_navigation->addAction(m_close);
    QToolButton *closeButton =(QToolButton*)m_navigation->widgetForAction(m_close);
    if(closeButton) {
        closeButton->setStyleSheet(navigationBarButtonStyle("close", 70, 36, "right", false));
        closeButton->setToolTip(qtr("Close"));
    }

    m_navigation->setVisible(false);
}

void AbstractBrowser::enableHistory(QAction *action, bool value) {
    QToolButton *button =(QToolButton*)m_navigation->widgetForAction(action);
    if(button) {
        button->setEnabled(value);
    }
}

void AbstractBrowser::showBrowser()
{
    show();
    m_wv->show();
    setFocus(Qt::ActiveWindowFocusReason);
    raise();
    emit raiseControls();
}

void AbstractBrowser::hideBrowser()
{
    enableHistory(m_backward, false);
    enableHistory(m_forward, false);
    m_navigation->close();
    m_navigation->hide();
    hide();
    if(!parentWidget()->hasFocus())
        parentWidget()->setFocus(Qt::ActiveWindowFocusReason);

    m_wv->setHtml("<html></html>");
    m_wv->setLoadingUrl(QUrl("about:blank"));
    m_wv->close();
}

void AbstractBrowser::clear()
{
    m_wv->setHtml("<html></html>");
    m_wv->setLoadingUrl(QUrl("about:blank"));
    m_wv->close();
}

void AbstractBrowser::resizeAndEnableToolbar()
{
    m_navigation->show();
    int pWidth = parentWidget()->size().width();
    int pHeight = parentWidget()->size().height();
    if(m_bottom > 0) {
        pHeight -= m_bottom;
    }
    setMinimumSize(pWidth, pHeight);
    setMaximumSize(pWidth, pHeight);
    move(0,0);
}

void AbstractBrowser::pageLoaded(bool loaded)
{
    if(!loaded) {
        msg_P2PLog(m_intf, "Browser page not loaded");
    }
    enableHistory(m_backward, m_wv->history()->canGoBack());
    enableHistory(m_forward, m_wv->history()->canGoForward());
}

void AbstractBrowser::backBrowser()
{
    m_wv->page()->triggerAction(QWebPage::Back);
}

void AbstractBrowser::nextBrowser()
{
    m_wv->page()->triggerAction(QWebPage::Forward);
}

void AbstractBrowser::buttonHideBrowser()
{
    hideBrowser();
    emit jsCommandPlayerUnPause();
}

/*
 * InteractivePausePreloadBrowser
 */
CommonInTimeBrowser::CommonInTimeBrowser(intf_thread_t *intf, QWidget *parent) :
    AbstractBrowser(intf, parent)
  , m_enabled(false)
{
    connect(m_wv, SIGNAL(showBrowser()), SLOT(showBrowser()));
    connect(m_wv, SIGNAL(showMessageDialog()), SIGNAL(showLoadFailDialog()));
}

void CommonInTimeBrowser::load(QString strurl)
{
    AbstractBrowser::load(strurl);
    m_enabled = true;
}

int CommonInTimeBrowser::bottom() const
{
    return m_bottom;
}

void CommonInTimeBrowser::setAdType(int val)
{
    if(m_wv)
        m_wv->setType(val);
}

int CommonInTimeBrowser::getAdType()
{
    return m_wv ? m_wv->getType() : -1;
}

bool CommonInTimeBrowser::enabled()
{
    return m_enabled;
}

void CommonInTimeBrowser::showBrowser()
{
    AbstractBrowser::showBrowser();
}

void CommonInTimeBrowser::hideBrowser()
{
    if(isVisible()) {
        m_enabled = false;
        AbstractBrowser::hideBrowser();
    }
}

void CommonInTimeBrowser::pageLoaded(bool loaded)
{
    AbstractBrowser::pageLoaded(loaded);
    if(loaded) {
        if(m_enabled) {
            AbstractBrowser::showBrowser();
        }
    }
}

/*
 * InteractivePausePreloadBrowser
 */
InteractivePausePreloadBrowser::InteractivePausePreloadBrowser(intf_thread_t *intf, QWidget *parent) :
    AbstractBrowser(intf, parent)
  , m_has_page(false)
  , m_preload_id("")
{
    m_wv->setType(P2P_URL_AD);
}

void InteractivePausePreloadBrowser::load(QString strurl, QString id)
{
    m_has_page = false;
    m_preload_id = id;
    AbstractBrowser::load(strurl);
}

bool InteractivePausePreloadBrowser::isAlreadyLoaded(QString url, QString id)
{
    return ready() && m_preload_id == id && m_has_page && url == m_wv->url().toString();
}

bool InteractivePausePreloadBrowser::isAlreadyLoading(QString url, QString id)
{
    return m_preload_id == id && url == m_wv->loadingUrl().toString();
}

int InteractivePausePreloadBrowser::bottom() const
{
    return m_bottom;
}

bool InteractivePausePreloadBrowser::ready() const
{
    return m_has_page && m_preload_id != "";
}

void InteractivePausePreloadBrowser::showBrowser()
{
    if(m_has_page && m_preload_id != "") {
        AbstractBrowser::showBrowser();
        emit registerShowing(m_preload_id);
    }
}

void InteractivePausePreloadBrowser::hideBrowser()
{
    if(isVisible()) {
        m_has_page = false;
        m_preload_id = "";
        AbstractBrowser::hideBrowser();
    }
}

void InteractivePausePreloadBrowser::clear()
{
    m_has_page = false;
    m_preload_id = "";
    AbstractBrowser::clear();
}

void InteractivePausePreloadBrowser::pageLoaded(bool loaded)
{
    AbstractBrowser::pageLoaded(loaded);
    if(loaded) {
        m_has_page = true;
    }
}

/*
 * OverlayPreloadAdBrowser
 */
OverlayPreloadAdBrowser::OverlayPreloadAdBrowser(intf_thread_t *intf, QWidget *parent) :
    AbstractBrowser(intf, parent)
  , m_closed(false)
  , m_preload_id("")
  , m_has_page(false)
{
    m_wv->setType(P2P_URL_OVERLAY);
    connect(m_wv, SIGNAL(hideBrowser()), SLOT(closeNonLinear()));
}

void OverlayPreloadAdBrowser::load(QString strurl, QString id)
{
    m_has_page = false;
    m_preload_id = id;
    m_closed = false;
    AbstractBrowser::load(strurl);
}

int OverlayPreloadAdBrowser::bottom() const
{
    return m_bottom;
}

bool OverlayPreloadAdBrowser::isAlreadyLoading(QString url, QString id)
{
    return m_preload_id == id && url == m_wv->loadingUrl().toString();
}

bool OverlayPreloadAdBrowser::ready() const
{
    return m_has_page && m_preload_id != "" && !m_closed;
}

void OverlayPreloadAdBrowser::showBrowser()
{
    if(m_has_page && m_preload_id != "" && !m_closed) {
        AbstractBrowser::showBrowser();
        emit registerShowing(m_preload_id);
    }
}

void OverlayPreloadAdBrowser::tempHideBrowser()
{
    if(isVisible()) {
        hide();
    }
}

void OverlayPreloadAdBrowser::clear()
{
    m_has_page = false;
    m_preload_id = "";
    m_closed = true;
    AbstractBrowser::clear();
}

void OverlayPreloadAdBrowser::hideBrowser()
{
    m_has_page = false;
    m_preload_id = "";
    m_closed = true;
    AbstractBrowser::hideBrowser();
}

void OverlayPreloadAdBrowser::pageLoaded(bool loaded)
{
    AbstractBrowser::pageLoaded(loaded);
    if(loaded) {
        m_has_page = true;
        emit overlayLoaded();
    }
}

void OverlayPreloadAdBrowser::closeNonLinear()
{
    emit closeNonLinearBrowser(m_preload_id);
}
