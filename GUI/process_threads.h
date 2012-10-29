#ifndef PROCESS_THREADS_H
#define PROCESS_THREADS_H

#include <QThread>

#include "../Engine/Engine.h"

class QGrayscaleThread;
class QTransformThread;
class QGrayWorldThread;
class QLinCorrectionThread;
class QMatrixFilterThread;
class QInvertThread;
class QWavesThread;

/*
 * Threads
 */

class QGrayscaleThread : public QThread
{
    Q_OBJECT

    Engine& _engine;

protected:
    void run();

public:
    QGrayscaleThread(Engine& engine) : _engine(engine) {}
};

class QGrayWorldThread : public QThread
{
    Q_OBJECT

    Engine& _engine;

protected:
    void run();

public:
    QGrayWorldThread(Engine& engine) : _engine(engine) {}
};

class QTransformThread : public QThread
{
    Q_OBJECT

    Engine& _engine;

protected:
    void run();

public:
    enum {
        Rotate = 1,
        Scale = 2,
        LinearTransform = 3
    };

    QTransformThread(Engine& engine) : _engine(engine) {}
    void setTransform(uint type);
    void setAngle(qreal angle);
    void setScaleRatio(qreal scaleRatio);
    void setHorizScale(qreal xScale);
    void setVertScal(qreal yScale);

private:
    uint _type;
    qreal _angle;
    qreal _scaleRatio;
    qreal _xScale;
    qreal _yScale;
};

class QLinCorrectionThread : public QThread
{
    Q_OBJECT

    Engine& _engine;

protected:
    void run();

public:
    enum {
        autoContrast = 1,
        autoLevels = 2
    };

    QLinCorrectionThread(Engine& engine) : _engine(engine) {}
    void setType(uint type);

private:
    uint _type;
};

class QMatrixFilterThread : public QThread
{
    Q_OBJECT

    Engine& _engine;

protected:
    void run();

public:
    enum {
        Gauss = 1,
        Median = 2,
        Matrix = 3,
        Unsharp = 4,
        Sobel = 5,
        SobelOne = 6,
        Emboss = 7
    };

    QMatrixFilterThread(Engine& engine) : _engine(engine) {}
    void setType(uint type);
    void setSize(uint size);
    void setSigma(qreal sigma);
    void setAlpha(qreal alpha);
    void setMatrix(const QVector<QVector<qreal> > &matrix);

private:
    uint _type;
    uint _size;
    qreal _sigma;
    qreal _alpha;
    QVector<QVector<qreal> > _matrix;
};

class QInvertThread : public QThread
{
    Q_OBJECT

    Engine& _engine;

protected:
    void run();

public:
    QInvertThread(Engine& engine) : _engine(engine) {}
};

class QWavesThread : public QThread
{
    Q_OBJECT

    Engine& _engine;

protected:
    void run();

public:
    QWavesThread(Engine& engine) : _engine(engine) {}
    void setAmplitude(qreal amplitudeX, qreal amplitudeY);
    void setLength(qreal length);

private:
    qreal  _amplitudeX;
    qreal _amplitudeY;
    qreal _length;
};

#endif // PROCESS_THREADS_H
