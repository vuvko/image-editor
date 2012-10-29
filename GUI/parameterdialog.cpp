#include "parameterdialog.h"

ParameterDialog::ParameterDialog(QWidget *parent) :
    QDialog(parent)
{
    btnOk = new QPushButton;
    btnOk->setText(tr("Ok"));
    btnOk->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnCancel = new QPushButton;
    btnCancel->setText(tr("Cancel"));
    btnCancel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    btnLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);

    mainLayout = new QGridLayout(this);

    matrixLayout = new QGridLayout();

    connect(btnCancel, SIGNAL(clicked()), this, SLOT(cancel()));
}

ParameterDialog::~ParameterDialog()
{
    for (uint i = 0; i < parLabels.size(); ++i)
    {
        delete parLabels[i];
    }
    for (uint i = 0; i < parIntBox.size(); ++i)
    {
        delete parIntBox[i];
    }
    for (uint i = 0; i < parDoubleBox.size(); ++i)
    {
        delete parDoubleBox[i];
    }

    if (btnOk != 0)
        delete btnOk;
    if (btnCancel != 0)
        delete btnCancel;
    if (matrixLayout != 0)
        delete matrixLayout;
    if (btnLayout != 0)
        delete btnLayout;
    if (mainLayout != 0)
        delete mainLayout;
}

void ParameterDialog::cancel()
{
    reject();
}

