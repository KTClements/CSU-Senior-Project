#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include "colorlevels.h"
#include <QDialog>
#include <QStringList>
#include <QPixmap>


class QLabel;

class ImageDialog : public QDialog
{
    Q_OBJECT

public:
    ImageDialog(int index, QStringList imageFiles, QWidget *parent = nullptr);
    void loadImage(int index);
    void resetImage();

private slots:
    void rotateImage(int degrees);
    void flipImage(Qt::Orientation orientation);
    void adjustColorLevels();
    void saveImage();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    int currentIndex;
    QStringList imageFiles;
    QLabel *imageLabel;
    QPixmap originalImage;
    QPixmap currentImage;
    QPushButton *adjustColorLevelsButton;

    int rotationAngle;
    bool isFlippedHorizontally;
    bool isFlippedVertically;

    void applyTransformations();
};

#endif // IMAGEDIALOG_H
