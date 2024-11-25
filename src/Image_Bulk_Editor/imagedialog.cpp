#include "imagedialog.h"
#include "colorlevels.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QPixmap>
#include <QPushButton>
#include <QTransform>
#include <QTimer>

ImageDialog::ImageDialog(int index, QStringList files, QWidget *parent)
    : QDialog(parent), currentIndex(index), imageFiles(files),
    rotationAngle(0), isFlippedHorizontally(false), isFlippedVertically(false)
{
    // Set focus policy for the dialog
    setFocusPolicy(Qt::StrongFocus);

    imageLabel = new QLabel(this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(imageLabel);

    // Add manipulation buttons on the right side
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    // Rotate buttons
    QPushButton *rotate90Button = new QPushButton("Rotate 90°");
    QPushButton *rotate180Button = new QPushButton("Rotate 180°");
    QPushButton *rotate270Button = new QPushButton("Rotate 270°");

    // Set focus policy for all buttons to prevent them from taking focus
    rotate90Button->setFocusPolicy(Qt::NoFocus);
    rotate180Button->setFocusPolicy(Qt::NoFocus);
    rotate270Button->setFocusPolicy(Qt::NoFocus);

    connect(rotate90Button, &QPushButton::clicked, this, [=](){ rotateImage(90); });
    connect(rotate180Button, &QPushButton::clicked, this, [=](){ rotateImage(180); });
    connect(rotate270Button, &QPushButton::clicked, this, [=](){ rotateImage(270); });

    buttonLayout->addWidget(rotate90Button);
    buttonLayout->addWidget(rotate180Button);
    buttonLayout->addWidget(rotate270Button);

    // Flip buttons
    QPushButton *flipHButton = new QPushButton("Flip Horizontally");
    QPushButton *flipVButton = new QPushButton("Flip Vertically");

    // Set focus policy for flip buttons
    flipHButton->setFocusPolicy(Qt::NoFocus);
    flipVButton->setFocusPolicy(Qt::NoFocus);

    connect(flipHButton, &QPushButton::clicked, this, [=](){ flipImage(Qt::Horizontal); });
    connect(flipVButton, &QPushButton::clicked, this, [=](){ flipImage(Qt::Vertical); });

    buttonLayout->addWidget(flipHButton);
    buttonLayout->addWidget(flipVButton);

    // Adjust Color Levels Button
    adjustColorLevelsButton = new QPushButton("Adjust Color Levels");
    adjustColorLevelsButton->setFocusPolicy(Qt::NoFocus);
    connect(adjustColorLevelsButton, &QPushButton::clicked, this, &ImageDialog::adjustColorLevels);
    buttonLayout->addWidget(adjustColorLevelsButton);

    // Reset and save buttons
    QPushButton *resetButton = new QPushButton("Reset");
    QPushButton *saveButton = new QPushButton("Save");
    QPushButton *cancelButton = new QPushButton("Cancel");

    // Set focus policy for action buttons
    resetButton->setFocusPolicy(Qt::NoFocus);
    saveButton->setFocusPolicy(Qt::NoFocus);
    cancelButton->setFocusPolicy(Qt::NoFocus);

    connect(resetButton, &QPushButton::clicked, this, [=](){ resetImage(); });
    connect(saveButton, &QPushButton::clicked, this, &ImageDialog::saveImage);
    connect(cancelButton, &QPushButton::clicked, this, &ImageDialog::reject);

    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    layout->addLayout(buttonLayout);

    setLayout(layout);

    loadImage(currentIndex);
    resize(800, 600);

    // Ensure the dialog itself has focus when shown
    QTimer::singleShot(0, this, [=]() {
        setFocus();
        imageLabel->setPixmap(currentImage.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });
}

void ImageDialog::loadImage(int index)
{
    if (index >= 0 && index < imageFiles.size()) {
        originalImage.load(imageFiles.at(index));  // Load the full-size image
        currentImage = originalImage;  // Store the image for manipulation

        // Apply the current transformations to the newly loaded image
        applyTransformations();

        if (!currentImage.isNull()) {
            imageLabel->setPixmap(currentImage.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            this->setWindowTitle(QString("Image %1/%2").arg(index + 1).arg(imageFiles.size()));
        }
    }
}

void ImageDialog::rotateImage(int degrees)
{
    rotationAngle = (rotationAngle + degrees) % 360;  // Update the rotation angle, keep it in the range [0, 360)
    applyTransformations();
}

void ImageDialog::flipImage(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal) {
        isFlippedHorizontally = !isFlippedHorizontally;  // Toggle horizontal flip state
    } else if (orientation == Qt::Vertical) {
        isFlippedVertically = !isFlippedVertically;  // Toggle vertical flip state
    }
    applyTransformations();
}

void ImageDialog::resetImage()
{
    // Reset transformations
    rotationAngle = 0;
    isFlippedHorizontally = false;
    isFlippedVertically = false;

    // Reset currentImage to the original image
    currentImage = originalImage;

    // Update the display
    imageLabel->setPixmap(currentImage.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}



void ImageDialog::saveImage()
{
    // Save the current manipulated image in the same location or overwrite
    QString imagePath = imageFiles.at(currentIndex);  // Get the current image path
    currentImage.save(imagePath);  // Overwrite the original file with the current manipulated image

    accept();  // Close the dialog
}

void ImageDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right) {
        if (currentIndex < imageFiles.size() - 1) {
            currentIndex++;
            loadImage(currentIndex);  // Load the next image with current transformations
        }
    } else if (event->key() == Qt::Key_Left) {
        if (currentIndex > 0) {
            currentIndex--;
            loadImage(currentIndex);  // Load the previous image with current transformations
        }
    } else {
        QDialog::keyPressEvent(event);  // Call the base class implementation
    }
}

void ImageDialog::applyTransformations()
{
    // Start with the currentImage (which may have been adjusted)
    QPixmap transformedImage = currentImage;

    QTransform transform;

    // Apply rotation
    transform.rotate(rotationAngle);

    // Apply flips
    if (isFlippedHorizontally) {
        transform.scale(-1, 1);  // Flip horizontally
    }
    if (isFlippedVertically) {
        transform.scale(1, -1);  // Flip vertically
    }

    // Apply the transformations to the image
    transformedImage = transformedImage.transformed(transform, Qt::SmoothTransformation);

    // Update currentImage with the transformed image
    currentImage = transformedImage;

    // Display the transformed image
    imageLabel->setPixmap(currentImage.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


void ImageDialog::adjustColorLevels()
{
    // Convert currentImage to QImage
    QImage image = currentImage.toImage();

    // Create and show the ColorLevelsDialog
    ColorLevelsDialog colorLevelsDialog(image, this);
    if (colorLevelsDialog.exec() == QDialog::Accepted)
    {
        // Get the adjusted image
        QImage adjustedImage = colorLevelsDialog.getAdjustedImage();

        // Update currentImage with the adjusted image
        currentImage = QPixmap::fromImage(adjustedImage);

        // Apply any transformations (rotation, flips) that were previously set
        applyTransformations();

        // Update the display
        imageLabel->setPixmap(currentImage.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void ImageDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    imageLabel->setPixmap(currentImage.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
