#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QGridLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QVector>

namespace Ui {
class ParameterDialog;
}

class ParameterDialog : public QDialog
{
    Q_OBJECT

private slots:
    void cancel();
    void onScale();
    void onGauss();
    void onUnsharp();
    void onMatrix();
    void onMatrixResizeHeight(int height);
    void onMatrixResizeWidth(int width);
    void onWaves();
    
public:
    explicit ParameterDialog(QWidget *parent = 0);
    ~ParameterDialog();

    void setForScaleFactors();
    void setForGaussParams();
    void setForUnsharpParams();
    void setForMatrixFilter();
    void setForWavesParams();

    QVector<qreal> getScaleFactors();
    QVector<qreal> getGaussParams();
    QVector<qreal> getUnsharpParams();
    QVector<QVector<qreal> > getMatrixFilter();
    QVector<qreal> getWavesParams();
    
private:
    void MatrixResize(int width, int height);
    QPushButton *btnOk;
    QPushButton *btnCancel;
    QBoxLayout *btnLayout;
    QGridLayout *mainLayout;
    QGridLayout *matrixLayout;

    QVector<QLabel *> parLabels;
    QVector<QDoubleSpinBox *> parDoubleBox;
    QVector<QSpinBox *> parIntBox;

    QVector<qreal> params;
};

#endif // PARAMETERDIALOG_H
