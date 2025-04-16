#ifndef COLORLEVELS_H
#define COLORLEVELS_H

#include <QDialog>
#include <QImage>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QFutureWatcher>
#include <QDialogButtonBox>

class ColorLevelsDialog : public QDialog
{
    Q_OBJECT

public:
    ColorLevelsDialog(QImage& image, QWidget *parent = nullptr);
    ~ColorLevelsDialog();
    QImage getAdjustedImage() const;

    int getBlackPoint() const { return inputBlackSlider->value() + fineAdjustBlackSlider->value(); }
    int getWhitePoint() const { return inputWhiteSlider->value() + fineAdjustWhiteSlider->value(); }
    double getGamma() const { return inputGammaSlider->value() / 100.0; }
    int getCurrentChannel() const { return currentChannel; }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void updateHistogram();
    void onImageProcessingFinished();
    void channelChanged(int index);
    void updateLevels();
    void resetChannel();
    void pickBlackPoint();
    void pickWhitePoint();

private:
    void setupUI();
    void drawHistogram();
    void calculateHistogram();
    QFutureWatcher<QImage> *futureWatcher;
    QImage sampleImage(const QPoint& point);

    QSlider *fineAdjustBlackSlider;
    QSlider *fineAdjustWhiteSlider;

    QImage originalImage;
    QImage workingImage;

    // UI Elements
    QComboBox *channelCombo;
    QGraphicsScene *histogramScene;
    QGraphicsView *histogramView;
    QSlider *inputBlackSlider;
    QSlider *inputGammaSlider;
    QSlider *inputWhiteSlider;
    QSpinBox *blackSpinBox;
    QSpinBox *gammaSpinBox;
    QSpinBox *whiteSpinBox;
    QPushButton *resetButton;
    QPushButton *blackPickerButton;
    QPushButton *whitePickerButton;

    // Image view declaration
    QLabel *imageView;

    // Histogram data
    std::vector<int> histogram;
    bool isPickingBlack;
    bool isPickingWhite;
    bool isLogScale;
    int currentChannel; // 0=Value, 1=Red, 2=Green, 3=Blue

    static const int HISTOGRAM_WIDTH = 256;
    static const int HISTOGRAM_HEIGHT = 100;
};

#endif // COLORLEVELS_H
