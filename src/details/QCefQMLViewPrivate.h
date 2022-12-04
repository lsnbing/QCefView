#pragma once

#include <QObject>
#include <QMutex>
#include <QMenu>

#include "CCefQMLClientDelegate.h"
#include "CefViewBrowserClient.h"
#include "QCefContextPrivate.h"

class QCefSetting;
class QCefQMLView;
class QWindow;

class QCefQMLViewPrivate : public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(QCefQMLView)
  QCefQMLView* q_ptr;

  friend class CCefQMLClientDelegate;

  public:
  struct OsrPrivateData
  {
    /// <summary>
    ///
    /// </summary>
    bool transparentPaintingEnabled = false;

    /// <summary>
    ///
    /// </summary>
    bool showPopup_ = false;

    /// <summary>
    ///
    /// </summary>
    QRect qPopupRect_;

    /// <summary>
    ///
    /// </summary>
    QRect qImeCursorRect_;

    /// <summary>
    ///
    /// </summary>
    QMutex qViewPaintLock_;

    /// <summary>
    ///
    /// </summary>
    QImage qCefViewFrame_;

    /// <summary>
    ///
    /// </summary>
    QMutex qPopupPaintLock_;

    /// <summary>
    ///
    /// </summary>
    QImage qCefPopupFrame_;

    /// <summary>
    ///
    /// </summary>
    bool isShowingContextMenu_ = false;

    /// <summary>
    ///
    /// </summary>
    QMenu* contextMenu_ = nullptr;

    /// <summary>
    ///
    /// </summary>
    CefRefPtr<CefRunContextMenuCallback> contextMenuCallback_;
  } osr;

public:
  explicit QCefQMLViewPrivate(QCefContextPrivate* ctx,
                              QCefQMLView* view,
                              const QString& url,
                              const QCefSetting* setting = nullptr);

  ~QCefQMLViewPrivate();

  void createCefBrowser(QCefQMLView* view, const QString url, const QCefSetting* setting);

  void destroyCefBrowser();

  void onCefMainBrowserCreated(CefRefPtr<CefBrowser>& browser, QWindow* window);
  void onCefPopupBrowserCreated(CefRefPtr<CefBrowser>& browser, QWindow* window);
  void onOsrUpdatePopupFrame(const QImage& frame, const QRegion& region);
  void onOsrUpdateViewFrame(const QImage& frame, const QRegion& region);

  void onViewWheelEvent(QWheelEvent* event);
  void onViewMouseEvent(QMouseEvent* event);


signals:
  void updateOsrFrame();

public:
  CefRefPtr<CefBrowser> pCefBrowser_ = nullptr;
  QCefContextPrivate* pContextPrivate_ = nullptr;
  CefRefPtr<CefViewBrowserClient> pClient_ = nullptr;
  CCefQMLClientDelegate::RefPtr pClientDelegate_ = nullptr;
};
