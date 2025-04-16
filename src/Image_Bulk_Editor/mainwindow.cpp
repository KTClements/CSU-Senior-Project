#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imagedialog.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QDir>
#include <QPixmap>
#include <QKeyEvent>
#include <QFileInfo>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QCloseEvent>
#include <QGroupBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QImage>
#include <QTransform>
#include <QProgressDialog>
#include <QPainter>
#include <QButtonGroup>
#include <QFileDialog>
#include <QProgressDialog>
#include <QDockWidget>
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentImageIndex(-1)
{
    ui->setupUi(this);

    // Create the dedicated preview label.
    previewLabel = new QLabel(this);
    previewLabel->setAlignment(Qt::AlignCenter);
    // Prevent previewLabel from intercepting mouse clicks.
    previewLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    // Add previewLabel to a specific layout area.
    if (ui->centralwidget->layout()) {
        ui->centralwidget->layout()->addWidget(previewLabel);
    } else {
        QVBoxLayout *layout = new QVBoxLayout(ui->centralwidget);
        layout->addWidget(previewLabel);
    }

    transformDock = createTransformationDock();
    addDockWidget(Qt::RightDockWidgetArea, transformDock);

    // Connect other signals.
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onUploadButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::onSaveButtonClicked);
    connect(ui->listWidget, &QListWidget::itemClicked,
            this, &MainWindow::onThumbnailClickedSingle);
    connect(ui->listWidget, &QListWidget::itemDoubleClicked,
            this, &MainWindow::onThumbnailClickedDouble);

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onUploadButtonClicked()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Select Image Folder");

    if (!folder.isEmpty()) {
        QDir dir(folder);
        QStringList filters;
        filters << "*.png" << "*.jpg" << "*.jpeg" << "*.xpm";
        dir.setNameFilters(filters);

        imageFiles.clear();
        ui->listWidget->clear();

        QStringList fileNames = dir.entryList(QDir::Files);
        for (const QString &fileName : fileNames) {
            QString imagePath = dir.absoluteFilePath(fileName);
            imageFiles << imagePath;

            QFileInfo fileInfo(imagePath);
            QString imageFileName = fileInfo.fileName();
            QString fileType = fileInfo.suffix();
            QDateTime creationDate = fileInfo.birthTime();
            QImage originalImage(imagePath);
            originalImages[imagePath] = originalImage;

            QListWidgetItem *item = new QListWidgetItem();

            QPixmap thumbnail(imagePath);
            if (!thumbnail.isNull()) {
                item->setIcon(QIcon(thumbnail.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
            }

            item->setText(imageFileName + " (" + fileType + "), Created: " + creationDate.toString());
            ui->listWidget->addItem(item);
        }

        if (!imageFiles.isEmpty()) {
            currentImageIndex = 0;
            showImage(currentImageIndex);
            this->setFocus();
        }
    }
}

void MainWindow::onSaveButtonClicked()
{
    showFormatDialog();
}

void MainWindow::saveImages(const QString &format, int quality, const QString &resizeMode,
                            bool useCustomSize, int width, int height, bool isPercentage,
                            bool reverseOrientation, bool noEnlarge, bool maintainAspectRatio)
{
    QString exportFolder = QFileDialog::getExistingDirectory(this, "Select Export Folder");
    if (exportFolder.isEmpty()) return;

    QFileInfo folderInfo(imageFiles.at(0));
    QString folderName = folderInfo.dir().dirName();
    QString newFolderName = exportFolder + "/" + folderName + " - (" + format + ")";

    QDir dir;
    if (!dir.exists(newFolderName)) {
        dir.mkpath(newFolderName);
    }

    QProgressDialog progress("Saving images...", "Cancel", 0, imageFiles.size(), this);
    progress.setWindowModality(Qt::WindowModal);

    for (int i = 0; i < imageFiles.size(); ++i) {
        if (progress.wasCanceled()) break;

        QString imagePath = imageFiles.at(i);
        QImage image(imagePath);

        if (!image.isNull()) {
            QSize originalSize = image.size();
            QSize newSize = originalSize;

            if (useCustomSize) {
                int targetWidth = width;
                int targetHeight = height;

                if (isPercentage) {
                    targetWidth = (originalSize.width() * width) / 100;
                    targetHeight = (originalSize.height() * height) / 100;
                }

                // Determine image orientation
                bool isLandscape = originalSize.width() > originalSize.height();

                if (reverseOrientation) {
                    if (resizeMode == "Stretch") {
                        // For Stretch mode with reverse orientation:
                        if (isLandscape) {
                            // Landscape: use height as width
                            newSize = QSize(targetHeight, targetWidth);
                        } else {
                            // Portrait: use width as height
                            newSize = QSize(targetWidth, targetHeight);
                        }
                    } else if (resizeMode == "Fit") {
                        // Keep existing Fit mode behavior
                        if (isLandscape) {
                            newSize = QSize(std::max(targetWidth, targetHeight),
                                            std::min(targetWidth, targetHeight));
                        } else {
                            newSize = QSize(std::min(targetWidth, targetHeight),
                                            std::max(targetWidth, targetHeight));
                        }

                        // Calculate the scaling factor to maintain aspect ratio
                        double scaleW = double(newSize.width()) / originalSize.width();
                        double scaleH = double(newSize.height()) / originalSize.height();
                        double scale = std::min(scaleW, scaleH);

                        // Apply the scaling while maintaining aspect ratio
                        newSize = QSize(originalSize.width() * scale,
                                        originalSize.height() * scale);
                    }
                } else {
                    // Original behavior when reverseOrientation is false
                    newSize = QSize(targetWidth, targetHeight);
                }

                // Apply no-enlarge constraint if enabled
                if (noEnlarge) {
                    if (newSize.width() > originalSize.width() ||
                        newSize.height() > originalSize.height()) {
                        newSize = originalSize;
                    }
                }
            }

            // Declare finalImage and create it based on the resize mode and maintainAspectRatio setting
            QImage finalImage;
            if (maintainAspectRatio) {
                // Maintain aspect ratio
                if (resizeMode == "Fit") {
                    finalImage = image.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                } else if (resizeMode == "Stretch") {
                    finalImage = image.scaled(newSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                }
            } else {
                // Ignore aspect ratio
                finalImage = image.scaled(newSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            }

            QFileInfo fileInfo(imagePath);
            QString newFileName = newFolderName + "/" + fileInfo.baseName() +
                                  "." + format.toLower();

            qDebug() << "Final dimensions for" << fileInfo.fileName() << ":"
                     << finalImage.size();

            if (!finalImage.save(newFileName, format.toUpper().toUtf8(), quality)) {
                qDebug() << "Failed to save image:" << newFileName;
            }
        }

        progress.setValue(i + 1);
    }
}


void MainWindow::onThumbnailClicked(QListWidgetItem *item)
{
    int index = ui->listWidget->row(item);

    showImage(index);
    showImageInSeparateWindow(index);
}

void MainWindow::onThumbnailClickedSingle(QListWidgetItem *item)
{
    int index = ui->listWidget->row(item);
    if (index >= 0 && index < imageFiles.size()) {
        showImage(index);
    }
}

void MainWindow::onThumbnailClickedDouble(QListWidgetItem *item)
{
    int index = ui->listWidget->row(item);
    showImageInSeparateWindow(index);
}

void MainWindow::showImageInSeparateWindow(int index)
{
    ImageDialog *dialog = new ImageDialog(index, imageFiles, this);
    dialog->exec();
    delete dialog;
}

void MainWindow::showFormatDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Select Format and Resize Options");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    // Format selection
    QLabel *formatLabel = new QLabel("Choose export format:");
    layout->addWidget(formatLabel);

    QStringList formats = {"JPEG", "PNG", "XPM"};
    QComboBox *formatComboBox = new QComboBox;
    formatComboBox->addItems(formats);
    layout->addWidget(formatComboBox);

    // Quality Slider (for JPEG)
    QLabel *qualityLabel = new QLabel("Quality:");
    layout->addWidget(qualityLabel);

    QSlider *qualitySlider = new QSlider(Qt::Horizontal);
    qualitySlider->setRange(1, 100);
    qualitySlider->setValue(90);
    layout->addWidget(qualitySlider);

    // Resize Options
    QGroupBox *resizeGroupBox = new QGroupBox("Resize Options");
    QVBoxLayout *resizeLayout = new QVBoxLayout(resizeGroupBox);

    // Resize mode buttons
    QButtonGroup *resizeModeGroup = new QButtonGroup(&dialog);
    QHBoxLayout *resizeModeLayout = new QHBoxLayout();

    QStringList modes = {"Fit", "Stretch"};
    for (const QString &mode : modes) {
        QPushButton *button = new QPushButton(mode);
        button->setCheckable(true);
        resizeModeGroup->addButton(button);
        resizeModeLayout->addWidget(button);

        // Set "Fit" as default selected
        if (mode == "Fit") {
            button->setChecked(true);
        }
    }
    resizeLayout->addLayout(resizeModeLayout);

    // Custom size / Percentage of original
    QRadioButton *customSizeRadio = new QRadioButton("Custom size / Percentage of original");
    resizeLayout->addWidget(customSizeRadio);

    QHBoxLayout *customSizeLayout = new QHBoxLayout;
    QSpinBox *widthSpinBox = new QSpinBox;
    QSpinBox *heightSpinBox = new QSpinBox;
    QComboBox *unitComboBox = new QComboBox;

    widthSpinBox->setRange(1, 10000);
    heightSpinBox->setRange(1, 10000);
    widthSpinBox->setValue(500);  // Set default value to 500
    heightSpinBox->setValue(500); // Set default value to 500
    unitComboBox->addItems({"px", "%"});

    customSizeLayout->addWidget(new QLabel("Width"));
    customSizeLayout->addWidget(widthSpinBox);
    customSizeLayout->addWidget(new QLabel("x"));
    customSizeLayout->addWidget(new QLabel("Height"));
    customSizeLayout->addWidget(heightSpinBox);
    customSizeLayout->addWidget(unitComboBox);

    resizeLayout->addLayout(customSizeLayout);

    // Preset size
    QRadioButton *presetSizeRadio = new QRadioButton("Preset size");
    resizeLayout->addWidget(presetSizeRadio);
    presetSizeRadio->setChecked(true);  // Pre-select the Preset size radio button

    // Make a button group for the radio buttons
    QButtonGroup *sizeButtonGroup = new QButtonGroup(&dialog);
    sizeButtonGroup->addButton(customSizeRadio);
    sizeButtonGroup->addButton(presetSizeRadio);

    // Additional options
    QCheckBox *reverseOrientationCheck = new QCheckBox("Reverse Width and Height by Orientation");
    QCheckBox *noEnlargeCheck = new QCheckBox("Do not enlarge if smaller");
    QCheckBox *maintainAspectRatioCheck = new QCheckBox("Maintain Aspect Ratio");

    resizeLayout->addWidget(reverseOrientationCheck);
    resizeLayout->addWidget(noEnlargeCheck);
    resizeLayout->addWidget(maintainAspectRatioCheck);

    layout->addWidget(resizeGroupBox);

    // OK and Cancel buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    // Connect signals for disabling/enabling options based on selected mode
    connect(resizeModeGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            [=](QAbstractButton *button) {
                if (button->text() == "Fit") {
                    maintainAspectRatioCheck->setChecked(true);
                    maintainAspectRatioCheck->setEnabled(false);
                } else if (button->text() == "Stretch") {
                    maintainAspectRatioCheck->setChecked(false);
                    maintainAspectRatioCheck->setEnabled(false);
                } else {
                    maintainAspectRatioCheck->setEnabled(true);
                }
            });

    connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
        QString selectedFormat = formatComboBox->currentText().toLower();
        int quality = qualitySlider->value();
        QString resizeMode = resizeModeGroup->checkedButton() ? resizeModeGroup->checkedButton()->text() : "";
        bool useCustomSize = customSizeRadio->isChecked();
        int width = widthSpinBox->value();
        int height = heightSpinBox->value();
        bool isPercentage = unitComboBox->currentText() == "%";
        bool reverseOrientation = reverseOrientationCheck->isChecked();
        bool noEnlarge = noEnlargeCheck->isChecked();
        bool maintainAspectRatio = maintainAspectRatioCheck->isChecked();

        saveImages(selectedFormat, quality, resizeMode, useCustomSize, width, height, isPercentage,
                   reverseOrientation, noEnlarge, maintainAspectRatio);
        dialog.accept();
    });

    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    maintainAspectRatioCheck->setChecked(true);
    maintainAspectRatioCheck->setEnabled(false);

    dialog.exec();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left) {
        if (currentImageIndex > 0) {
            currentImageIndex--;
            showImageInSeparateWindow(currentImageIndex);
        }
    } else if (event->key() == Qt::Key_Right) {
        if (currentImageIndex < imageFiles.size() - 1) {
            currentImageIndex++;
            showImageInSeparateWindow(currentImageIndex);
        }
    }
}

