#include "Engine.h"

Engine::Engine()
{
    progressCallback = NULL;
    callbackExtra = NULL;
    statusCallback = NULL;
}

bool Engine::load(const string& pathString)
{
    QString path(pathString.data());
    bool ok = image.load(path);
    if (ok)
    {
        area =image.rect();
    }
    return ok;
}

bool Engine::save(const string& pathString)
{
    QString path(pathString.data());
    return image.save(path);
}

const QImage& Engine::getImage() const
{
    return image;
}

void Engine::setCallbacks(
        PCALLBACK progCallback,
        SCALLBACK statCallback,
        void *callbackExtra_)
{
    progressCallback = progCallback;
    statusCallback = statCallback;
    callbackExtra = callbackExtra_;
}

void Engine::setArea(const QRect &rArea)
{
    area = rArea;
}

qreal sqr(qreal x)
{
    return x * x;
}

uint sqr(uint x)
{
    return x * x;
}

/*
 *Create Gauss filter matrix
 */
QVector<QVector<qreal> > Engine::createGauss(qreal sigma, uint size)
{
    QVector<QVector<qreal> > filterMatrix;
    if (size < 3)
        size = qMax(6 * sigma, 3.0);
    if (size % 2 == 0)
        size++;
    for (int i = 0; i < size; ++i)
    {
        filterMatrix.append(QVector<qreal>(size, 0));
        int x = - (int) size / 2 + i;
        for (int j = 0; j < size; ++j)
        {
            int y = - (int) size / 2 + i;
            filterMatrix[i][j] = qExp(-(x * x + y * y) / (2 * sigma * sigma)) /
                    (2 * M_PI * sigma * sigma);
        }
    }

    return filterMatrix;
}

/*
 * Create Gauss vector for Gauss filter
 */
QVector<qreal> Engine::createGaussVector(qreal sigma, uint size)
{
    if (size < 3)
    {
        size = qMax(6 * sigma, 3.0);
    }
    if (size % 2 == 0)
    {
        size++;
    }
    QVector<qreal> filter(size, 0);
    for (uint i = 0; i < size; ++i)
    {
        int x = -(int) size / 2 + i;
        filter[i] = qExp(-(x * x) / (2 * sigma * sigma)) / qSqrt(2 * M_PI) / sigma;
    }

    return filter;
}

/*
 * Sobel filter
 */
QVector<QVector<int> > Engine::createSobelX()
{
    QVector<QVector<int> > filter(3, QVector<int>(3, 0));
    for (uint i = 0; i < 3; ++i)
    {
        for (uint j = 0; j < 3; ++j)
        {
            filter[i][j] = -1 + j;
            if (i == 1)
                filter[i][j] *= 2;
        }
    }

    return filter;
}

QVector<QVector<int> > Engine::createSobelY()
{
    QVector<QVector<int> > filter(3, QVector<int>(3, 0));
    for (uint i = 0; i < 3; ++i)
    {
        for (uint j = 0; j < 3; ++j)
        {
            filter[i][j] = -1 + i;
            if (j == 1)
                filter[i][j] *= 2;
        }
    }

    return filter;
}

/*
 *--------------------------------------
 * Interpolation
 */

qreal Engine::linearInterpolation(
        qreal left,
        qreal right,
        qreal lValue,
        qreal rValue,
        qreal x)
{
    return ((right - x) * lValue + (x - left) * rValue) / (right - left);
}

qreal Engine::belinearInterpolation(
        QRect r,
        qreal ltValue,
        qreal lbValue,
        qreal rbValue,
        qreal rtValue,
        qreal x, qreal y)
{

    qreal r1 = linearInterpolation(r.left(), r.left() + r.width(), ltValue, rtValue, x);
    qreal r2 = linearInterpolation(r.left(), r.left() + r.width(), lbValue, rbValue, x);
    qreal result = linearInterpolation(r.top(), r.top() + r.height(), r1, r2, y);
    return result;
}

