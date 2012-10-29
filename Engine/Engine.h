#pragma once
#ifndef _Engine_H
#define _Engine_H

#include <string>
using namespace std;
#include <qmath.h>
#include <QImage>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QColor>

typedef void (*PCALLBACK)(uint percent, void *extra);
typedef void (*SCALLBACK)(QString message, void *extra);

class Engine
{
public:
    Engine();

    void setCallbacks(
            PCALLBACK progCallback,
            SCALLBACK statCallback,
            void *callbackExtra);

    bool load(const string& path);
    bool save(const string& path);

    const QImage& getImage() const;

    void toGrayscale();
    void toGrayWorld();
    void toInvert();
    void addWaves(qreal amplitudeX, qreal amplitudeY, qreal length);
    void autoContrast();
    void autoLevels();

    void addGaussFilter(qreal sigma, uint size = 0);
    void addMedianFilter(uint size = 3);
    void addMatrixFilter(const QVector<QVector<qreal> > &filterMatrix);
    void addMatrixFilterNonParsed(const char *matrix);
    void addUnsharp(qreal alpha, qreal sigma);
    void addSobel();
    void addSobelOne();
    void toEmboss();

    void rotate(qreal angle);
    void scale(qreal scaleRatio);
    void scale(qreal xScale, qreal yScale);

    void setArea(const QRect &rArea);

private:
    enum {
        Red = 0,
        Green = 1,
        Blue = 2
    };

    static const qreal EPS = 0.000001;

    QImage image;
    QRect area;
    PCALLBACK progressCallback;
    SCALLBACK statusCallback;
    void *callbackExtra;

    QVector<QVector<qreal> > createGauss(qreal sigma, uint size = 0);
    QVector<qreal> createGaussVector(qreal sigma, uint size = 0);
    QVector<QVector<int> > createSobelX();
    QVector<QVector<int> > createSobelY();
    qreal linearInterpolation(qreal left, qreal right, qreal lValue, qreal rValue, qreal x);
    qreal belinearInterpolation(
            QRect r,
            qreal ltValue,
            qreal lbValue,
            qreal rbValue,
            qreal rtValue,
            qreal x, qreal y);
    QRgb belinearColorInterpolation(
            QRect r,
            QRgb ltValue,
            QRgb lbValue,
            QRgb rbValue,
            QRgb rtValue,
            qreal x, qreal y);
};

#endif //_Engine_H