void MainWindow::showImage(int index)
{
    if (index >= 0 && index < imageFiles.size()) {
        QPixmap image(imageFiles.at(index));
        if (!image.isNull()) {
            previewLabel->setPixmap(
                image.scaled(previewLabel->size(),
                             Qt::KeepAspectRatio,
                             Qt::SmoothTransformation)
                );
        }
    }
}


void MainWindow::bulkEditImages(int rotationDegrees, bool flipHorizontal, bool flipVertical)
{
    if (imageFiles.isEmpty()) {
        return;
    }

    QProgressDialog progress("Applying edits...", "Cancel", 0, imageFiles.size(), this);
    progress.setWindowModality(Qt::WindowModal);

    for (int i = 0; i < imageFiles.size(); ++i) {
        if (progress.wasCanceled()) {
            break;
        }

        QImage image(imageFiles[i]);
        if (!image.isNull()) {
            if (rotationDegrees != -1) {
                QTransform transform;
                transform.rotate(rotationDegrees);
                image = image.transformed(transform);
            }
            if (flipHorizontal) {
                image = image.mirrored(true, false);
            }
            if (flipVertical) {
                image = image.mirrored(false, true);
            }
            image.save(imageFiles[i]);
        }
        progress.setValue(i + 1);
    }

    updateThumbnails();
}