QRgb Engine::belinearColorInterpolation(
        QRect rt,
        QRgb ltValue,
        QRgb lbValue,
        QRgb rbValue,
        QRgb rtValue,
        qreal x, qreal y)
{
    int r = (int)belinearInterpolation(rt, qRed(ltValue),
                                       qRed(lbValue),
                                       qRed(rbValue),
                                       qRed(rtValue), x, y);
    int g = (int)belinearInterpolation(rt, qGreen(ltValue),
                                       qGreen(lbValue),
                                       qGreen(rbValue),
                                       qGreen(rtValue), x, y);
    int b = (int)belinearInterpolation(rt, qBlue(ltValue),
                                       qBlue(lbValue),
                                       qBlue(rbValue),
                                       qBlue(rtValue), x, y);
    return qRgb(r, g, b);
}

/*
 *--------------------------------------
 */

/*
 *Grayscale
 */
void Engine::toGrayscale()
{
    QImage imgNew(image);
    uint progPercent, progQuantum = area.width() * area.height() / 100 + 1;
    if (statusCallback)
        statusCallback("Converting image to grayscale...", callbackExtra);

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QRgb rgb = image.pixel(x, y);
            uint uiGray = 0.2125*qRed(rgb) + 0.7154*qGreen(rgb) + 0.0721*qBlue(rgb);
            imgNew.setPixel(x, y, qRgb(uiGray, uiGray, uiGray));
            progPercent = y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    image = imgNew;
    if (progressCallback) progressCallback(100, callbackExtra);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}

/*
 *Gray World
 */
void Engine::toGrayWorld()
{
    QImage imgNew(image);
    uint progPercent, progQuantum = area.width() * area.height() / 50 + 1;
    if (statusCallback)
        statusCallback("Using \"Gray World\"...", callbackExtra);

    qreal mRed = 0, mGreen = 0, mBlue = 0;
    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        QRgb *row = (QRgb *) image.scanLine(y);
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QRgb rgb = row[x];
            mRed += qRed(rgb);
            mGreen += qGreen(rgb);
            mBlue += qBlue(rgb);
            progPercent = y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    qreal avgColor = (mRed + mGreen + mBlue) / 3;

    mRed = avgColor / mRed;
    mGreen = avgColor / mGreen;
    mBlue = avgColor / mBlue;

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QRgb oRgb = image.pixel(x, y);
            QRgb nRgb = qRgb(qMin(qRed(oRgb) * mRed, 255.0),
                             qMin(qGreen(oRgb) * mGreen, 255.0),
                             qMin(qBlue(oRgb) * mBlue, 255.0));
            imgNew.setPixel(x, y, nRgb);
            progPercent = area.width() * area.height() + y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    image = imgNew;
    if (progressCallback) progressCallback(100, callbackExtra);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}

/*
 * Digital Invert
 */
void Engine::toInvert()
{
    QImage imgNew(image);
    uint progPercent, progQuantum = area.width() * area.height() / 100 + 1;
    if (statusCallback)
        statusCallback("Inverting colors...", callbackExtra);

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        QRgb *row = (QRgb *) image.scanLine(y);
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QRgb oRgb = row[x];
            uint ro = qRed(oRgb);
            uint go = qGreen(oRgb);
            uint bo = qBlue(oRgb);
            imgNew.setPixel(x, y, qRgb(255 - ro, 255 - go, 255 - bo));
            progPercent = y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    image = imgNew;
    if (progressCallback) progressCallback(100, callbackExtra);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}

/*
 * Waves
 */
void Engine::addWaves(qreal amplitudeX, qreal amplitudeY, qreal length)
{
    if (length < EPS || qAbs(amplitudeX + amplitudeY) < EPS)
    {
        if (progressCallback) progressCallback(100, callbackExtra);
        return;
    }
    QImage imgNew(image);
    uint progPercent, progQuantum = area.width() * area.height() / 100 + 1;
    if (statusCallback)
        statusCallback("Wave transforming...", callbackExtra);
    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        for (uint x = area.left(); x <= area.right(); ++x)
            imgNew.setPixel(x, y, qRgb(0, 0, 0));
    }

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            qreal phase = (amplitudeX * y + amplitudeY * x) / (amplitudeX + amplitudeY);
            qreal oldXF = x + amplitudeX * qSin(2 * M_PI * phase / length);
            qreal oldYF = y + amplitudeY * qSin(2 * M_PI * phase / length);
            int oldX = (int) oldXF;
            int oldY = (int) oldYF;

            if (area.contains(oldX, oldY, true))
            {
                imgNew.setPixel(x, y, belinearColorInterpolation(
                                    QRect(oldX - 1, oldY - 1, 2, 2),
                                    image.pixel(oldX - 1, oldY - 1),
                                    image.pixel(oldX - 1, oldY),
                                    image.pixel(oldX, oldY),
                                    image.pixel(oldX, oldY - 1),
                                    oldXF, oldYF));
            }
            progPercent = y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    image = imgNew;
    if (progressCallback) progressCallback(100, callbackExtra);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}

