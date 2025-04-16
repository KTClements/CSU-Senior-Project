#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QWidget>
#include <QListWidgetItem>
#include <QDockWidget>
#include <QImage>
#include <QString>
#include <QStringList>
#include <QMap>
#include "imagedialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    QImage performAutoWhiteBalance(const QImage &image);
    void bulkEditImages(int rotationDegrees = -1, bool flipHorizontal = false, bool flipVertical = false);
    void bulkSaveImages();
    void resetImages();
    void bulkAdjustColorLevels();
    void updateThumbnails();
    void onUploadButtonClicked();
    void onSaveButtonClicked();
    void onThumbnailClicked(QListWidgetItem *item);
    void showFormatDialog();
    void applyAutoWhiteBalance();
    void saveImages(const QString &format, int quality, const QString &resizeMode,
                    bool useCustomSize, int width, int height, bool isPercentage,
                    bool reverseOrientation, bool noEnlarge, bool maintainAspectRatio);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void onThumbnailClickedSingle(QListWidgetItem *item);
    void onThumbnailClickedDouble(QListWidgetItem *item);

    void showImage(int index);
    void showImageInSeparateWindow(int index);
    QLabel *previewLabel;
    QLabel *imageLabel;

    QDockWidget *createTransformationDock();
    QDockWidget *transformDock;
    QMap<QString, QImage> originalImages;
    Ui::MainWindow *ui;
    QStringList imageFiles;
    int currentImageIndex;
};

#endif // MAINWINDOW_H
