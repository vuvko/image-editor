#include "process_threads.h"

/*
 * Grayscale thread
 */

void QGrayscaleThread::run()
{
    _engine.toGrayscale();
}

/*
 * Gray world thread
 */

void QGrayWorldThread::run()
{
    _engine.toGrayWorld();
}

/*
 * Contrast correction thread
 */

void QLinCorrectionThread::run()
{
    switch(_type)
    {
    case autoContrast:
        _engine.autoContrast();
        break;
    case autoLevels:
        _engine.autoLevels();
        break;
    default:
        break;
    }
}

void QLinCorrectionThread::setType(uint type)
{
    _type = type;
}

/*
 * Transform thread
 */

void QTransformThread::run()
{
    switch(_type)
    {
    case Rotate:
        _engine.rotate(_angle);
        break;
    case Scale:
        _engine.scale(_scaleRatio);
        break;
    case LinearTransform:
        _engine.scale(_xScale, _yScale);
        break;
    default:
        break;
    }
}

void QTransformThread::setAngle(qreal angle)
{
    _angle = angle;
}

void QTransformThread::setScaleRatio(qreal scaleRatio)
{
    _scaleRatio = scaleRatio;
}

void QTransformThread::setHorizScale(qreal xScale)
{
    _xScale = xScale;
}

void QTransformThread::setVertScal(qreal yScale)
{
    _yScale = yScale;
}

void QTransformThread::setTransform(uint type)
{
    _type = type;
}

/*
 * Matrix filter thread
 */

void QMatrixFilterThread::run()
{
    switch (_type)
    {
    case Gauss:
        _engine.addGaussFilter(_sigma, _size);
        break;
    case Median:
        _engine.addMedianFilter(_size);
        break;
    case Unsharp:
        _engine.addUnsharp(_alpha, _sigma);
        break;
    case Sobel:
        _engine.addSobel();
        break;
    case SobelOne:
        _engine.addSobelOne();
        break;
    case Matrix:
        _engine.addMatrixFilter(_matrix);
        break;
    case Emboss:
        _engine.toEmboss();
        break;
    default:
        break;
    }
}

void QMatrixFilterThread::setType(uint type)
{
    _type = type;
}

void QMatrixFilterThread::setSize(uint size)
{
    _size = size;
}

void QMatrixFilterThread::setSigma(qreal sigma)
{
    _sigma = sigma;
}

void QMatrixFilterThread::setAlpha(qreal alpha)
{
    _alpha = alpha;
}

void QMatrixFilterThread::setMatrix(const QVector<QVector<qreal> > &matrix)
{
    _matrix = matrix;
}

void QInvertThread::run()
{
    _engine.toInvert();
}

void QWavesThread::run()
{
    _engine.addWaves(_amplitudeX, _amplitudeY, _length);
}

void QWavesThread::setAmplitude(qreal amplitudeX, qreal amplitudeY)
{
    _amplitudeX = amplitudeX;
    _amplitudeY = amplitudeY;
}

void QWavesThread::setLength(qreal length)
{
    _length = length;
}
