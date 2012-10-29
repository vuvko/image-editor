#include "gui.h"

/*
 *--------------------------------------
 *GUI
 */

void GUI::onInit()
{
    QRect geometry;

    //move window to screen center
    geometry = frameGeometry();
    geometry.moveCenter(QDesktopWidget().availableGeometry().center());
    move(geometry.topLeft());

    setMinimumSize(500, 500);

    // set default encoding to UTF-8
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    currentScene = NULL;
    _engine.setCallbacks((PCALLBACK)progCallback, (SCALLBACK)statCallback, this);
}

void GUI::updateImage()
{
    statusBar()->showMessage("Updating image...");
    QPixmap pmNew = QPixmap::fromImage(_engine.getImage());
    mainImgShow->setImageSize(pmNew.width(), pmNew.height());
    QGraphicsScene *newScene = new QGraphicsScene;
    newScene->addPixmap(pmNew);
    mainImgShow->setScene(newScene);
    if (currentScene != NULL) delete currentScene;
    currentScene = newScene;
    enableControls(true);
    statusBar()->showMessage("Done.");
}

void GUI::onOpen()
{
    string path;
    statusBar()->showMessage("Opening image file...");
    path = QFileDialog::getOpenFileName(this,
                                        tr("Load image."),
                                        QDir::currentPath(),
                                        tr("Images (*.png *.jpg *.bmp)")).toStdString();
    if (path.empty()) return;

    if (!_engine.load(path))
    {
        QMessageBox::critical(this, "Error!",
                              "Can't load image.");
        return;
    };

    statusBar()->showMessage("Done.");
    updateImage();
    imgPath = path;
}

void GUI::onSaveAs()
{
    string path;

    path = QFileDialog::getSaveFileName(this,
                                        tr("Save image."),
                                        QDir::currentPath(),
                                        tr("Images (*.png *.jpg *.bmp)")).toStdString();
    if (path.empty()) return;

    statusBar()->showMessage(QString::fromStdString("Saving image file to " + path));

    if (!_engine.save(path))
    {
        QMessageBox::critical(this, "Error!",
                              "Can't save image.");
        return;
    };
    statusBar()->showMessage("Done.");
    imgPath = path;
}

void GUI::onSave()
{
    statusBar()->showMessage("Saving image...");
    if (!_engine.save(imgPath))
    {
        QMessageBox::critical(this, "Error!",
                              "Can't save image");
        return;
    }

    statusBar()->showMessage("Done.");
}

void GUI::onRotate()
{
    enableControls(false);
    bool ok = false;
    qreal angle = QInputDialog::getDouble(this, tr("Rotate image"),
                                          tr("Angle:"), 0, -360, 360, 5, &ok);
    if (!ok)
    {
        enableControls(true);
        return;
    }
    _engine.setArea(mainImgShow->getArea());
    transformThread->setTransform(QTransformThread::Rotate);
    transformThread->setAngle(angle);
    transformThread->start();
}

void GUI::onFreeScale()
{
    ParameterDialog dialog(this);
    dialog.setForScaleFactors();
    if (dialog.exec())
    {
        enableControls(false);
        QVector<qreal> scaleFactors = dialog.getScaleFactors();
        _engine.setArea(mainImgShow->getArea());
        transformThread->setTransform(QTransformThread::LinearTransform);
        transformThread->setHorizScale(scaleFactors[0]);
        transformThread->setVertScal(scaleFactors[1]);
        transformThread->start();
    }
}

void GUI::onScale()
{
    enableControls(false);
    bool ok = false;
    qreal scaleRatio = QInputDialog::getDouble(this,
                                               tr("Scale image"),
                                               tr("Scale factor:"),
                                               1, 0, 10, 5, &ok);
    if (!ok)
    {
        enableControls(true);
        return;
    }
    _engine.setArea(mainImgShow->getArea());
    transformThread->setTransform(QTransformThread::Scale);
    transformThread->setScaleRatio(scaleRatio);
    transformThread->start();
}

void GUI::onAutoContrast()
{
    enableControls(false);
    _engine.setArea(mainImgShow->getArea());
    linCorThread->setType(QLinCorrectionThread::autoContrast);
    linCorThread->start();
}

void GUI::onAutoLevels()
{
    enableControls(false);
    _engine.setArea(mainImgShow->getArea());
    linCorThread->setType(QLinCorrectionThread::autoLevels);
    linCorThread->start();
}

