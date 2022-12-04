#include "QCefQMLViewPrivate.h"

#include "QCefSetting.h"
#include "QCefSettingPrivate.h"
#include <QCefQMLView.h>
#include <QPainter>
#include <QWindow>

QCefQMLViewPrivate::QCefQMLViewPrivate(QCefContextPrivate* ctx,
                                       QCefQMLView* view,
                                       const QString& url,
                                       const QCefSetting* setting)
  : pContextPrivate_(ctx)
  , q_ptr(view)
{}

QCefQMLViewPrivate ::~QCefQMLViewPrivate() {}

void
QCefQMLViewPrivate::onCefMainBrowserCreated(CefRefPtr<CefBrowser>& browser, QWindow* window)
{
  pCefBrowser_ = browser;

  // notify the visibility and size
  pCefBrowser_->GetHost()->WasHidden(!q_ptr->isVisible());
  pCefBrowser_->GetHost()->WasResized();
  connect(this, SIGNAL(updateOsrFrame()), q_ptr, SLOT(update()));

  // monitor the screenChanged signal from the top-level window
  disconnect(this, SLOT(onViewScreenChanged(QScreen*)));
  // if (q_ptr->window()->windowHandle()) {
  //  connect(q_ptr->window()->windowHandle(),    //
  //          SIGNAL(screenChanged(QScreen*)),    //
  //          this,                               //
  //          SLOT(onViewScreenChanged(QScreen*)) //
  //  );
  //}
}

void
QCefQMLViewPrivate::onCefPopupBrowserCreated(CefRefPtr<CefBrowser>& browser, QWindow* window)
{
  q_ptr->onPopupCreated(window);
}

void
QCefQMLViewPrivate::createCefBrowser(QCefQMLView* view, const QString url, const QCefSetting* setting)
{
  auto pClientDelegate = std::make_shared<CCefQMLClientDelegate>(this);
  auto pClient = new CefViewBrowserClient(pContextPrivate_->getCefApp(), pClientDelegate);

  CefWindowInfo window_info;
  window_info.SetAsWindowless(0);

  CefBrowserSettings browserSettings;
  QCefSettingPrivate::CopyToCefBrowserSettings(setting, &browserSettings);
  // TODO by sl 透明绘制

  // create browser object
  bool success = CefBrowserHost::CreateBrowser(window_info,       // window info
                                               pClient,           // handler
                                               url.toStdString(), // url
                                               browserSettings,   // settings
                                               nullptr,
                                               CefRequestContext::GetGlobalContext());
  Q_ASSERT_X(success, "QCefViewPrivate::createBrowser", "Failed to create cef browser");
  if (!success) {
    qWarning("Failed to create cef browser");
    return;
  }

  pClient_ = pClient;
  pClientDelegate_ = pClientDelegate;
  return;
}

void
QCefQMLViewPrivate::destroyCefBrowser()
{
  if (!pClient_)
    return;
  pClient_->CloseAllBrowsers();
  pClient_ = nullptr;
  pCefBrowser_ = nullptr;
}

void
QCefQMLViewPrivate::onOsrUpdatePopupFrame(const QImage& frame, const QRegion& region)
{
  if (osr.qCefPopupFrame_.size() == frame.size()) {
    QMutexLocker lock(&(osr.qPopupPaintLock_));
    // update region
    QPainter painter(&osr.qCefPopupFrame_);
    for (auto& rc : region) {
      painter.drawImage(rc, frame, rc);
    }
  } else {
    QMutexLocker lock(&(osr.qPopupPaintLock_));
    osr.qCefPopupFrame_ = frame.copy();
  }
  emit updateOsrFrame();
}

