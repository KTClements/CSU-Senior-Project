# Test Plan Document <!-- omit in toc -->

- [IDENTIFICATION INFORMATION SECTION](#identification-information-section)
  - [PRODUCT](#product)
  - [PROJECT DESCRIPTION](#project-description)
  - [TEST PERSONNEL](#test-personnel)
  - [PROGRAMMERS](#programmers)
- [UNIT TEST SECTION](#unit-test-section)
  - [UNIT TEST STRATEGY / EXTENT OF UNIT TESTING:](#unit-test-strategy--extent-of-unit-testing)
  - [UNIT TEST CASES](#unit-test-cases)
- [REGRESSION TEST SECTION](#regression-test-section)
  - [REGRESSION TEST STRATEGY](#regression-test-strategy)
  - [REGRESSION TEST CASES](#regression-test-cases)
- [INTEGRATION TEST SECTION](#integration-test-section)
  - [INTEGRATION TEST STRATEGY AND EXTENT OF INTEGRATION TESTING](#integration-test-strategy-and-extent-of-integration-testing)
  - [INTEGRATION TEST CASES](#integration-test-cases)
- [USER ACCEPTANCE TEST SECTION (To be completed by the business office)](#user-acceptance-test-section-to-be-completed-by-the-business-office)
  - [USER ACCEPTANCE TEST STRATEGY](#user-acceptance-test-strategy)
  - [USER ACCEPTANCE TEST CASES](#user-acceptance-test-cases)
- [Appendix](#appendix)

## IDENTIFICATION INFORMATION SECTION

### PRODUCT

- **Product Name:** Image Bulk Editor
- **Product Line Portfolio:** Image Processing Software

### PROJECT DESCRIPTION

A cross-platform bulk image manipulation software developed using the Qt C++ framework. The software offers a graphical user interface that allows users to manipulate multiple images simultaneously. Key features include resizing, cropping, format conversion, auto white balance, color correction, undo/redo functionality, and batch processing. The application supports various image formats, including JPEG, PNG, and XPM.

### TEST PERSONNEL

- Lead Tester and Developer: Kyle Clements

### PROGRAMMERS

- Kyle Clements

## UNIT TEST SECTION

### UNIT TEST STRATEGY / EXTENT OF UNIT TESTING:

Evaluate new features and bug fixes introduced in this release. Unit testing focuses on individual functions such as image rotation, flipping, resizing, color adjustments, and format conversion to ensure they behave correctly under various inputs.

### UNIT TEST CASES

| \#  | OBJECTIVE                                  | INPUT                               | EXPECTED RESULTS                          | TEST DELIVERABLES               |
| --- | ----------------------------------------- | ----------------------------------- | ---------------------------------------- | ------------------------------ |
| 1   | Test image rotation functionality         | Rotate image by 90 degrees         | Image should rotate correctly            | Test Report (rotation)         |
| 2   | Test image flipping (horizontal)          | Flip image horizontally            | Image should be flipped horizontally     | Test Report (flipping)         |
| 3   | Test image flipping (vertical)            | Flip image vertically              | Image should be flipped vertically       | Test Report (flipping)         |
| 4   | Test format conversion                    | Convert JPEG to PNG                | Image should be converted to PNG         | Test Report (conversion)       |
| 5   | Test auto white balance                   | High-contrast image input          | Colors adjusted for balanced white levels | Test Report (white balance)    |
| 6   | Test batch processing                     | Process multiple images            | All images processed without error       | Test Report (batch processing) |
| 7   | Test undo functionality                   | Undo last resize                   | Image restored to its previous state     | Test Report (undo/redo)        |
| 8   | Test redo functionality                   | Redo last undo                     | Redo restores the undone operation       | Test Report (undo/redo)        |
| 9   | Test unsupported format handling          | Upload BMP image                   | Error message displayed                  | Test Report (format validation)|
| 10  | Test resizing functionality               | Resize to 500x500 pixels           | Image resized to 500x500 pixels          | Test Report (resizing)         |
| 11  | Test percentage resizing                  | Resize to 50% of original size     | Image resized to half the original size  | Test Report (percentage resize)|
| 12  | Test maintain aspect ratio option         | Resize with maintain aspect ratio  | Image resized while maintaining aspect ratio | Test Report (aspect ratio)     |
| 13  | Test no-enlarge option                    | Resize smaller image with no-enlarge enabled | Image size remains unchanged            | Test Report (no-enlarge)       |
| 14  | Test histogram calculation for grayscale  | Load grayscale image               | Histogram calculated accurately          | Test Report (histogram calc)   |
| 15  | Test histogram calculation for RGB        | Load color image                   | Histogram calculated for all RGB channels | Test Report (histogram calc)   |
| 16  | Test black/white point selection          | Select black/white point on image  | Sliders updated with corresponding intensity values | Test Report (point selection) |
| 17  | Test fine adjustment sliders              | Adjust fine sliders for black/white points | Image adjusts accordingly               | Test Report (fine adjust)      |
| 18  | Test gamma correction                     | Adjust gamma to 1.5                | Image brightness adjusted appropriately  | Test Report (gamma correction) |
| 19  | Test drag-and-drop functionality          | Drag image into GUI                | Image loads successfully into the application | Test Report (drag-drop)       |
| 20  | Test invalid file path handling           | Load non-existent file             | Error message displayed                  | Test Report (file validation)  |
| 21  | Test rotation + flip sequence             | Rotate 90 degrees, then flip horizontally | Image transformed as expected           | Test Report (sequence testing) |
| 22  | Test batch export functionality           | Export images as PNG               | All images exported in chosen format     | Test Report (batch export)     |

## REGRESSION TEST SECTION

### REGRESSION TEST STRATEGY

Evaluate all previously tested features such as batch processing, image rotation, and format conversion to ensure that updates or bug fixes in the new release do not introduce new defects. Testing will involve retesting the functionality that has been successfully tested before.

### REGRESSION TEST CASES

| #   | OBJECTIVE                           | INPUT                     | EXPECTED RESULTS                    | OBSERVED                        |
| --- | ----------------------------------- | ------------------------ | ----------------------------------- | ----------------------------- |
| 1   | Validate batch processing          | Multiple image inputs    | All images processed correctly      | Successfully passed            |
| 2   | Validate image format conversion   | Convert images to PNG    | Images converted as expected       | Successfully passed            |
| 3   | Validate image flipping            | Flip image horizontally  | Image flipped correctly            | Successfully passed            |
| 4   | Validate image rotation            | Rotate image by 90 degrees      | Image rotated correctly            | Successfully passed            |

## INTEGRATION TEST SECTION

### INTEGRATION TEST STRATEGY AND EXTENT OF INTEGRATION TESTING

Evaluate all integrations between modules such as image dialog, main window, and the image processing backend. Test how these modules work together when loading, processing, and saving images.

### INTEGRATION TEST CASES

| #   | OBJECTIVE                           | INPUT                     | EXPECTED RESULTS                    | TEST DELIVERABLES               |
| --- | ----------------------------------- | ------------------------ | ----------------------------------- | ------------------------------ |
| 1   | Test image upload integration      | Load image files         | Images load correctly into GUI     | Integration Report (upload)    |
| 2   | Test rotation and save sequence    | Rotate and save image    | Image rotates and saves correctly  | Integration Report (save)      |
| 3   | Test batch export functionality    | Export images as PNG     | All images exported in chosen format | Integration Report (export)    |

## USER ACCEPTANCE TEST SECTION (To be completed by the business office)

### USER ACCEPTANCE TEST STRATEGY

User acceptance testing will be conducted to verify that the software meets user requirements, including ease of use, functionality, and efficiency of image processing tasks. As the sole developer and tester, I will test the software to perform common tasks (batch resizing & applying auto white balance) and evaluate the outcome against the requirements.

### USER ACCEPTANCE TEST CASES

| #   | TEST ITEM                           | EXPECTED RESULTS                          | ACTUAL RESULTS                    | DATE                           |
| --- | ----------------------------------- | ---------------------------------------- | -------------------------------- | ----------------------------- |
| 1   | Batch resize functionality         | All images resized as specified          | Successfully passed              | November 25th, 2024 |
| 2   | Image format conversion            | Images correctly converted               | Successfully passed              | November 25th, 2024 |
| 3   | Auto white balance functionality   | Image colors balanced correctly          | Successfully passed              | November 25th, 2024 |

## Appendix

- [Project Proposal](https://github.com/KTClements/CSU-Senior-Project/blob/c0ed972d6b85b11a94c032435afdf2e0f1987cbd/docs/Final%20Proposal%20(Kyle%20Clements).md)
- [Requirements Document](https://github.com/KTClements/CSU-Senior-Project/blob/0dd8ef22b01a3bf9938bb4416ff5044b65439d13/docs/Requirements%20Document%20(Kyle%20Clements).pdf)