/*
 *--------------------------------------
 * Contrast fix
 */

/*
 *Auto Contrast
 */
void Engine::autoContrast()
{
    QImage imgNew(image);
    uint progPercent, progQuantum = area.width() * area.height() / 50 + 1;
    int yMin, yMax;
    QVector<uint> yLevels(256, 0);
    if (statusCallback)
        statusCallback("Stretching histogram...", callbackExtra);

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        QRgb *row = (QRgb *) image.scanLine(y);
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QRgb rgb = row[x];
            uint grayLevel = 0.2125*qRed(rgb) + 0.7154*qGreen(rgb) + 0.0721*qBlue(rgb);
            yLevels[grayLevel]++;
            progPercent = y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    uint limit = 0.05 * area.height() * area.width();
    uint sum = 0;
    for (yMin = 0; sum < limit; ++yMin)
        sum += yLevels[yMin];

    for (yMax = 255, sum = 0; sum < limit; --yMax)
        sum += yLevels[yMax];

    int yDiff = yMax - yMin;

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        QRgb *row = (QRgb *) image.scanLine(y);
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QRgb oRgb = row[x];
            QRgb nRgb = qRgb(255 * qBound(0, (int) qRed(oRgb) - yMin, yDiff) / yDiff,
                             255 * qBound(0, (int) qGreen(oRgb) - yMin, yDiff) / yDiff,
                             255 * qBound(0, (int) qBlue(oRgb) - yMin, yDiff) / yDiff);
            imgNew.setPixel(x, y, nRgb);
            progPercent = area.width() * area.height() + y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    image = imgNew;
    if (progressCallback) progressCallback(100, callbackExtra);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}


/*
 *Auto Levels
 */
void Engine::autoLevels()
{
    QImage imgNew(image);
    uint progPercent, progQuantum = area.width() * area.height() / 50 + 1;
    int yMin[3], yMax[3];
    QVector<QVector<uint> > yLevels(3, QVector<uint>(256, 0));
    if (statusCallback)
        statusCallback("Stretching channels' histograms.", callbackExtra);

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        QRgb *row = (QRgb *) image.scanLine(y);
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QRgb rgb = row[x];
            yLevels[Red][qRed(rgb)]++;
            yLevels[Green][qGreen(rgb)]++;
            yLevels[Blue][qBlue(rgb)]++;
            progPercent = y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    uint limit = 0.05 * area.height() * area.width();
    uint sum = 0;
    for (uint i = Red; i <= Blue; ++i)
        for (yMin[i] = 0, sum = 0; sum < limit; ++yMin[i])
            sum += yLevels[i][yMin[i]];

    for (uint i = Red; i <= Blue; ++i)
        for (yMax[i] = 255, sum = 0; sum < limit; --yMax[i])
            sum += yLevels[i][yMax[i]];

    int yDiff[3];
    for (uint i = Red; i <= Blue; ++i)
        yDiff[i] = yMax[i] - yMin[i];

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        QRgb *row = (QRgb *) image.scanLine(y);
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QRgb oRgb = row[x];
            QRgb nRgb = qRgb(255 * qBound(0, (int) qRed(oRgb) - yMin[Red], yDiff[Red]) /
                             yDiff[Red],
                             255 * qBound(0, (int) qGreen(oRgb) - yMin[Green], yDiff[Green]) /
                             yDiff[Green],
                             255 * qBound(0, (int) qBlue(oRgb) - yMin[Blue], yDiff[Blue]) /
                             yDiff[Blue]);
            imgNew.setPixel(x, y, nRgb);
            progPercent = area.width() * area.height() + y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    image = imgNew;
    if (progressCallback) progressCallback(100, callbackExtra);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}

/*
 *--------------------------------------
 *Geometry transforms
 */

/*
 *Rotate
 */
