#include "colorlevels.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QPainter>
#include <QtConcurrent>
#include <QMouseEvent>
#include <QGraphicsPathItem>
#include <QDialogButtonBox>
#include <cmath>

ColorLevelsDialog::ColorLevelsDialog(QImage& image, QWidget *parent)
    : QDialog(parent)
    , originalImage(image)
    , workingImage(image)
    , isPickingBlack(false)
    , isPickingWhite(false)
    , isLogScale(false)
    , currentChannel(0)
{
    setupUI();
    calculateHistogram();
    drawHistogram();

    futureWatcher = new QFutureWatcher<QImage>(this);
    connect(futureWatcher, &QFutureWatcher<QImage>::finished, this, &ColorLevelsDialog::onImageProcessingFinished);
}

ColorLevelsDialog::~ColorLevelsDialog() {
    if (futureWatcher && futureWatcher->isRunning()) {
        futureWatcher->cancel();
        futureWatcher->waitForFinished();
    }
}

void ColorLevelsDialog::setupUI()
{
    setWindowTitle(tr("Adjust Color Levels"));
    setMinimumWidth(700);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Channel selection
    QHBoxLayout *channelLayout = new QHBoxLayout;
    QLabel *channelLabel = new QLabel(tr("Channel:"));
    channelCombo = new QComboBox;
    channelCombo->addItems(QStringList() << "Value" << "Red" << "Green" << "Blue");
    channelLayout->addWidget(channelLabel);
    channelLayout->addWidget(channelCombo);
    channelLayout->addStretch();

    // Fine adjustments group
    fineAdjustBlackSlider = new QSlider(Qt::Horizontal);
    fineAdjustWhiteSlider = new QSlider(Qt::Horizontal);
    fineAdjustBlackSlider->setRange(-10, 10);
    fineAdjustWhiteSlider->setRange(-10, 10);
    QGroupBox *fineAdjustGroup = new QGroupBox(tr("Fine Adjustments"));
    QGridLayout *fineAdjustLayout = new QGridLayout;
    fineAdjustLayout->addWidget(new QLabel(tr("Fine Adjust Black")), 0, 0);
    fineAdjustLayout->addWidget(fineAdjustBlackSlider, 0, 1);
    fineAdjustLayout->addWidget(new QLabel(tr("Fine Adjust White")), 1, 0);
    fineAdjustLayout->addWidget(fineAdjustWhiteSlider, 1, 1);
    fineAdjustGroup->setLayout(fineAdjustLayout);

    // Reset and picker buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    resetButton = new QPushButton(tr("Reset Channel"));
    blackPickerButton = new QPushButton(tr("Pick Black Point"));
    whitePickerButton = new QPushButton(tr("Pick White Point"));
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(blackPickerButton);
    buttonLayout->addWidget(whitePickerButton);
    buttonLayout->addStretch();

    // Histogram view
    histogramScene = new QGraphicsScene(this);
    histogramView = new QGraphicsView(histogramScene);
    histogramView->setFixedHeight(HISTOGRAM_HEIGHT + 50);
    histogramView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    histogramView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Input levels group
    QGroupBox *inputGroup = new QGroupBox(tr("Input Levels"));
    QGridLayout *inputLayout = new QGridLayout;
    inputBlackSlider = new QSlider(Qt::Horizontal);
    inputGammaSlider = new QSlider(Qt::Horizontal);
    inputWhiteSlider = new QSlider(Qt::Horizontal);
    blackSpinBox = new QSpinBox;
    gammaSpinBox = new QSpinBox;
    whiteSpinBox = new QSpinBox;
    inputBlackSlider->setRange(0, 255);
    inputGammaSlider->setRange(10, 1000);
    inputWhiteSlider->setRange(0, 255);
    blackSpinBox->setRange(0, 255);
    gammaSpinBox->setRange(10, 1000);
    whiteSpinBox->setRange(0, 255);
    inputBlackSlider->setValue(0);
    inputGammaSlider->setValue(100);
    inputWhiteSlider->setValue(255);
    blackSpinBox->setValue(0);
    gammaSpinBox->setValue(100);
    whiteSpinBox->setValue(255);
    inputLayout->addWidget(new QLabel(tr("Black Point")), 0, 0);
    inputLayout->addWidget(inputBlackSlider, 0, 1);
    inputLayout->addWidget(blackSpinBox, 0, 2);
    inputLayout->addWidget(new QLabel(tr("Gamma")), 1, 0);
    inputLayout->addWidget(inputGammaSlider, 1, 1);
    inputLayout->addWidget(gammaSpinBox, 1, 2);
    inputLayout->addWidget(new QLabel(tr("White Point")), 2, 0);
    inputLayout->addWidget(inputWhiteSlider, 2, 1);
    inputLayout->addWidget(whiteSpinBox, 2, 2);
    inputGroup->setLayout(inputLayout);

    // Image view
    imageView = new QLabel(this);
    imageView->setPixmap(QPixmap::fromImage(workingImage));
    imageView->setAlignment(Qt::AlignCenter);
    imageView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageView->setMinimumHeight(200);
    imageView->setMouseTracking(true);
    imageView->installEventFilter(this);

    // Dialog buttons
    QDialogButtonBox *dialogButtons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // Assemble the main layout in order
    mainLayout->addLayout(channelLayout);
    mainLayout->addWidget(fineAdjustGroup);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(histogramView);
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(imageView);
    mainLayout->addWidget(dialogButtons);

    // Connect signals for non-preset functionality
    connect(channelCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(channelChanged(int)));
    connect(resetButton, &QPushButton::clicked, this, &ColorLevelsDialog::resetChannel);
    connect(blackPickerButton, &QPushButton::clicked, this, &ColorLevelsDialog::pickBlackPoint);
    connect(whitePickerButton, &QPushButton::clicked, this, &ColorLevelsDialog::pickWhitePoint);
    connect(fineAdjustBlackSlider, &QSlider::valueChanged, this, &ColorLevelsDialog::updateLevels);
    connect(fineAdjustWhiteSlider, &QSlider::valueChanged, this, &ColorLevelsDialog::updateLevels);
    connect(inputBlackSlider, &QSlider::valueChanged, blackSpinBox, &QSpinBox::setValue);
    connect(inputGammaSlider, &QSlider::valueChanged, gammaSpinBox, &QSpinBox::setValue);
    connect(inputWhiteSlider, &QSlider::valueChanged, whiteSpinBox, &QSpinBox::setValue);
    connect(blackSpinBox, SIGNAL(valueChanged(int)), inputBlackSlider, SLOT(setValue(int)));
    connect(gammaSpinBox, SIGNAL(valueChanged(int)), inputGammaSlider, SLOT(setValue(int)));
    connect(whiteSpinBox, SIGNAL(valueChanged(int)), inputWhiteSlider, SLOT(setValue(int)));
    connect(inputBlackSlider, &QSlider::valueChanged, this, &ColorLevelsDialog::updateLevels);
    connect(inputGammaSlider, &QSlider::valueChanged, this, &ColorLevelsDialog::updateLevels);
    connect(inputWhiteSlider, &QSlider::valueChanged, this, &ColorLevelsDialog::updateLevels);
    connect(blackSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateLevels()));
    connect(gammaSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateLevels()));
    connect(whiteSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateLevels()));
    connect(dialogButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(dialogButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ColorLevelsDialog::calculateHistogram()
{
    histogram.clear();
    histogram.resize(HISTOGRAM_WIDTH, 0);

    for(int y = 0; y < workingImage.height(); ++y) {
        for(int x = 0; x < workingImage.width(); ++x) {
            QColor color = workingImage.pixelColor(x, y);
            int value;

            switch(currentChannel) {
            case 0: // Value
                value = qGray(color.rgb());
                break;
            case 1: // Red
                value = color.red();
                break;
            case 2: // Green
                value = color.green();
                break;
            case 3: // Blue
                value = color.blue();
                break;
            default:
                value = 0;
            }

            if(value >= 0 && value < HISTOGRAM_WIDTH) {
                histogram[value]++;
            }
        }
    }
}

void ColorLevelsDialog::drawHistogram()
{
    histogramScene->clear();

    // Find maximum value for scaling
    int maxValue = *std::max_element(histogram.begin(), histogram.end());
    if(maxValue == 0) return;

    // Create the histogram path
    QPainterPath path;
    path.moveTo(0, HISTOGRAM_HEIGHT);

    for(int i = 0; i < HISTOGRAM_WIDTH; ++i) {
        double value = histogram[i];
        if(isLogScale && value > 0) {
            value = std::log10(value);
        }
        double height = (value / maxValue) * HISTOGRAM_HEIGHT;
        path.lineTo(i, HISTOGRAM_HEIGHT - height);
    }

    path.lineTo(HISTOGRAM_WIDTH, HISTOGRAM_HEIGHT);
    path.closeSubpath();

    // Add the path to the scene
    QGraphicsPathItem *pathItem = histogramScene->addPath(path);
    pathItem->setPen(QPen(Qt::black));

    // Set the brush color based on the current channel
    QColor brushColor;
    switch(currentChannel) {
    case 0: brushColor = Qt::gray; break;
    case 1: brushColor = Qt::red; break;
    case 2: brushColor = Qt::green; break;
    case 3: brushColor = Qt::blue; break;
    case 4: brushColor = Qt::white; break;
    }
    pathItem->setBrush(brushColor);

    // Draw black and white point markers
    int blackPoint = inputBlackSlider->value();
    int whitePoint = inputWhiteSlider->value();

    QPen markerPen(Qt::black);
    markerPen.setWidth(2);

    histogramScene->addLine(blackPoint, 0, blackPoint, HISTOGRAM_HEIGHT, markerPen);
    markerPen.setColor(Qt::white);
    histogramScene->addLine(whitePoint, 0, whitePoint, HISTOGRAM_HEIGHT, markerPen);

    histogramScene->setSceneRect(0, 0, HISTOGRAM_WIDTH, HISTOGRAM_HEIGHT);
    histogramView->fitInView(histogramScene->sceneRect(), Qt::KeepAspectRatio);
}

void ColorLevelsDialog::updateHistogram()
{
    calculateHistogram();
    drawHistogram();
}

void ColorLevelsDialog::channelChanged(int index)
{
    currentChannel = index;
    updateLevels();
}

void ColorLevelsDialog::updateLevels()
{
    if (!inputBlackSlider || !inputWhiteSlider || !inputGammaSlider || !fineAdjustBlackSlider || !fineAdjustWhiteSlider)
        return;

    // Gather parameters from the UI.
    int blackPoint = inputBlackSlider->value() + fineAdjustBlackSlider->value();
    int whitePoint = inputWhiteSlider->value() + fineAdjustWhiteSlider->value();
    if (whitePoint <= blackPoint) {
        whitePoint = blackPoint + 1;
    }
    double gamma = inputGammaSlider->value() / 100.0;
    if (gamma <= 0) {
        gamma = 0.01;
    }
    int channel = currentChannel;

    // Copy the original image for processing.
    QImage src = originalImage;

    // If a previous processing task is still running, cancel it.
    if (futureWatcher->isRunning()) {
        futureWatcher->cancel();
        futureWatcher->waitForFinished();
    }

    // Offload the pixel processing to a separate thread.
    auto future = QtConcurrent::run([src, blackPoint, whitePoint, gamma, channel]() -> QImage {
        QImage result = src;
        for (int y = 0; y < result.height(); ++y) {
            QRgb *scanLine = reinterpret_cast<QRgb*>(result.scanLine(y));
            for (int x = 0; x < result.width(); ++x) {
                QColor color = QColor::fromRgb(scanLine[x]);
                int channels[4];
                channels[0] = color.red();
                channels[1] = color.green();
                channels[2] = color.blue();
                channels[3] = color.alpha();

                if (channel == 0) {  // Adjust all channels (value)
                    for (int i = 0; i < 3; ++i) {
                        double I = channels[i];
                        double O = ((I - blackPoint) / (whitePoint - blackPoint));
                        O = qBound(0.0, O, 1.0);
                        O = pow(O, 1.0 / gamma) * 255.0;
                        channels[i] = qBound(0, int(O + 0.5), 255);
                    }
                } else if (channel >= 1 && channel <= 3) {  // Adjust specific channel
                    int idx = channel - 1;
                    double I = channels[idx];
                    double O = ((I - blackPoint) / (whitePoint - blackPoint));
                    O = qBound(0.0, O, 1.0);
                    O = pow(O, 1.0 / gamma) * 255.0;
                    channels[idx] = qBound(0, int(O + 0.5), 255);
                }

                color.setRed(channels[0]);
                color.setGreen(channels[1]);
                color.setBlue(channels[2]);
                color.setAlpha(channels[3]);
                scanLine[x] = color.rgba();
            }
        }
        return result;
    });

    // Watch the asynchronous task.
    futureWatcher->setFuture(future);
}

void ColorLevelsDialog::onImageProcessingFinished()
{
    workingImage = futureWatcher->result();
    imageView->setPixmap(QPixmap::fromImage(workingImage));
    updateHistogram();
}

void ColorLevelsDialog::resetChannel()
{
    inputBlackSlider->setValue(0);
    inputGammaSlider->setValue(100);
    inputWhiteSlider->setValue(255);

    fineAdjustBlackSlider->setValue(0);
    fineAdjustWhiteSlider->setValue(0);

    workingImage = originalImage;
    imageView->setPixmap(QPixmap::fromImage(workingImage));
    updateHistogram();
}

void ColorLevelsDialog::pickBlackPoint()
{
    isPickingBlack = true;
    isPickingWhite = false;
    setCursor(Qt::CrossCursor);
}

void ColorLevelsDialog::pickWhitePoint()
{
    isPickingBlack = false;
    isPickingWhite = true;
    setCursor(Qt::CrossCursor);
}

QImage ColorLevelsDialog::getAdjustedImage() const
{
    return workingImage;
}

bool ColorLevelsDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == imageView && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        if (isPickingBlack || isPickingWhite)
        {
            QPoint pos = mouseEvent->pos();

            QPixmap pixmap = imageView->pixmap();

            QSize pixmapSize = pixmap.size();
            QSize labelSize = imageView->size();

            double scaleX = double(pixmapSize.width()) / labelSize.width();
            double scaleY = double(pixmapSize.height()) / labelSize.height();

            int imageX = pos.x() * scaleX;
            int imageY = pos.y() * scaleY;

            if (imageX >= 0 && imageX < workingImage.width() &&
                imageY >= 0 && imageY < workingImage.height())
            {
                QColor color = workingImage.pixelColor(imageX, imageY);

                int value;
                switch (currentChannel)
                {
                case 0: // Value
                    value = qGray(color.rgb());
                    break;
                case 1: // Red
                    value = color.red();
                    break;
                case 2: // Green
                    value = color.green();
                    break;
                case 3: // Blue
                    value = color.blue();
                    break;
                case 4: // Alpha
                    value = color.alpha();
                    break;
                default:
                    value = 0;
                }

                if (isPickingBlack)
                {
                    inputBlackSlider->setValue(value);
                    isPickingBlack = false;
                }
                else if (isPickingWhite)
                {
                    inputWhiteSlider->setValue(value);
                    isPickingWhite = false;
                }

                unsetCursor();
                return true;
            }
        }
    }

    return QDialog::eventFilter(obj, event);
}
