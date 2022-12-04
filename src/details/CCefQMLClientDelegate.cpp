#include "CCefQMLClientDelegate.h"

#include <QApplication>
#include <QImage>
#include <QScreen>
#include <QSizeF>
#include <QThread>

//#include "QCefSettingPrivate.h"
#include "QCefQMLView.h"
#include "QCefQMLViewPrivate.h"
#include "QCefSettingPrivate.h"
#include "utils/ValueConvertor.h"

CCefQMLClientDelegate::CCefQMLClientDelegate(QCefQMLViewPrivate* p)
  : pCefViewPrivate_(p)
{}

CCefQMLClientDelegate::~CCefQMLClientDelegate()
{
  return;
}

void
CCefQMLClientDelegate::processUrlRequest(const std::string& url)
{
  // deprecated feature
  // auto view = take(browser);
  // if (view) {
  //  auto u = QString::fromStdString(url);
  //  view->onQCefUrlRequest(u);
  //}
}

void
CCefQMLClientDelegate::processQueryRequest(CefRefPtr<CefBrowser>& browser,
                                           int64_t frameId,
                                           const std::string& request,
                                           const int64_t query_id)
{
  if (!QMLIsValidBrowser(browser))
    return;

  auto browserId = browser->GetIdentifier();
  auto req = QString::fromStdString(request);
  pCefViewPrivate_->q_ptr->cefQueryRequest(browserId, frameId, QCefQuery(req, query_id));
}

void
CCefQMLClientDelegate::focusedEditableNodeChanged(CefRefPtr<CefBrowser>& browser,
                                                  int64_t frameId,
                                                  bool focusOnEditableNode)
{
  if (!QMLIsValidBrowser(browser))
    return;

  QMetaObject::invokeMethod(pCefViewPrivate_, "onCefInputStateChanged", Q_ARG(bool, focusOnEditableNode));
}

void
CCefQMLClientDelegate::invokeMethodNotify(CefRefPtr<CefBrowser>& browser,
                                          int64_t frameId,
                                          const std::string& method,
                                          const CefRefPtr<CefListValue>& arguments)
{
  if (!QMLIsValidBrowser(browser))
    return;

  auto m = QString::fromStdString(method);
  QVariantList args;
  for (int i = 0; i < arguments->GetSize(); i++) {
    QVariant qV;
    auto cV = arguments->GetValue(i);
    ValueConvertor::CefValueToQVariant(&qV, cV.get());
    args.push_back(qV);
  }

  auto browserId = browser->GetIdentifier();
  pCefViewPrivate_->q_ptr->invokeMethod(browserId, frameId, m, args);
}

void
CCefQMLClientDelegate::reportJSResult(CefRefPtr<CefBrowser>& browser,
                                      int64_t frameId,
                                      int64_t contextId,
                                      const CefRefPtr<CefValue>& result)
{
  if (!QMLIsValidBrowser(browser))
    return;

  auto browserId = browser->GetIdentifier();
  QVariant qV;
  ValueConvertor::CefValueToQVariant(&qV, result.get());
  pCefViewPrivate_->q_ptr->reportJavascriptResult(browserId, frameId, contextId, qV);
}

// -----------------Render

bool
CCefQMLClientDelegate::getRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
  if (!QMLIsValidBrowser(browser)) {
    return false;
  }

  // get the screen which the view is currently residing in
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QScreen* currentScreen = pCefViewPrivate_->q_ptr->screen();
#else
  QWidget* ancestorWidget = pCefViewPrivate_->q_ptr->window();
  QWindow* ancestorWindow = ancestorWidget ? ancestorWidget->windowHandle() : nullptr;
  QScreen* currentScreen = ancestorWindow ? ancestorWindow->screen() : nullptr;
#endif

  if (!currentScreen) {
    // the view is not visible so we retrieve the main screen info
    currentScreen = QApplication::screens().at(0);
  }

  QRect rcScreen = currentScreen->geometry();

  // qDebug() << "CCefClientDelegate::GetRootScreenRect:" << rcScreen;

  rect.Set(rcScreen.x(), rcScreen.y(), rcScreen.width(), rcScreen.height());
  return true;
}
void
CCefQMLClientDelegate::getViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
  if (!QMLIsValidBrowser(browser)) {
    rect.Set(0, 0, 1, 1);
    return;
  }

  QSizeF rcSize = pCefViewPrivate_->q_ptr->size();

  // qDebug() << "CCefClientDelegate::GetViewRect:" << QRect(ptWindow, rcSize);

  rect.Set(0, 0, rcSize.width() ? rcSize.width() : 1, rcSize.height() ? rcSize.height() : 1);
}

bool
CCefQMLClientDelegate::getScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY)

{
  if (!QMLIsValidBrowser(browser))
    return false;

  // QPoint ptScreen = pCefViewPrivate_->q_ptr->mapToGlobal(QPoint(viewX, viewY));
  // screenX = ptScreen.x();
  // screenY = ptScreen.y();
  // TODO by sl

  screenX = 0;
  screenY = 0;
  return true;
}
bool
CCefQMLClientDelegate::getScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
  if (!QMLIsValidBrowser(browser))
    return false;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QScreen* currentScreen = pCefViewPrivate_->q_ptr->screen();
#else
  QWidget* ancestorWidget = pCefViewPrivate_->q_ptr->window();
  QWindow* ancestorWindow = ancestorWidget ? ancestorWidget->windowHandle() : nullptr;
  QScreen* currentScreen = ancestorWindow ? ancestorWindow->screen() : nullptr;
