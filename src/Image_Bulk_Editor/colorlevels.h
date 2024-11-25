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
#include <QDialogButtonBox>

class ColorLevelsDialog : public QDialog
{
    Q_OBJECT

public:
    ColorLevelsDialog(QImage& image, QWidget *parent = nullptr);
    QImage getAdjustedImage() const;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void updateHistogram();
    void channelChanged(int index);
    void updateLevels();
    void resetChannel();
    void pickBlackPoint();
    void pickWhitePoint();

private:
    void setupUI();
    void drawHistogram();
    void calculateHistogram();
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

    // Add imageView declaration
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