void
QCefQMLViewPrivate::onOsrUpdateViewFrame(const QImage& frame, const QRegion& region)
{
  if (osr.qCefViewFrame_.size() != frame.size() || osr.transparentPaintingEnabled) {
    // update full image
    QMutexLocker lock(&(osr.qViewPaintLock_));
    osr.qCefViewFrame_ = frame.copy();
  } else {
    QMutexLocker lock(&(osr.qViewPaintLock_));
    // update only dirty regions
    QPainter painter(&osr.qCefViewFrame_);
    for (auto& rc : region) {
      painter.drawImage(rc, frame, rc);
    }
  }
  emit updateOsrFrame();
}

void
QCefQMLViewPrivate::onViewWheelEvent(QWheelEvent* event)
{
  auto p = event->position();
  auto d = event->angleDelta();
  auto m = event->modifiers();
  auto b = event->buttons();

  CefMouseEvent e;
  e.modifiers |= m & Qt::ControlModifier ? EVENTFLAG_CONTROL_DOWN : 0;
  e.modifiers |= m & Qt::ShiftModifier ? EVENTFLAG_SHIFT_DOWN : 0;
  e.modifiers |= m & Qt::AltModifier ? EVENTFLAG_ALT_DOWN : 0;
  e.modifiers |= b & Qt::LeftButton ? EVENTFLAG_LEFT_MOUSE_BUTTON : 0;
  e.modifiers |= b & Qt::RightButton ? EVENTFLAG_RIGHT_MOUSE_BUTTON : 0;
  e.modifiers |= b & Qt::MiddleButton ? EVENTFLAG_MIDDLE_MOUSE_BUTTON : 0;

  e.x = p.x();
  e.y = p.y();
  pCefBrowser_->GetHost()->SendMouseWheelEvent(e, m & Qt::ShiftModifier ? 0 : d.x(), m & Qt::ShiftModifier ? 0 : d.y());
}

void
QCefQMLViewPrivate::onViewMouseEvent(QMouseEvent* event)
{
    if (!pCefBrowser_)
      return;

    auto b = event->buttons();
    auto m = event->modifiers();

    CefMouseEvent e;
    e.x = ((QMouseEvent*)event)->pos().x();
    e.y = ((QMouseEvent*)event)->pos().y();
    e.modifiers |= m & Qt::ControlModifier ? EVENTFLAG_CONTROL_DOWN : 0;
    e.modifiers |= m & Qt::ShiftModifier ? EVENTFLAG_SHIFT_DOWN : 0;
    e.modifiers |= m & Qt::AltModifier ? EVENTFLAG_ALT_DOWN : 0;
    e.modifiers |= b & Qt::LeftButton ? EVENTFLAG_LEFT_MOUSE_BUTTON : 0;
    e.modifiers |= b & Qt::RightButton ? EVENTFLAG_RIGHT_MOUSE_BUTTON : 0;
    e.modifiers |= b & Qt::MiddleButton ? EVENTFLAG_MIDDLE_MOUSE_BUTTON : 0;

    if (QEvent::MouseMove == event->type()) {
      pCefBrowser_->GetHost()->SendMouseMoveEvent(e, false);
      return;
    }

    // qDebug() << "====== onViewMouseEvent:" << event;

    CefBrowserHost::MouseButtonType mbt = MBT_LEFT;
    switch (event->button()) {
      case Qt::LeftButton: {
        mbt = MBT_LEFT;
      } break;
      case Qt::RightButton: {
        mbt = MBT_RIGHT;
      } break;
      case Qt::MiddleButton: {
        mbt = MBT_MIDDLE;
      } break;
      default:
        break;
    }

    if (QEvent::MouseButtonPress == event->type()) {
      pCefBrowser_->GetHost()->SendMouseClickEvent(e, mbt, false, 1);
    } else if (QEvent::MouseButtonDblClick == event->type()) {
      pCefBrowser_->GetHost()->SendMouseClickEvent(e, mbt, false, 2);
    } else if (QEvent::MouseButtonRelease == event->type()) {
      // if the release was generated right after a popup, we must discard it
      pCefBrowser_->GetHost()->SendMouseClickEvent(e, mbt, true, 1);
    }  
}