void GUI::onGrayscale()
{
    enableControls(false);
    _engine.setArea(mainImgShow->getArea());
    grayscaleThread->start();
}

void GUI::onGrayWorld()
{
    enableControls(false);
    _engine.setArea(mainImgShow->getArea());
    grayWorldThread->start();
}

void GUI::onGauss()
{
    ParameterDialog dialog(this);
    dialog.setForGaussParams();
    if (dialog.exec())
    {
        enableControls(false);
        QVector<qreal> params = dialog.getGaussParams();
        _engine.setArea(mainImgShow->getArea());
        matrixFilterThread->setType(QMatrixFilterThread::Gauss);
        matrixFilterThread->setSigma(params[1]);
        matrixFilterThread->setSize((uint) params[0]);
        matrixFilterThread->start();
    }
}

void GUI::onMedian()
{
    enableControls(false);
    bool ok = false;
    uint size = QInputDialog::getInt(this,
                                     tr("Median filter"),
                                     tr("Matrix size:"),
                                     3, 3, 25, 2, &ok);
    if (!ok)
    {
        enableControls(true);
        return;
    }
    matrixFilterThread->setType(QMatrixFilterThread::Median);
    _engine.setArea(mainImgShow->getArea());
    matrixFilterThread->setSize(size);
    matrixFilterThread->start();
}

void GUI::onUnsharp()
{
    ParameterDialog dialog;
    dialog.setForUnsharpParams();
    if (dialog.exec())
    {
        enableControls(false);
        QVector<qreal> params = dialog.getUnsharpParams();
        _engine.setArea(mainImgShow->getArea());
        matrixFilterThread->setType(QMatrixFilterThread::Unsharp);
        matrixFilterThread->setSigma(params[1]);
        matrixFilterThread->setAlpha(params[0]);
        matrixFilterThread->start();
    }
}

void GUI::onSobel()
{
    enableControls(false);
    matrixFilterThread->setType(QMatrixFilterThread::Sobel);
    _engine.setArea(mainImgShow->getArea());
    matrixFilterThread->start();
}

void GUI::onSobelOne()
{
    enableControls(false);
    matrixFilterThread->setType(QMatrixFilterThread::SobelOne);
    _engine.setArea(mainImgShow->getArea());
    matrixFilterThread->start();
}

void GUI::onMatrix()
{
    ParameterDialog dialog;
    dialog.setForMatrixFilter();
    if (dialog.exec())
    {
        enableControls(false);
        QVector<QVector<qreal> > filter = dialog.getMatrixFilter();
        _engine.setArea(mainImgShow->getArea());
        matrixFilterThread->setMatrix(filter);
        matrixFilterThread->setType(QMatrixFilterThread::Matrix);
        matrixFilterThread->start();
    }
}

void GUI::onInvert()
{
    enableControls(false);
    _engine.setArea(mainImgShow->getArea());
    invertThread->start();
}

void GUI::onWaves()
{
    ParameterDialog dialog;
    dialog.setForWavesParams();
    if (dialog.exec())
    {
        enableControls(false);
        QVector<qreal> params = dialog.getWavesParams();
        _engine.setArea(mainImgShow->getArea());
        wavesThread->setAmplitude(params[0], params[1]);
        wavesThread->setLength(params[2]);
        wavesThread->start();
    }
}

void GUI::onEmboss()
{
    enableControls(false);
    _engine.setArea(mainImgShow->getArea());
    matrixFilterThread->setType(QMatrixFilterThread::Emboss);
    matrixFilterThread->start();
}

void GUI::onSelect()
{
    mainImgShow->setSelect(true);
    toolSelect->setEnabled(false);
    toolUnSelect->setEnabled(true);
}

void GUI::onUnSelect()
{
    mainImgShow->removeSelection();
    toolSelect->setEnabled(true);
    toolUnSelect->setEnabled(false);
}

void GUI::onAbout()
{
    QMessageBox::about(this, tr("About image editor"),
                       tr("<p>This is a simple image editor</p>"));
}

