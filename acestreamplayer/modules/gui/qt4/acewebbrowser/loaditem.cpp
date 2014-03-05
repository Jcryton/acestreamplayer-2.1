#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "acewebbrowser/loaditem.hpp"

using namespace AceWebBrowser;

QDebug operator<<(QDebug debug, const LoadItem &item)
{
    debug.nospace() << "LoadItem:"
        << "\r  Type: " << (int)item.type()
        << "\r  ID: " << item.id()
        << "\r  Url: " << item.url()
        << "\r  Width: " << item.width()
        << "\r  Height: " << item.height()
        << "\r  Left: " << item.left()
        << "\r  Top: " << item.top()
        << "\r  Right: " << item.right()
        << "\r  Bottom: " << item.bottom()
        << "\r  AllowDialogs: " << item.allowDialogs()
        << "\r  EnableFlash: " << item.enableFlash()
        << "\r  Cookies: " << (int)item.cookies()
        << "\r  EmbedScripts: " << item.embedScripts()
        << "\r  EmbedCode: " << item.embedCode()
        << "\r  Preload: " << item.preload()
        << "\r  UserAgent: " << (int)item.userAgent()
        << "\r  CloseAfter: " << item.closeAfterSeconds();
    return debug.space();
}

LoadItem::LoadItem()
{
}

LoadItem::LoadItem(BrowserType _type, QString _id, QString _url,
                                 int _w, int _h, int _l, int _t, int _r, int _b,
                                 bool _allowD, bool _enableF, BrowserCookies _cook, QStringList _embedS, QString _embedC,
                                 bool _preload,
                                 QString _contentT, QString _creativeT, QString _clickU,
                                 BrowserUserAgent _uA, int _cA)
    : mType(_type)
    , mId(_id)
    , mUrl(_url)
    , mWidth(_w)
    , mHeight(_h)
    , mLeft(_l)
    , mTop(_t)
    , mRight(_r)
    , mBottom(_b)
    , mAllowDialogs(_allowD)
    , mEnableFlash(_enableF)
    , mCookies(_cook)
    , mEmbedScripts(_embedS)
    , mEmbedCode(_embedC)
    , mPreload(_preload)
    , mContentType(_contentT)
    , mCreativeType(_creativeT)
    , mClickUrl(_clickU)
    , mUserAgent(_uA)
    , mHostUserAgent("")
    , mFixedBottomSpace(0)
    , mFixedFullscreenBottomSpace(0)
    , mEventShownRegistered(false)
    , mEventHideRegistered(false)
    , mEventCompleteRegistered(false)
    , mCloseAfterSeconds(_cA)
{
}

LoadItem::LoadItem(const LoadItem &other)
    : mType(other.type())
    , mId(other.id())
    , mUrl(other.url())
    , mWidth(other.width())
    , mHeight(other.height())
    , mLeft(other.left())
    , mTop(other.top())
    , mRight(other.right())
    , mBottom(other.bottom())
    , mAllowDialogs(other.allowDialogs())
    , mEnableFlash(other.enableFlash())
    , mCookies(other.cookies())
    , mEmbedScripts(other.embedScripts())
    , mEmbedCode(other.embedCode())
    , mPreload(other.preload())
    , mContentType(other.contentType())
    , mCreativeType(other.creativeType())
    , mClickUrl(other.clickUrl())
    , mUserAgent(other.userAgent())
    , mHostUserAgent(other.hostUserAgent())
    , mFixedBottomSpace(other.fixedBottomSpace())
    , mFixedFullscreenBottomSpace(other.fixedFullscreenBottomSpace())
    , mEventShownRegistered(false)
    , mEventHideRegistered(false)
    , mEventCompleteRegistered(false)
    , mCloseAfterSeconds(other.closeAfterSeconds())
{
}

LoadItem::~LoadItem()
{
}

bool LoadItem::operator ==(const LoadItem &other) const
{
    return type()==other.type()
            && !id().compare(other.id())
            && !url().compare(other.url());
}

bool LoadItem::operator !=(const LoadItem &other) const
{
    return type()!=other.type()
            || id().compare(other.id())
            || url().compare(other.url());
}

BrowserType LoadItem::type() const
{
    return mType;
}

QString LoadItem::id() const
{
    return mId;
}

QString LoadItem::url() const
{
    return mUrl;
}

int LoadItem::width() const
{
    return mWidth;
}

int LoadItem::height() const
{
    return mHeight;
}

int LoadItem::left() const
{
    return mLeft;
}

int LoadItem::top() const
{
    return mTop;
}

int LoadItem::right() const
{
    return mRight;
}

int LoadItem::bottom() const
{
    return mBottom;
}

bool LoadItem::allowDialogs() const
{
    return mAllowDialogs;
}

bool LoadItem::enableFlash() const
{
    return mEnableFlash;
}

BrowserCookies LoadItem::cookies() const
{
    return mCookies;
}

QStringList LoadItem::embedScripts() const
{
    return mEmbedScripts;
}

QString LoadItem::embedCode() const
{
    return mEmbedCode;
}

bool LoadItem::preload() const
{
    return mPreload;
}

QString LoadItem::contentType() const
{
    return mContentType;
}

QString LoadItem::creativeType() const
{
    return mCreativeType;
}

QString LoadItem::clickUrl() const
{
    return mClickUrl;
}

BrowserUserAgent LoadItem::userAgent() const
{
    return mUserAgent;
}

void LoadItem::setSize(int w, int h)
{
    mWidth = w;
    mHeight = h;
}

void LoadItem::clear()
{
    mType = BTYPE_UNDEFINED;
    mId = "";
    mUrl = "";
    mEmbedScripts.clear();
    mEmbedCode = "";
    mContentType = "";
    mCreativeType = "";
    mClickUrl = "";
}

bool LoadItem::isCleared() const
{
    return mType == BTYPE_UNDEFINED && mId == "" && mUrl == "";
}

QString AceWebBrowser::LoadItem::hostUserAgent() const
{
    return mHostUserAgent;
}

unsigned int LoadItem::fixedBottomSpace() const
{
    return mFixedBottomSpace;
}

unsigned int LoadItem::fixedFullscreenBottomSpace() const
{
    return mFixedFullscreenBottomSpace;
}

bool LoadItem::shownRegistered() const
{
    return mEventShownRegistered;
}

bool LoadItem::hideRegistered() const
{
    return mEventHideRegistered;
}

void LoadItem::setHostUserAgent(const QString &ua)
{
    mHostUserAgent = ua;
}

void LoadItem::setFixedBottomSpace(unsigned int space)
{
    mFixedBottomSpace = space;
}

void LoadItem::setFixedFullscreenBottomSpace(unsigned int space)
{
    mFixedFullscreenBottomSpace = space;
}

void LoadItem::setShownRegistered(bool val)
{
    mEventShownRegistered = val;
}

void LoadItem::setHideRegistered(bool val)
{
    mEventHideRegistered = val;
}

void LoadItem::clearEventFlags()
{
    setShownRegistered(false);
    setHideRegistered(false);
}

int LoadItem::closeAfterSeconds() const
{
    return mCloseAfterSeconds;
}