void Engine::rotate(qreal angleG)
{
    if (qAbs(angleG) < EPS)
    {
        if (progressCallback) progressCallback(100, callbackExtra);
        if (statusCallback)
            statusCallback("Done.", callbackExtra);
        return;
    }
    uint progPercent, progQuantum = area.width() * area.height() / 100 + 1;
    qreal angle = -angleG * M_PI / 180;
    QImage imgNew(image);
    if (statusCallback)
        statusCallback("Rotating image...", callbackExtra);

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        for (uint x = area.left(); x <= area.right(); ++x)
            imgNew.setPixel(x, y, qRgb(0, 0, 0));
    }

    QPointF center(area.left() + area.width() / 2.0, area.top() + area.height() / 2.0);

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QPointF relPoint = QPointF(x, y) - center;
            qreal oldXF = center.x() + relPoint.x() * qCos(angle) + relPoint.y() * qSin(angle);
            qreal oldYF = center.y() + relPoint.y() * qCos(angle) - relPoint.x() * qSin(angle);

            int oldX = (int) oldXF;
            int oldY = (int) oldYF;

            if (area.contains(oldX, oldY, true))
            {
                imgNew.setPixel(x, y, belinearColorInterpolation(
                                    QRect(oldX - 1, oldY - 1, 2, 2),
                                    image.pixel(oldX - 1, oldY - 1),
                                    image.pixel(oldX - 1, oldY),
                                    image.pixel(oldX, oldY),
                                    image.pixel(oldX, oldY - 1),
                                    oldXF, oldYF));
            }
            progPercent = y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    image = imgNew;
    if (progressCallback) progressCallback(100, callbackExtra);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}

/*
 *Scale
 */
void Engine::scale(qreal scaleRatio)
{
    scale(scaleRatio, scaleRatio);
}

/*
 *Free scale
 */
void Engine::scale(qreal xScale, qreal yScale)
{
    if (qAbs(xScale - 1) < EPS && qAbs(yScale - 1) < EPS)
    {
        if (progressCallback) progressCallback(100, callbackExtra);
        if (statusCallback)
            statusCallback("Done.", callbackExtra);
        return;
    }
    uint progPercent, progQuantum = area.width() * area.height() / 100 + 1;
    QImage imgNew(image);
    if (statusCallback)
        statusCallback("Scaling image...", callbackExtra);

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        for (uint x = area.left(); x <= area.right(); ++x)
            imgNew.setPixel(x, y, qRgb(0, 0, 0));
    }

    QPointF center(area.left() + area.width() / 2.0, area.top() + area.height() / 2.0);

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QPointF relPoint = QPointF(x, y) - center;
            qreal oldXF = center.x() + relPoint.x() / xScale;
            qreal oldYF = center.y() + relPoint.y() / yScale;
            int oldX = (int) oldXF;
            int oldY = (int) oldYF;
            if (area.contains(oldX, oldY, true))
            {
                imgNew.setPixel(x, y, belinearColorInterpolation(
                                    QRect(oldX - 1, oldY - 1, 2, 2),
                                    image.pixel(oldX - 1, oldY - 1),
                                    image.pixel(oldX - 1, oldY),
                                    image.pixel(oldX, oldY),
                                    image.pixel(oldX, oldY - 1),
                                    oldXF, oldYF));
            }
            progPercent = y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    image = imgNew;
    if (progressCallback) progressCallback(100, callbackExtra);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}

/*
 *--------------------------------------
 *Matrix filters
 */

/*
 * Median Filter
 */

void Engine::addMedianFilter(uint size)
{
    if (size %2 == 0)
        size++;
    else if (size < 3)
        size = 3;
    QImage imgNew(image);
    uint progPercent, progQuantum = area.width() * area.height() / 100 + 1;
    if (statusCallback)
        statusCallback("Bluring image with median filter...", callbackExtra);

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QImage neighbours = image.copy(x - size / 2, y - size / 2, size, size);
            QVector<QVector<uint> >channels(3, QVector<uint>(256, 0));
            for (uint i = 0; i < size; ++i)
            {
                QRgb *row = (QRgb *) neighbours.scanLine(i);
                for (uint j = 0; j < size; ++j)
                {
                    QRgb rgb = row[j];
                    channels[Red][qRed(rgb)]++;
                    channels[Green][qGreen(rgb)]++;
                    channels[Blue][qBlue(rgb)]++;
                }
            }
            QVector<uint> med(3);
            uint sum, limit = size * size / 2;
            for (uint i = Red; i <= Blue; ++i)
            {
                for (med[i] = 0, sum = 0; sum < limit; ++med[i])
                {
                    sum += channels[i][med[i]];
                }
                --med[i];
            }
            imgNew.setPixel(x, y, qRgb(med[Red], med[Green], med[Blue]));

            progPercent = y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    image = imgNew;
    if (progressCallback) progressCallback(100, callbackExtra);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}