GUI::GUI(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    grayscaleThread = new QGrayscaleThread(_engine);
    grayWorldThread = new QGrayWorldThread(_engine);
    transformThread = new QTransformThread(_engine);
    linCorThread = new QLinCorrectionThread(_engine);
    matrixFilterThread = new QMatrixFilterThread(_engine);
    invertThread = new QInvertThread(_engine);
    wavesThread = new QWavesThread(_engine);

    centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    mainImgShow = new GraphicsView();
    mainImgShow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainImgShow->setBackgroundRole(QPalette::Dark);

    mainPBar = new QProgressBar();
    mainPBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    btnExit = new QPushButton(tr("Exit"));
    btnExit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QGridLayout *mainLayout = new QGridLayout(centralWidget);
    mainLayout->addWidget(mainImgShow, 1, 1, 1, 2);
    mainLayout->addWidget(mainPBar, 2, 1, 1, 1);
    mainLayout->addWidget(btnExit, 2, 2, 1, 1);

    connect(btnExit, SIGNAL(clicked()), this, SLOT(close()));

    onInit();

    createActions();
    createMenu();
    createToolBar();

    setWindowIcon(QIcon(":/Resources/mainicon.png"));
    setWindowTitle(tr("Simple image editor"));

    enableControls(false);

    statusBar()->showMessage(tr("Ready"));
}

GUI::~GUI()
{}

void GUI::createMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(mnuOpen);
    fileMenu->addAction(mnuSave);
    fileMenu->addAction(mnuSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(mnuExit);

    filtersMenu = menuBar()->addMenu(tr("Fi&lters"));
    filtersMenu->addAction(mnuGrayscale);
    filtersMenu->addAction(mnuGrayWorld);
    filtersMenu->addSeparator();
    blurMenu = filtersMenu->addMenu(tr("Blur filters"));
    blurMenu->addAction(mnuGauss);
    blurMenu->addAction(mnuMedian);
    edgeMenu = filtersMenu->addMenu(tr("Find edges"));
    edgeMenu->addAction(mnuSobel);
    edgeMenu->addAction(mnuSobelOne);
    effectMenu = filtersMenu->addMenu(tr("Effects"));
    effectMenu->addAction(mnuEmboss);
    effectMenu->addAction(mnuUnsharp);
    effectMenu->addAction(mnuInvert);
    effectMenu->addAction(mnuWaves);
    filtersMenu->addAction(mnuMatrix);

    imageMenu = menuBar()->addMenu(tr("&Image"));
    contrastMenu = imageMenu->addMenu(tr("&Contrast correction"));
    contrastMenu->addAction(mnuAutoContrast);
    contrastMenu->addAction(mnuAutoLevels);
    transformMenu = imageMenu->addMenu(tr("&Transform"));
    transformMenu->addAction(mnuRotate);
    transformMenu->addAction(mnuScale);
    transformMenu->addAction(mnuFreeScale);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(mnuAbout);
}