#endif

  if (!currentScreen) {
    // the view is not visible so we retrieve the main screen info
    currentScreen = QApplication::screens().at(0);
  }

  QRect rect = currentScreen->geometry();
  QRect availableRect = currentScreen->availableGeometry();
  screen_info.Set(currentScreen->devicePixelRatio(),                                                      //
                  currentScreen->depth(),                                                                 //
                  0,                                                                                      //
                  false,                                                                                  //
                  { rect.x(), rect.y(), rect.width(), rect.height() },                                    //
                  { availableRect.x(), availableRect.y(), availableRect.width(), availableRect.height() } //
  );

  return true;
}
void
CCefQMLClientDelegate::onPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{}

void
CCefQMLClientDelegate::onPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{}
void
CCefQMLClientDelegate::onPaint(CefRefPtr<CefBrowser> browser,
                               CefRenderHandler::PaintElementType type,
                               const CefRenderHandler::RectList& dirtyRects,
                               const void* buffer,
                               int width,
                               int height)
{
  if (!QMLIsValidBrowser(browser))
    return;

  QImage frame;
  QRegion region;

  frame = QImage(static_cast<const uchar*>(buffer), width, height, QImage::Format_ARGB32_Premultiplied);
  for (auto& rect : dirtyRects) {
    region += QRect{ rect.x, rect.y, rect.width, rect.height };
  }

  if (PET_VIEW == type) {
    pCefViewPrivate_->onOsrUpdateViewFrame(frame, region);
  } else if (PET_POPUP == type) {
    pCefViewPrivate_->onOsrUpdatePopupFrame(frame, region);
  } else {
  }
}

void
CCefQMLClientDelegate::onAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                                          CefRenderHandler::PaintElementType type,
                                          const CefRenderHandler::RectList& dirtyRects,
                                          void* shared_handle)
{}
bool
CCefQMLClientDelegate::startDragging(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefDragData> drag_data,
                                     CefRenderHandler::DragOperationsMask allowed_ops,
                                     int x,
                                     int y)
{
  return false;
}
void
CCefQMLClientDelegate::updateDragCursor(CefRefPtr<CefBrowser> browser, CefRenderHandler::DragOperation operation)
{}
void
CCefQMLClientDelegate::onScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y)
{}
void
CCefQMLClientDelegate::onImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser,
                                                    const CefRange& selected_range,
                                                    const CefRenderHandler::RectList& character_bounds)
{}
void
CCefQMLClientDelegate::onTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                                              const CefString& selected_text,
                                              const CefRange& selected_range)
{}
void
CCefQMLClientDelegate::onVirtualKeyboardRequested(CefRefPtr<CefBrowser> browser,
                                                  CefRenderHandler::TextInputMode input_mode)
{}

// LifSpanHandler
bool
CCefQMLClientDelegate::onBeforePopup(CefRefPtr<CefBrowser>& browser,
                                     int64_t frameId,
                                     const std::string& targetUrl,
                                     const std::string& targetFrameName,
                                     CefLifeSpanHandler::WindowOpenDisposition targetDisposition,
                                     CefWindowInfo& windowInfo,
                                     CefBrowserSettings& settings,
                                     bool& DisableJavascriptAccess)
{
  bool result = false;
  if (pCefViewPrivate_) {
    auto url = QString::fromStdString(targetUrl);
    auto name = QString::fromStdString(targetFrameName);

    QCefSetting s;
    QCefQMLView::CefWindowOpenDisposition d = (QCefQMLView::CefWindowOpenDisposition)targetDisposition;
    QCefSettingPrivate::CopyFromCefBrowserSettings(&s, &settings);

    Qt::ConnectionType c = pCefViewPrivate_->q_ptr->thread() == QThread::currentThread() ? Qt::DirectConnection
                                                                                         : Qt::BlockingQueuedConnection;
    QMetaObject::invokeMethod(pCefViewPrivate_->q_ptr,
                              "onBeforePopup",                                 //
                              c,                                               //
                              Q_RETURN_ARG(bool, result),                      //
                              Q_ARG(qint64, frameId),                          //
                              Q_ARG(const QString&, url),                      //
                              Q_ARG(const QString&, name),                     //
                              Q_ARG(QCefQMLView::CefWindowOpenDisposition, d), //
                              Q_ARG(QCefSetting&, s),                          //
                              Q_ARG(bool&, DisableJavascriptAccess)            //
    );
    QCefSettingPrivate::CopyToCefBrowserSettings(&s, &settings);
  }
  return result;
}

void
CCefQMLClientDelegate::onAfterCreate(CefRefPtr<CefBrowser>& browser)
{
  if (!pCefViewPrivate_)
    return;

  QWindow* w = nullptr;

  Qt::ConnectionType c = Qt::BlockingQueuedConnection;

  QMetaObject::invokeMethod(
    pCefViewPrivate_,
    [=]() {
      CefRefPtr<CefBrowser> b(browser);
      if (b->IsPopup())
        pCefViewPrivate_->onCefPopupBrowserCreated(b, w);
      else
        pCefViewPrivate_->onCefMainBrowserCreated(b, w);
    },
    c);
}

bool
CCefQMLClientDelegate::doClose(CefRefPtr<CefBrowser> browser)
{
  return false;
}

void
CCefQMLClientDelegate::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{}
