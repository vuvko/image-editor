#ifndef GUI_H
#define GUI_H

#include <string>
#include <vector>
#include <fstream>
using namespace std;
#include <QtGui/QMainWindow>
#include <QEvent>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QTextCodec>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QTimer>
#include <QDesktopServices>
#include <QInputDialog>
#include <QGridLayout>
#include <QGraphicsView>
#include <QProgressBar>
#include <QPushButton>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QToolButton>
#include <QRubberBand>
#include <QMouseEvent>
#include <QStatusBar>
#include <QDebug>

#include "../Engine/Engine.h"
#include "process_threads.h"
#include "parameterdialog.h"

/*
 *-------------
 */

namespace Ui {
class GUI;
}

class  QProgressEvent : public QEvent
{
public:
    uint percent;
    QProgressEvent() : QEvent((QEvent::Type)(QEvent::User + 1)) {}
};

class QStatusEvent : public QEvent
{
public:
    QString message;
    QStatusEvent() : QEvent((QEvent::Type)(QEvent::User + 2)) {}
};

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GraphicsView(QGraphicsView *parent = 0);
    ~GraphicsView();
    void setMove(bool is_moving = false);
    void setSelect(bool is_selected = false);
    void hideArea();
    QRect getArea();
    void setImageSize(uint width, uint height);
    void removeSelection();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    bool select;
    bool moving;
    bool showing;
    QPoint beginPos;
    QPoint endPos;
    QRubberBand *area;
    uint imgHeight;
    uint imgWidth;

    void updateSelection();
};

class GUI : public QMainWindow
{
    Q_OBJECT

public:
    GUI(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~GUI();

private slots:
    void onOpen();
    void onSave();
    void onSaveAs();
    void onAbout();

    void onGrayscale();
    void onGrayWorld();
    void onInvert();
    void onWaves();
    void onEmboss();

    void onAutoContrast();
    void onAutoLevels();

    void onGauss();
    void onMedian();
    void onUnsharp();
    void onSobel();
    void onSobelOne();
    void onMatrix();

    void onRotate();
    void onFreeScale();
    void onScale();

    void onSelect();
    void onUnSelect();

protected:
    bool event(QEvent *event);

private:
    QMenu *fileMenu;
    QMenu *filtersMenu;
    QMenu *blurMenu;
    QMenu *edgeMenu;
    QMenu *effectMenu;
    QMenu *imageMenu;
    QMenu *contrastMenu;
    QMenu *transformMenu;
    QMenu *helpMenu;

    QAction *mnuOpen;
    QAction *mnuSave;
    QAction *mnuSaveAs;
    QAction *mnuExit;

    QAction *mnuGrayscale;
    QAction *mnuGrayWorld;
    QAction *mnuGauss;
    QAction *mnuMedian;
    QAction *mnuSobel;
    QAction *mnuSobelOne;
    QAction *mnuUnsharp;
    QAction *mnuMatrix;
    QAction *mnuInvert;
    QAction *mnuWaves;
    QAction *mnuEmboss;

    QAction *mnuAutoContrast;
    QAction *mnuAutoLevels;

    QAction *mnuRotate;
    QAction *mnuScale;
    QAction *mnuFreeScale;

    QAction *mnuAbout;

    QAction *toolOpen;
    QAction *toolSave;
    QAction *toolSaveAs;
    QAction *toolSelect;
    QAction *toolUnSelect;

    QToolBar *fileBar;
    QToolBar *selBar;

    QPushButton *btnExit;
    GraphicsView *mainImgShow;
    QProgressBar *mainPBar;
    QWidget *centralWidget;

    Engine _engine;
    string imgPath;
    QGraphicsScene *currentScene;
    QGrayscaleThread *grayscaleThread;
    QTransformThread *transformThread;
    QGrayWorldThread *grayWorldThread;
    QLinCorrectionThread *linCorThread;
    QMatrixFilterThread *matrixFilterThread;
    QInvertThread *invertThread;
    QWavesThread *wavesThread;

    void onInit();
    void updateImage();
    void enableControls(bool enable);

    void createMenu();
    void createActions();
    void createToolBar();

    static void progCallback(uint percent, GUI *toThis);
    static void statCallback(QString message, GUI *toThis);
};

#endif // GUI_H
