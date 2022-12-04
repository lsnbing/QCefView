#include <QApplication>

#include <QCefContext.h>

//#include "MainWindow.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include "QCefQMLView.h"

int
main(int argc, char* argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication a(argc, argv);

  // build QCefConfig
  QCefConfig config;
  config.setUserAgent("QCefViewTest");
  config.setLogLevel(QCefConfig::LOGSEVERITY_DEFAULT);
  config.setBridgeObjectName("CallBridge");
  config.setRemoteDebuggingPort(9000);
  config.setBackgroundColor(Qt::lightGray);

  // WindowlessRenderingEnabled is set to true by default, set to false to disable the OSR mode
  // config.setWindowlessRenderingEnabled(false);
  // config.addCommandLineSwitch("disable-gpu");

  // add command line args
  // config.addCommandLineSwitch("allow-universal-access-from-files");
  config.addCommandLineSwitch("enable-media-stream");
  config.addCommandLineSwitch("use-mock-keychain");
  config.addCommandLineSwitch("allow-file-access-from-files");
  config.addCommandLineSwitch("disable-spell-checking");
  config.addCommandLineSwitch("disable-site-isolation-trials");
  config.addCommandLineSwitch("enable-aggressive-domstorage-flushing");
  config.addCommandLineSwitchWithValue("renderer-process-limit", "1");
  config.addCommandLineSwitchWithValue("disable-features", "BlinkGenPropertyTrees,TranslateUI,site-per-process");

  // initialize QCefContext instance with config
  QCefContext cefContext(&a, argc, argv, &config);
  //MainWindow w;
  //w.show();


  qmlRegisterType<QCefQMLView>("QCefQMLView", 1, 0, "QCefQMLView");

  QQuickView* view = new QQuickView;
  view->setSource(QUrl("qrc:/qml/main.qml"));
  view->show();

  return a.exec();
}