void GUI::createActions()
{
    mnuOpen = new QAction(tr("&Open"), this);
    mnuOpen->setShortcut(tr("Ctrl+O"));
    mnuOpen->setStatusTip(tr("Open image file"));
    connect(mnuOpen, SIGNAL(activated()), this, SLOT(onOpen()));
    mnuSave = new QAction(tr("&Save"), this);
    mnuSave->setShortcut(tr("Ctrl+S"));
    mnuSave->setStatusTip(tr("Save image file"));
    connect(mnuSave, SIGNAL(activated()), this, SLOT(onSave()));
    mnuSaveAs = new QAction(tr("Save As"), this);
    mnuSaveAs->setShortcut(tr("Ctrl+Shift+S"));
    mnuSaveAs->setStatusTip(tr("Save as another image file"));
    connect(mnuSaveAs, SIGNAL(activated()), this, SLOT(onSaveAs()));
    mnuGrayscale = new QAction(tr("Grayscale"), this);
    mnuGrayscale->setStatusTip(tr("Convert image to grayscale"));
    connect(mnuGrayscale, SIGNAL(activated()), this, SLOT(onGrayscale()));
    mnuRotate = new QAction(tr("Rotate"), this);
    mnuRotate->setStatusTip(tr("Rotate image"));
    connect(mnuRotate, SIGNAL(activated()), this, SLOT(onRotate()));
    mnuFreeScale = new QAction(tr("Stretch image"), this);
    mnuFreeScale->setStatusTip(tr("Stretch image"));
    connect(mnuFreeScale, SIGNAL(activated()), this, SLOT(onFreeScale()));
    mnuScale = new QAction(tr("Scale image"), this);
    mnuScale->setStatusTip(tr("Scale image"));
    connect(mnuScale, SIGNAL(activated()), this, SLOT(onScale()));
    mnuExit = new QAction(tr("E&xit"), this);
    mnuExit->setShortcut(tr("Ctrl+Q"));
    mnuExit->setStatusTip(tr("Exit from programm"));
    connect(mnuExit, SIGNAL(activated()), this, SLOT(close()));
    mnuGrayWorld = new QAction(tr("Gray world"), this);
    mnuGrayWorld->setStatusTip(tr("Use \"Gray World\" filter"));
    connect(mnuGrayWorld, SIGNAL(activated()), this, SLOT(onGrayWorld()));
    mnuAutoContrast = new QAction(tr("Auto contrast"), this);
    mnuAutoContrast->setStatusTip(tr("Use autocontrast to stretch histogram"));
    connect(mnuAutoContrast, SIGNAL(activated()), this, SLOT(onAutoContrast()));
    mnuAutoLevels = new QAction(tr("Auto levels"), this);
    mnuAutoLevels->setStatusTip(tr("Use autolevels to stretch channels' histograms"));
    connect(mnuAutoLevels, SIGNAL(activated()), this, SLOT(onAutoLevels()));
    mnuGauss = new QAction(tr("Gaussian blur"), this);
    mnuGauss->setStatusTip(tr("Use Gaussian filter to blur image"));
    connect(mnuGauss, SIGNAL(activated()), this, SLOT(onGauss()));
    mnuMedian = new QAction(tr("Median filter"), this);
    mnuMedian->setStatusTip(tr("Use median matrix to remove noise"));
    connect(mnuMedian, SIGNAL(activated()), this, SLOT(onMedian()));
    mnuUnsharp = new QAction(tr("Unsharp"), this);
    mnuUnsharp->setStatusTip(tr("Use unsharp filter to mark edges"));
    connect(mnuUnsharp, SIGNAL(activated()), this, SLOT(onUnsharp()));
    mnuAbout = new QAction(tr("About"), this);
    mnuAbout->setShortcut(tr("F1"));
    mnuAbout->setStatusTip(tr("About programm"));
    connect(mnuAbout, SIGNAL(activated()), this, SLOT(onAbout()));
    mnuSobel = new QAction(tr("Sobel operator"), this);
    mnuSobel->setStatusTip(tr("Use Sobel operator to find edges"));
    connect(mnuSobel, SIGNAL(activated()), this, SLOT(onSobel()));
    mnuSobelOne = new QAction(tr("One direction Sobel operator"), this);
    mnuSobelOne->setStatusTip(tr("Use one direction Sobel operator to find edges"));
    connect(mnuSobelOne, SIGNAL(activated()), this, SLOT(onSobelOne()));
    mnuMatrix = new QAction(tr("Matrix filter"), this);
    mnuMatrix->setStatusTip(tr("Use custom matrix filter"));
    connect(mnuMatrix, SIGNAL(activated()), this, SLOT(onMatrix()));
    mnuInvert = new QAction(tr("Digital intermediate"), this);
    mnuInvert->setStatusTip(tr("Invert image's colors"));
    connect(mnuInvert, SIGNAL(activated()), this, SLOT(onInvert()));
    mnuWaves = new QAction(tr("Waves"), this);
    mnuWaves->setStatusTip(tr("Add \"Waves\" effect"));
    connect(mnuWaves, SIGNAL(activated()), this, SLOT(onWaves()));
    mnuEmboss = new QAction(tr("Emboss"), this);
    mnuEmboss->setStatusTip(tr("Use filter to create a stamp-like image"));
    connect(mnuEmboss, SIGNAL(activated()), this, SLOT(onEmboss()));

    toolOpen = new QAction(QIcon(tr(":/Resources/open.png")), tr("Open"), this);
    connect(toolOpen, SIGNAL(activated()), this, SLOT(onOpen()));
    toolSave = new QAction(QIcon(tr(":/Resources/save.png")), tr("Save"), this);
    connect(toolSave, SIGNAL(activated()), this, SLOT(onSave()));
    toolSaveAs = new QAction(QIcon(tr(":/Resources/save-as.png")), tr("Save As"), this);
    connect(toolSaveAs, SIGNAL(activated()), this, SLOT(onSaveAs()));

    toolSelect = new QAction(QIcon(":/Resources/select.png"), tr("Select region"), this);
    connect(toolSelect, SIGNAL(activated()), this, SLOT(onSelect()));
    toolUnSelect = new QAction(
                QIcon(":/Resources/unselect.png"),
                tr("Remove selection"),
                this);
    connect(toolUnSelect, SIGNAL(activated()), this, SLOT(onUnSelect()));
    toolUnSelect->setEnabled(false);
}