void ParameterDialog::setForScaleFactors()
{
    parLabels = QVector<QLabel *>(2);
    parDoubleBox = QVector<QDoubleSpinBox *>(2);
    for (uint i = 0; i < 2; ++i)
    {
        parLabels[i] =new QLabel;
        parLabels[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        parDoubleBox[i] = new QDoubleSpinBox;
        parDoubleBox[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        parDoubleBox[i]->setDecimals(3);
        parDoubleBox[i]->setRange(0, 15);
        parDoubleBox[i]->setSingleStep(0.25);
        parDoubleBox[i]->setValue(1.0);
    }
    parLabels[0]->setText(tr("Width scale factor:"));
    parLabels[1]->setText(tr("Height scale factor"));

    for (uint i = 0; i < 2; ++i)
    {
        mainLayout->addWidget(parLabels[i], i + 1, 1, 1, 1);
        mainLayout->addWidget(parDoubleBox[i], i + 1, 2, 1, 1);
    }

    mainLayout->addLayout(btnLayout, 3, 1, 1, 2);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(onScale()));
}

QVector<qreal> ParameterDialog::getScaleFactors()
{
    return params;
}

void ParameterDialog::onScale()
{
    params = QVector<qreal>(2);
    for (uint i = 0; i < 2; ++i)
    {
        params[i] = parDoubleBox[i]->value();
    }

    accept();
}

void ParameterDialog::setForGaussParams()
{
    parLabels = QVector<QLabel *>(2);
    parDoubleBox = QVector<QDoubleSpinBox *>(2);
    for (uint i = 0; i < 2; ++i)
    {
        parLabels[i] =new QLabel;
        parLabels[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    parLabels[0]->setText(tr("Filter radius:"));
    parLabels[1]->setText(tr("Blur radius"));

    parDoubleBox[0] = new QDoubleSpinBox;
    parDoubleBox[0]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    parDoubleBox[0]->setDecimals(1);
    parDoubleBox[0]->setRange(1.5, 25);
    parDoubleBox[0]->setSingleStep(1);
    parDoubleBox[0]->setValue(1.5);

    parDoubleBox[1] = new QDoubleSpinBox;
    parDoubleBox[1]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    parDoubleBox[1]->setDecimals(3);
    parDoubleBox[1]->setRange(0, 10);
    parDoubleBox[1]->setSingleStep(0.25);
    parDoubleBox[1]->setValue(1.0);

    for (uint i = 0; i < 2; ++i)
    {
        mainLayout->addWidget(parLabels[i], i + 1, 1, 1, 1);
    }

    mainLayout->addWidget(parDoubleBox[0], 1, 2, 1, 1);
    mainLayout->addWidget(parDoubleBox[1], 2, 2, 1, 1);

    mainLayout->addLayout(btnLayout, 3, 1, 1, 2);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(onGauss()));
}

void ParameterDialog::onGauss()
{
    params = QVector<qreal>(2);
    params[0] = parDoubleBox[0]->value() * 2;
    params[1] = parDoubleBox[1]->value();

    accept();
}

QVector<qreal> ParameterDialog::getGaussParams()
{
    return params;
}

void ParameterDialog::setForUnsharpParams()
{
    parLabels = QVector<QLabel *>(2);
    parDoubleBox = QVector<QDoubleSpinBox *>(2);
    for (uint i = 0; i < 2; ++i)
    {
        parLabels[i] =new QLabel;
        parLabels[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        parDoubleBox[i] = new QDoubleSpinBox;
        parDoubleBox[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        parDoubleBox[i]->setDecimals(3);
        parDoubleBox[i]->setRange(0.25, 15);
        parDoubleBox[i]->setSingleStep(0.25);
    }
    parDoubleBox[0]->setValue(0.25);
    parDoubleBox[1]->setValue(1.0);
    parLabels[0]->setText(tr("Parameter (alpha):"));
    parLabels[1]->setText(tr("Blur radius:"));

    for (uint i = 0; i < 2; ++i)
    {
        mainLayout->addWidget(parLabels[i], i + 1, 1, 1, 1);
        mainLayout->addWidget(parDoubleBox[i], i + 1, 2, 1, 1);
    }

    mainLayout->addLayout(btnLayout, 3, 1, 1, 2);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(onUnsharp()));
}

QVector<qreal> ParameterDialog::getUnsharpParams()
{
    return params;
}

void ParameterDialog::onUnsharp()
{
    params = QVector<qreal>(2);
    for (uint i = 0; i < 2; ++i)
    {
        params[i] = parDoubleBox[i]->value();
    }

    accept();
}

void ParameterDialog::setForMatrixFilter()
{
    parLabels = QVector<QLabel *>(2);
    parIntBox = QVector<QSpinBox *>(2);
    parDoubleBox = QVector<QDoubleSpinBox *>(9);

    for (uint i = 0; i < 2; ++i)
    {
        parIntBox[i] = new QSpinBox;
        parIntBox[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        parIntBox[i]->setRange(3, 25);
        parIntBox[i]->setSingleStep(1);

        parLabels[i] = new QLabel;
        parLabels[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    parLabels[0]->setText(tr("Kernel width:"));
    parLabels[1]->setText(tr("Kernel height"));

    for (uint i = 0; i < parDoubleBox.size(); ++i)
    {
        parDoubleBox[i] = new QDoubleSpinBox;
        parDoubleBox[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        parDoubleBox[i]->setDecimals(5);
        parDoubleBox[i]->setRange(-100, 100);
        parDoubleBox[i]->setSingleStep(0.2);
        parDoubleBox[i]->setValue(0);
    }

    for (uint i = 0; i < 3; ++i)
    {
        for (uint j = 0; j < 3; ++j)
        {
            matrixLayout->addWidget(parDoubleBox[i * 3 + j], i + 1, j + 1);
        }
    }

    mainLayout->addWidget(parLabels[0], 1, 1);
    mainLayout->addWidget(parIntBox[0], 1, 2);
    mainLayout->addWidget(parLabels[1], 2, 1);
    mainLayout->addWidget(parIntBox[1], 2, 2);
    mainLayout->addLayout(matrixLayout, 3, 1, 1, 2);

    mainLayout->addLayout(btnLayout, 4, 1, 1, 2);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(onMatrix()));
    connect(parIntBox[0], SIGNAL(valueChanged(int)), this, SLOT(onMatrixResizeWidth(int)));
    connect(parIntBox[1], SIGNAL(valueChanged(int)), this, SLOT(onMatrixResizeHeight(int)));
}

void ParameterDialog::onMatrix()
{
    uint width = parIntBox[0]->value();
    uint height = parIntBox[1]->value();
    params = QVector<qreal>(width * height + 2);
    params[0] = width;
    params[1] = height;

    for (uint i = 0; i < width * height; ++i)
    {
        params[i + 2] = parDoubleBox[i]->value();
    }

    accept();
}

void ParameterDialog::MatrixResize(int width, int height)
{
    parDoubleBox.resize(width * height);
    for (uint i = 0; i < parDoubleBox.size(); ++i)
    {
        parDoubleBox[i] = new QDoubleSpinBox;
        parDoubleBox[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        parDoubleBox[i]->setDecimals(5);
        parDoubleBox[i]->setRange(-100, 100);
        parDoubleBox[i]->setSingleStep(0.2);
        parDoubleBox[i]->setValue(0);
    }

    for (uint i = 0; i < height; ++i)
    {
        for (uint j = 0; j < width; ++j)
        {
            matrixLayout->addWidget(parDoubleBox[i * width + j], i + 1, j + 1);
        }
    }
}

void ParameterDialog::onMatrixResizeHeight(int height)
{
    for (uint i = 0; i < parDoubleBox.size(); ++i)
    {
        delete parDoubleBox[i];
    }
    int width = parIntBox[0]->value();

    MatrixResize(width, height);
}

void ParameterDialog::onMatrixResizeWidth(int width)
{
    for (uint i = 0; i < parDoubleBox.size(); ++i)
    {
        delete parDoubleBox[i];
    }
    int height = parIntBox[1]->value();

    MatrixResize(width, height);
}

QVector<QVector<qreal> > ParameterDialog::getMatrixFilter()
{
    QVector<QVector<qreal> > matrix(params[1], QVector<qreal>(params[0]));
    uint width = params[0];
    uint height = params[1];

    for (uint i = 0; i < height; ++i)
    {
        for (uint j = 0; j < width; ++j)
        {
            matrix[j][i] = params[2 + i * width + j];
        }
    }

    return matrix;
}

void ParameterDialog::setForWavesParams()
{
    parLabels = QVector<QLabel *>(3);
    parDoubleBox = QVector<QDoubleSpinBox *>(3);

    for (uint i = 0; i < 3; ++i)
    {
        parLabels[i] = new QLabel;
        parDoubleBox[i] = new QDoubleSpinBox;
        parDoubleBox[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        parDoubleBox[i]->setDecimals(3);
        parDoubleBox[i]->setRange(-100, 100);
        parDoubleBox[i]->setSingleStep(0.2);
        parDoubleBox[i]->setValue(0);
    }
    parDoubleBox[2]->setRange(1, 500);
    parDoubleBox[2]->setValue(20);
    parLabels[0]->setText(tr("X amplitude:"));
    parLabels[1]->setText(tr("Y amplitude:"));
    parLabels[2]->setText(tr("Wave length:"));

    for (uint i = 0; i < 3; ++i)
    {
        mainLayout->addWidget(parLabels[i], i, 1);
        mainLayout->addWidget(parDoubleBox[i], i, 2);
    }

    mainLayout->addLayout(btnLayout, 4, 1, 1, 2);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(onWaves()));
}

void ParameterDialog::onWaves()
{
    params = QVector<qreal>(3);
    for (uint i = 0; i < 3; ++i)
    {
        params[i] = parDoubleBox[i]->value();
    }

    accept();
}

QVector<qreal> ParameterDialog::getWavesParams()
{
    return params;
}
