#ifndef AWB_LOADITEM_H
#define AWB_LOADITEM_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "acewebbrowser/defines.hpp"

#include <QStringList>

namespace AceWebBrowser {

class LoadItem
{
public:
    LoadItem();
    LoadItem(BrowserType _type, QString _id, QString _url,
            int _w, int _h, int _l, int _t, int _r, int _b,
            bool _allowD, bool _enableF, BrowserCookies _cook, QStringList _embedS, QString _embedC,
            bool _preload,
            QString _contentT, QString _creativeT, QString _clickU,
            BrowserUserAgent _uA, int _cA, int _sT, bool _sH, bool _allowWO, int _group);
    LoadItem(const LoadItem &other);
    ~LoadItem();

    bool operator==(const LoadItem &other) const;
    bool operator!=(const LoadItem &other) const;

    BrowserType type() const;
    QString id() const;
    QString url() const;
    int width() const;
    int height() const;
    int left() const;
    int top() const;
    int right() const;
    int bottom() const;
    bool allowDialogs() const;
    bool allowWindowOpen() const;
    bool enableFlash() const;
    BrowserCookies cookies() const;
    QStringList embedScripts() const;
    QString embedCode() const;
    bool preload() const;
    QString contentType() const;
    QString creativeType() const;
    QString clickUrl() const;
    BrowserUserAgent userAgent() const;
    int closeAfterSeconds() const;
    int showTime() const;
    bool startHidden() const;

    QString hostUserAgent() const;
    unsigned int fixedBottomSpace() const;
    unsigned int fixedFullscreenBottomSpace() const;
    bool shownRegistered() const;
    bool hideRegistered() const;
    bool completeRegistered() const { return mEventCompleteRegistered; }

    void setHostUserAgent(const QString& ua);
    void setFixedBottomSpace(unsigned int space);
    void setFixedFullscreenBottomSpace(unsigned int space);
    void setShownRegistered(bool val);
    void setHideRegistered(bool val);
    void setCompleteRegistered(bool val) {
        mEventCompleteRegistered = val;
    }
    void clearEventFlags();

    void setSize(int w, int h);

    void clear();
    bool isCleared() const;

    void setEngineHttpHost(const QString&);
    void setEngineHttpPort(int);
    QString engineHttpHost() const;
    int engineHttpPort() const;

    QString toString() const;
    int groupId() const;

private:
    BrowserType mType;
    QString mId;
    QString mUrl;
    int mWidth;
    int mHeight;
    int mLeft;
    int mTop;
    int mRight;
    int mBottom;
    bool mAllowDialogs;
    bool mAllowWindowOpen;
    bool mEnableFlash;
    BrowserCookies mCookies;
    QStringList mEmbedScripts;
    QString mEmbedCode;
    bool mPreload;
    QString mContentType;
    QString mCreativeType;
    QString mClickUrl;
    BrowserUserAgent mUserAgent;
    int mCloseAfterSeconds;
    int mShowTime;
    bool mStartHidden;

    //additional
    QString mHostUserAgent;
    unsigned int mFixedBottomSpace;
    unsigned int mFixedFullscreenBottomSpace;
    bool mEventShownRegistered;
    bool mEventHideRegistered;
    bool mEventCompleteRegistered;
    
    QString mEngineHttpHost;
    int mEngineHttpPort;

    int mGroupId;
};

}

Q_DECLARE_METATYPE(AceWebBrowser::LoadItem)

QDebug operator<<(QDebug debug, const AceWebBrowser::LoadItem &item);

#endif // AWB_LOADITEM_H
