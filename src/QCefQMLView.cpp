#include "QCefQMLView.h"

#pragma region qt_headers
#include <QPainter>
#include <QPoint>
#include <QResizeEvent>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QtDebug>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#pragma endregion qt_headers

#include <QCefContext.h>

#include "details/CCefQMLClientDelegate.h"
#include "details/QCefEventPrivate.h"
#include "details/QCefSettingPrivate.h"
#include "details/QCefViewPrivate.h"
#include "details/utils/CommonUtils.h"
#include <details/QCefConfigPrivate.h>
#include <details/QCefContextPrivate.h>
#include <details/QCefQMLViewPrivate.h>

QCefQMLView::QCefQMLView(const QString url, const QCefSetting* setting, QQuickItem* parent)
  : QQuickPaintedItem(parent)
  , d_ptr(new QCefQMLViewPrivate(QCefContext::instance()->d_func(), this, url, setting))
{
  d_ptr->createCefBrowser(this, url, setting);
}

QCefQMLView::QCefQMLView(QQuickItem* parent)
  : QCefQMLView("http://www.baidu.com", nullptr, parent)
{}

QCefQMLView ::~QCefQMLView()
{
  if (d_ptr) {
    d_ptr->destroyCefBrowser();
  }
}

QScreen*
QCefQMLView::screen()
{
  return qApp->primaryScreen();
}

void
QCefQMLView::addLocalFolderResource(const QString& path, const QString& url, int priority)
{}

void
QCefQMLView::addArchiveResource(const QString& path, const QString& url, const QString& password, int priority)
{}

int
QCefQMLView::browserId()
{
  return 0;
}

void
QCefQMLView::navigateToString(const QString& content)
{}
void
QCefQMLView::navigateToUrl(const QString& url)
{}
bool
QCefQMLView::browserCanGoBack()
{
  return false;
}
bool
QCefQMLView::browserCanGoForward()
{
  return false;
}
void
QCefQMLView::browserGoBack()
{}
void
QCefQMLView::browserGoForward()
{}
bool
QCefQMLView::browserIsLoading()
{
  return false;
}
void
QCefQMLView::browserReload()
{}
void
QCefQMLView::browserStopLoad()
{}
bool
QCefQMLView::triggerEvent(const QCefEvent& event)
{
  return true;
}
bool
QCefQMLView::triggerEvent(const QCefEvent& event, qint64 frameId)
{
  return true;
}
bool
QCefQMLView::broadcastEvent(const QCefEvent& event)
{
  return true;
}
bool
QCefQMLView::responseQCefQuery(const QCefQuery& query)
{
  return true;
}
bool
QCefQMLView::executeJavascript(qint64 frameId, const QString& code, const QString& url)
{
  return true;
}
bool
QCefQMLView::executeJavascriptWithResult(qint64 frameId, const QString& code, const QString& url, qint64 context)
{
  return true;
}
bool
QCefQMLView::setPreference(const QString& name, const QVariant& value, const QString& error)
{
  return true;
}
void
QCefQMLView::setDisablePopupContextMenu(bool disable)
{}
bool
QCefQMLView::isPopupContextMenuDisabled()
{
  return true;
}
void
QCefQMLView::onBrowserWindowCreated(QWindow* win)
{}
bool
QCefQMLView::onBeforePopup(qint64 frameId,
                           const QString& targetUrl,
                           const QString& targetFrameName,
                           QCefQMLView::CefWindowOpenDisposition targetDisposition,
                           QCefSetting& settings,
                           bool& DisableJavascriptAccess)
{
  return true;
}

void
QCefQMLView::onPopupCreated(QWindow* wnd)
{}
void
QCefQMLView::setFocus(Qt::FocusReason reason)
{}

QVariant
QCefQMLView::inputMethodQuery(Qt::InputMethodQuery query) const
{
  return QVariant(0);
}

void
QCefQMLView::inputMethodEvent(QInputMethodEvent* event)
{}

/// <summary>
/// Please refer to QWidget::focusInEvent
/// </summary>
void
QCefQMLView::focusInEvent(QFocusEvent* event)
{}

/// <summary>
/// Please refer to QWidget::focusOutEvent
/// </summary>
void
QCefQMLView::focusOutEvent(QFocusEvent* event)
{}

/// <summary>
/// Please refer to QWidget::keyPressEvent
/// </summary>
void
QCefQMLView::keyPressEvent(QKeyEvent* event)
{}

/// <summary>
/// Please refer to QWidget::keyReleaseEvent
/// </summary>
void
QCefQMLView::keyReleaseEvent(QKeyEvent* event)
{}

/// <summary>
/// Please refer to QWidget::mouseMoveEvent
/// </summary>
void
QCefQMLView::mouseMoveEvent(QMouseEvent* event)
{}

/// <summary>
/// Please refer to QWidget::mousePressEvent
/// </summary>
void
QCefQMLView::mousePressEvent(QMouseEvent* event)
{}

/// <summary>
/// Please refer to QWidget::mouseReleaseEvent
/// </summary>
void
QCefQMLView::mouseReleaseEvent(QMouseEvent* event)
{}

/// <summary>
/// Please refer to QWidget::wheelEvent
/// </summary>
void
QCefQMLView::wheelEvent(QWheelEvent* event)
{}

void
QCefQMLView::paint(QPainter* painter)
{
  Q_D(QCefQMLView);
  qDebug() << d->osr.qPopupRect_ << d->osr.qCefViewFrame_.isNull();

  painter->drawImage(boundingRect(), d->osr.qCefViewFrame_);
  painter->drawLine(0, 0, 30, 30);
}