void MainWindow::bulkSaveImages()
{
    // Use default values for the new parameters
    saveImages("jpg", 90, "Fit", true, 100, 100, true, false, false, true);
}

void MainWindow::resetImages()
{
    if (imageFiles.isEmpty()) {
        return;
    }

    QProgressDialog progress("Resetting images...", "Cancel", 0, imageFiles.size(), this);
    progress.setWindowModality(Qt::WindowModal);

    for (int i = 0; i < imageFiles.size(); ++i) {
        if (progress.wasCanceled()) {
            break;
        }

        QString imagePath = imageFiles[i];
        if (originalImages.contains(imagePath)) {
            // Restore the original image
            originalImages[imagePath].save(imagePath);
        }

        progress.setValue(i + 1);
    }

    // Refresh thumbnails
    updateThumbnails();

    if (currentImageIndex >= 0 && currentImageIndex < imageFiles.size()) {
        showImage(currentImageIndex);
    }
}

void MainWindow::updateThumbnails()
{
    ui->listWidget->clear();

    for (const QString &imagePath : imageFiles) {
        QFileInfo fileInfo(imagePath);
        QString imageFileName = fileInfo.fileName();
        QString fileType = fileInfo.suffix();
        QDateTime creationDate = fileInfo.birthTime();

        QListWidgetItem *item = new QListWidgetItem();

        QPixmap thumbnail(imagePath);
        if (!thumbnail.isNull()) {
            item->setIcon(QIcon(thumbnail.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
        }

        item->setText(imageFileName + " (" + fileType + "), Created: " + creationDate.toString());
        ui->listWidget->addItem(item);
    }
}

void MainWindow::applyAutoWhiteBalance()
{
    if (imageFiles.isEmpty()) {
        return;
    }

    QProgressDialog progress("Applying auto white balance...", "Cancel", 0, imageFiles.size(), this);
    progress.setWindowModality(Qt::WindowModal);

    for (int i = 0; i < imageFiles.size(); ++i) {
        if (progress.wasCanceled()) {
            break;
        }

        QString imagePath = imageFiles.at(i);
        QImage image(imagePath);

        if (!image.isNull()) {
            // Auto white balance logic
            image = performAutoWhiteBalance(image);

            // Save the updated image back to its path
            image.save(imagePath);
        }

        progress.setValue(i + 1);
    }

    updateThumbnails();
}

QImage MainWindow::performAutoWhiteBalance(const QImage &image)
{
    QImage balancedImage = image.convertToFormat(QImage::Format_ARGB32);
    int redSum = 0, greenSum = 0, blueSum = 0;
    int pixelCount = 0;

    for (int y = 0; y < balancedImage.height(); ++y) {
        QRgb *line = (QRgb *)balancedImage.scanLine(y);
        for (int x = 0; x < balancedImage.width(); ++x) {
            QColor color(line[x]);
            redSum += color.red();
            greenSum += color.green();
            blueSum += color.blue();
            pixelCount++;
        }
    }

    // Calculate averages
    double avgRed = redSum / (double)pixelCount;
    double avgGreen = greenSum / (double)pixelCount;
    double avgBlue = blueSum / (double)pixelCount;

    // Scaling factors for white balance adjustment
    double scaleRed = 128.0 / avgRed;
    double scaleGreen = 128.0 / avgGreen;
    double scaleBlue = 128.0 / avgBlue;

    for (int y = 0; y < balancedImage.height(); ++y) {
        QRgb *line = (QRgb *)balancedImage.scanLine(y);
        for (int x = 0; x < balancedImage.width(); ++x) {
            QColor color(line[x]);
            int newRed = qBound(0, int(color.red() * scaleRed), 255);
            int newGreen = qBound(0, int(color.green() * scaleGreen), 255);
            int newBlue = qBound(0, int(color.blue() * scaleBlue), 255);
            line[x] = qRgba(newRed, newGreen, newBlue, color.alpha());
        }
    }

    return balancedImage;
}

void MainWindow::bulkAdjustColorLevels()
{
    if (imageFiles.isEmpty()) return;

    // Use the first image as a sample for adjustment.
    QImage sampleImage(imageFiles.first());
    if (sampleImage.isNull()) return;

    // Open the color levels dialog for parameter selection.
    ColorLevelsDialog dialog(sampleImage, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    // Get the user-selected parameters.
    int blackPoint = dialog.getBlackPoint();
    int whitePoint = dialog.getWhitePoint();
    double gamma = dialog.getGamma();
    int channel = dialog.getCurrentChannel();

    // Optionally, you might want to show a progress dialog.
    QProgressDialog progress("Applying color adjustments...", "Cancel", 0, imageFiles.size(), this);
    progress.setWindowModality(Qt::WindowModal);

    // For each image file, load, adjust, and save.
    for (int i = 0; i < imageFiles.size(); ++i) {
        if (progress.wasCanceled()) break;

        QString imagePath = imageFiles.at(i);
        QImage image(imagePath);
        if (image.isNull()) continue;

        // Apply the color adjustment (using a helper lambda function):
        auto adjustImage = [=](const QImage &src) -> QImage {
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

                    if (channel == 0) {
                        for (int j = 0; j < 3; ++j) {
                            double I = channels[j];
                            double O = ((I - blackPoint) / (whitePoint - blackPoint));
                            O = qBound(0.0, O, 1.0);
                            O = pow(O, 1.0 / gamma) * 255.0;
                            channels[j] = qBound(0, int(O + 0.5), 255);
                        }
                    } else if (channel >= 1 && channel <= 3) {
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
        };

        QImage adjusted = adjustImage(image);
        // Save the adjusted image (you might want to backup originals)
        adjusted.save(imagePath);
        progress.setValue(i + 1);
    }

    // Refresh the thumbnails after bulk processing.
    updateThumbnails();
}

QDockWidget* MainWindow::createTransformationDock()
{
    QDockWidget *dock = new QDockWidget(tr("Image Transformations"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    // Create a widget to hold all the transformation controls
    QWidget *transformWidget = new QWidget(dock);
    QVBoxLayout *layout = new QVBoxLayout(transformWidget);

    // Create a group box for color adjustments
    QGroupBox *colorGroup = new QGroupBox("Colors", transformWidget);
    QVBoxLayout *colorLayout = new QVBoxLayout(colorGroup);

    // Create a button for auto-white balance
    QPushButton *autoWhiteBalanceButton = new QPushButton("Auto White Balance", colorGroup);
    colorLayout->addWidget(autoWhiteBalanceButton);
    colorLayout->addStretch();

    QPushButton *bulkColorAdjustButton = new QPushButton("Color Adjust All", colorGroup);
    colorLayout->addWidget(bulkColorAdjustButton);

    // Add the color group to the main layout
    layout->addWidget(colorGroup);

    // Connect signals
    connect(autoWhiteBalanceButton, &QPushButton::clicked, this, &MainWindow::applyAutoWhiteBalance);

    // Create a group box for rotation buttons
    QGroupBox *rotateGroup = new QGroupBox("Rotate", transformWidget);
    QVBoxLayout *rotateLayout = new QVBoxLayout(rotateGroup);

    // Create and add rotation buttons
    QPushButton *rotate90 = new QPushButton("Rotate 90°", rotateGroup);
    QPushButton *rotate180 = new QPushButton("Rotate 180°", rotateGroup);
    QPushButton *rotate270 = new QPushButton("Rotate 270°", rotateGroup);

    rotateLayout->addWidget(rotate90);
    rotateLayout->addWidget(rotate180);
    rotateLayout->addWidget(rotate270);
    rotateLayout->addStretch();

    // Create a group box for flip buttons
    QGroupBox *flipGroup = new QGroupBox("Flip", transformWidget);
    QVBoxLayout *flipLayout = new QVBoxLayout(flipGroup);

    // Create and add flip buttons
    QPushButton *flipHor = new QPushButton("Flip Horizontally", flipGroup);
    QPushButton *flipVer = new QPushButton("Flip Vertically", flipGroup);

    flipLayout->addWidget(flipHor);
    flipLayout->addWidget(flipVer);
    flipLayout->addStretch();

    // Create reset button
    QPushButton *resetButton = new QPushButton("Reset", transformWidget);

    // Add all groups to the main layout
    layout->addWidget(rotateGroup);
    layout->addWidget(flipGroup);
    layout->addWidget(resetButton);
    layout->addStretch();

    // Connect signals
    connect(rotate90, &QPushButton::clicked, this, [this](){ bulkEditImages(90); });
    connect(rotate180, &QPushButton::clicked, this, [this](){ bulkEditImages(180); });
    connect(rotate270, &QPushButton::clicked, this, [this](){ bulkEditImages(270); });
    connect(flipHor, &QPushButton::clicked, this, [this](){ bulkEditImages(-1, true, false); });
    connect(flipVer, &QPushButton::clicked, this, [this](){ bulkEditImages(-1, false, true); });
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetImages);
    connect(bulkColorAdjustButton, &QPushButton::clicked, this, &MainWindow::bulkAdjustColorLevels);


    // Set the widget as the dock widget's content
    dock->setWidget(transformWidget);

    return dock;
}