/*
 * Gauss filter
 */
void Engine::addGaussFilter(qreal sigma, uint size)
{
    if (statusCallback)
        statusCallback("Bluring image with Gaussian blur...", callbackExtra);
    QVector<qreal> filter = createGaussVector(sigma, size);
    size = filter.size();
    QVector<QVector<qreal> > first(size, QVector<qreal>(1));
    QVector<QVector<qreal> > second(1, QVector<qreal>(size));
    for (uint i = 0; i < size; ++i)
    {
        first[i][0] = filter[i];
        second[0][i] = filter[i];
    }
    addMatrixFilter(second);
    addMatrixFilter(first);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}

/*
 * Free matrix filter
 */
void Engine::addMatrixFilter(const QVector<QVector<qreal> > &filter)
{
    if (filter.isEmpty())
    {
        if (progressCallback) progressCallback(100, callbackExtra);
        if (statusCallback)
            statusCallback("Done.", callbackExtra);
        return;
    }
    QImage imgNew(image);
    uint progPercent, progQuantum = area.width() * area.height() / 100 + 1;
    uint sizeX = filter.size();
    uint sizeY = filter[0].size();
    if (statusCallback)
        statusCallback("Applying custom matrix filter...", callbackExtra);

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QImage neighbours = image.copy(x - sizeX / 2, y - sizeY / 2, sizeX, sizeY);
            QVector<qreal> channels(3, 0.0);
            for (uint i = 0; i < sizeY; ++i)
            {
                QRgb *row = (QRgb *) neighbours.scanLine(i);
                for (uint j = 0; j < sizeX; ++j)
                {
                    QRgb rgb = row[j];
                    channels[Red] += qRed(rgb) * filter[sizeX - j - 1][sizeY - i - 1];
                    channels[Green] += qGreen(rgb) * filter[sizeX - j - 1][sizeY - i - 1];
                    channels[Blue] += qBlue(rgb) * filter[sizeX - j - 1][sizeY - i - 1];
                }
            }
            for (uint k = Red; k <= Blue; ++k)
            {
                channels[k] = qBound(0.0, channels[k], 255.0);
            }
            imgNew.setPixel(x, y,
                            qRgb(channels[Red], channels[Green], channels[Blue]));

            progPercent = y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    image = imgNew;
    if (progressCallback) progressCallback(100, callbackExtra);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}

/*
 * Unsharp
 */
void Engine::addUnsharp(qreal alpha, qreal sigma)
{
    if (statusCallback)
        statusCallback("Sharping image with Unsharp filter...", callbackExtra);
    QVector<QVector<qreal> > filterGauss = createGauss(sigma);
    QVector<QVector<qreal> > filter;
    for (uint i = 0; i < filterGauss.size(); ++i)
    {
        filter.append(QVector<qreal>(filterGauss.size(), 0));
        for (uint j = 0; j < filterGauss.size(); ++j)
        {
            filter[i][j] = -alpha * filterGauss[i][j];
            if (i == filterGauss.size() / 2 && j ==i)
            {
                filter[i][j] += (1 + alpha);
            }
        }
    }

    addMatrixFilter(filter);
}

/*
 * Sobel
 */