void GUI::createToolBar()
{
    fileBar = addToolBar(tr("File"));
    fileBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    fileBar->addAction(toolOpen);
    fileBar->addAction(toolSave);
    fileBar->addAction(toolSaveAs);

    selBar = addToolBar(tr("Selection"));
    selBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    selBar->addAction(toolSelect);
    selBar->addAction(toolUnSelect);
}

void GUI::progCallback(uint percent, GUI* toThis)
{
    QProgressEvent *event = new QProgressEvent;
    event->percent = percent;
    QCoreApplication::postEvent(toThis, event);
}

void GUI::statCallback(QString message, GUI *toThis)
{
    QStatusEvent *event = new QStatusEvent;
    event->message = message;
    QCoreApplication::postEvent(toThis, event);
}

void GUI::enableControls(bool enable)
{
    filtersMenu->setEnabled(enable);
    imageMenu->setEnabled(enable);
    mnuSave->setEnabled(enable);
    mnuSaveAs->setEnabled(enable);
    toolSave->setEnabled(enable);
    toolSaveAs->setEnabled(enable);
}

bool GUI::event(QEvent *event)
{
    if (event->type() == (QEvent::Type)(QEvent::User + 1))
    {
        // Progress event
        QProgressEvent *pEvent = (QProgressEvent *)event;
        mainPBar->setValue(pEvent->percent);
        if (pEvent->percent == 100)
        {
            enableControls(true);
            updateImage();
        }
    } else if (event->type() == (QEvent::Type)(QEvent::User + 2))
    {
        // Status event
        QStatusEvent *sEvent = (QStatusEvent *)event;
        statusBar()->showMessage(sEvent->message);
    }

    return QMainWindow::event(event);
}

GraphicsView::GraphicsView(QGraphicsView *parent) : QGraphicsView(parent)
{
    moving = false;
    showing = false;
    select = false;
    area = new QRubberBand(QRubberBand::Rectangle, this);
    area->resize(size());
    beginPos = QPoint(0, 0);
    endPos = QPoint(width(), height());
}

GraphicsView::~GraphicsView()
{
    delete area;
}

void GraphicsView::setMove(bool is_moving)
{
    moving = is_moving;
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && select)
    {
        moving = true;
        showing = false;
        beginPos = event->pos();
        area->hide();
    }
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (moving && select)
    {
        showing = true;
        endPos = event->pos();
        updateSelection();
    }
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && moving && select)
    {
        moving = false;
        if (!showing)
            endPos = beginPos;
    }
}

void GraphicsView::removeSelection()
{
    area->hide();
    showing = false;
    select = false;
}

QRect boundRect(QPoint begin, QPoint end)
{
    int left = qMin(begin.x(), end.x());
    int right = qMax(begin.x(), end.x());
    int top = qMin(begin.y(), end.y());
    int bottom = qMax(begin.y(), end.y());
    return QRect(QPoint(left, top), QPoint(right, bottom));
}

QRectF boundRect(QPointF begin, QPointF end)
{
    int left = qMin(begin.x(), end.x()) - 2;
    int right = qMax(begin.x(), end.x()) - 2;
    int top = qMin(begin.y(), end.y()) - 2;
    int bottom = qMax(begin.y(), end.y()) - 2;
    return QRectF(QPointF(left, top), QPointF(right, bottom));
}

void GraphicsView::updateSelection()
{
    area->setGeometry(boundRect(beginPos, endPos));
    area->show();
}

void GraphicsView::hideArea()
{
    area->hide();
}

void GraphicsView::setImageSize(uint width, uint height)
{
    imgWidth = width;
    imgHeight = height;
}

void GraphicsView::setSelect(bool is_selected)
{
    select = is_selected;
}

QRect GraphicsView::getArea()
{
    QRectF selectionRect;
    if (showing)
    {
        QPointF begin = mapToScene(beginPos);
        QPointF end = mapToScene(endPos);
        selectionRect = boundRect(begin, end);
        QRectF imageRect = scene()->itemsBoundingRect().adjusted(0, 0, 0, 0);
        selectionRect = selectionRect & imageRect;
        if (selectionRect.isEmpty()) {
            selectionRect = imageRect;
        }
    } else {
        selectionRect = QRectF(0.0, 0.0, imgWidth, imgHeight);
    }
    QRectF resultRect = selectionRect;

    return resultRect.toAlignedRect();
}