void Engine::addSobel()
{
    if (statusCallback)
        statusCallback("Finding edges with Sobel operator", callbackExtra);
    QVector<QVector<int> > xFilter = createSobelX();
    QVector<QVector<int> > yFilter = createSobelY();
    uint progPercent, progQuantum = area.width() * area.height() / 100 + 1;
    QImage imgNew(image);

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QImage neighbours = image.copy(x - 1, y - 1, 3, 3);
            QVector<qreal> channelsX(3, 0.0);
            QVector<qreal> channelsY(3, 0.0);
            for (uint i = 0; i < 3; ++i)
            {
                QRgb *row = (QRgb *) neighbours.scanLine(i);
                for (uint j = 0; j < 3; ++j)
                {
                    QRgb rgb = row[j];
                    channelsX[Red] += qRed(rgb) * xFilter[2 - j][2 - i];
                    channelsX[Green] += qGreen(rgb) * xFilter[2 - j][2 - i];
                    channelsX[Blue] += qBlue(rgb) * xFilter[2 - j][2 - i];
                    channelsY[Red] += qRed(rgb) * yFilter[2 - j][2 - i];
                    channelsY[Green] += qGreen(rgb) * yFilter[2 - j][2 - i];
                    channelsY[Blue] += qBlue(rgb) * yFilter[2 - j][2 - i];
                }
            }
            imgNew.setPixel(x, y, qRgb(
                                qMin(qSqrt(sqr(channelsX[Red]) + sqr(channelsY[Red])), 255.0),
                                qMin(qSqrt(sqr(channelsX[Green]) + sqr(channelsY[Green])), 255.0),
                                qMin(qSqrt(sqr(channelsX[Blue]) + sqr(channelsY[Blue])), 255.0)));

            progPercent = y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }
    image = imgNew;
    if (progressCallback) progressCallback(100, callbackExtra);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}

/*
 * One-direction sobel
 */
void Engine::addSobelOne()
{
    if (statusCallback)
        statusCallback("Applying one-direction Sobel operator...", callbackExtra);
    QVector<QVector<qreal> > filter(3, QVector<qreal>(3, 0));
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            filter[i][j] = -1 + j;
            if (i == 1)
                filter[i][j] *= 2;
        }
    }

    addMatrixFilter(filter);
}

/*
 * Emboss filter
 */
void Engine::toEmboss()
{
    if (statusCallback)
        statusCallback("Embossing image...", callbackExtra);
    QVector<QVector<int> > filter(3, QVector<int>(3, 0));
    filter[0][1] = 1;
    filter[1][0] = 1;
    filter[1][2] = -1;
    filter[2][1] = -1;

    QImage imgNew(image);
    uint progPercent, progQuantum = area.width() * area.height() / 100 + 1;

    for (uint y = area.top(); y <= area.bottom(); ++y)
    {
        for (uint x = area.left(); x <= area.right(); ++x)
        {
            QImage neighbours = image.copy(x - 1, y - 1, 3, 3);
            QVector<qreal> channels(3, 0.0);
            for (uint i = 0; i < 3; ++i)
            {
                QRgb *row = (QRgb *) neighbours.scanLine(i);
                for (uint j = 0; j < 3; ++j)
                {
                    QRgb rgb = row[j];
                    channels[Red] += qRed(rgb) * filter[2 - j][2 - i];
                    channels[Green] += qGreen(rgb) * filter[2 - j][2 - i];
                    channels[Blue] += qBlue(rgb) * filter[2 - j][2 - i];
                }
            }
            uint gray = 0.2125 * channels[Red] +
                    0.7154 * channels[Green] + 0.0721 * channels[Blue];
            gray = qBound(0, (int)gray + 128, 255);
            imgNew.setPixel(x, y, qRgb(gray, gray, gray));

            progPercent = y * area.width() + x;
            if (progPercent % progQuantum == 0 && progressCallback)
                progressCallback(progPercent / progQuantum, callbackExtra);
        }
    }

    image = imgNew;
    if (progressCallback) progressCallback(100, callbackExtra);
    if (statusCallback)
        statusCallback("Done.", callbackExtra);
}

/*
 * Parse matrix
 */
void Engine::addMatrixFilterNonParsed(const char *matrixString)
{
    QVector<double> filter;
    QString matrix(matrixString);

    matrix.remove("'");
    QStringList rowsList = matrix.split(";", QString::SkipEmptyParts);
    QVector<QString> rows = QVector<QString>::fromList(rowsList);
    for (uint i = 0; i < rows.size(); ++i)
    {
        QStringList itemsList = rows[i].split(",", QString::SkipEmptyParts);
        QVector<QString> items = QVector<QString>::fromList(itemsList);
        filter.append(items.size());
        for (uint j = 0; j < items.size(); ++j)
        {
            bool ok = false;
            filter[i * items.size() + j] = items[j].toDouble(&ok);
            if (!ok)
                return;
        }
    }

    addMatrixFilter(filter::data());
}
