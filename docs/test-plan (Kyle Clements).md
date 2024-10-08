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

A cross-platform bulk image manipulation software developed using the Qt C++ framework. The software offers a graphical user interface that allows users to manipulate multiple images simultaneously. Key features include resizing, cropping, format conversion, auto white balance, color correction, and undo/redo functionality. The application supports various image formats, including JPEG, PNG, and XPM.

### TEST PERSONNEL

- Lead Tester and Developer: Kyle Clements

### PROGRAMMERS

- Kyle Clements

## UNIT TEST SECTION

### UNIT TEST STRATEGY / EXTENT OF UNIT TESTING:

Evaluate new features and bug fixes introduced in this release. Unit testing focuses on individual functions such as image rotation, flipping, resizing, and format conversion to ensure they behave correctly under various inputs.

### UNIT TEST CASES

| \#  | OBJECTIVE                               | INPUT                     | EXPECTED RESULTS                    | TEST DELIVERABLES               |
| --- | --------------------------------------- | ------------------------ | ----------------------------------- | ------------------------------ |
| 1   | Test image rotation functionality       | Rotate image by 90 degrees      | Image should rotate correctly      | Test Report (rotation)         |
| 2   | Test image flipping (horizontal)        | Flip image horizontally  | Image should be flipped horizontally | Test Report (flipping)         |
| 3   | Test format conversion                  | Convert JPEG to PNG      | Image should be converted to PNG   | Test Report (conversion)       |
| 4   | Test batch processing                   | Process multiple images  | All images processed without error | Test Report (batch processing) |

## REGRESSION TEST SECTION

Ensure that previously developed and tested software still performs after changes.

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

Combine individual software modules and test as a group.

### INTEGRATION TEST STRATEGY AND EXTENT OF INTEGRATION TESTING

Evaluate all integrations between modules such as image dialog, main window, and the image processing backend. Test how these modules work together when loading, processing, and saving images.

### INTEGRATION TEST CASES

| #   | OBJECTIVE                           | INPUT                     | EXPECTED RESULTS                    | TEST DELIVERABLES               |
| --- | ----------------------------------- | ------------------------ | ----------------------------------- | ------------------------------ |
| 1   | Test image upload integration      | Load image files         | Images load correctly into GUI     | Integration Report (upload)    |
| 2   | Test rotation and save sequence    | Rotate and save image    | Image rotates and saves correctly  | Integration Report (save)      |
| 3   | Test batch export functionality    | Export images as PNG     | All images exported in chosen format | Integration Report (export)    |

## USER ACCEPTANCE TEST SECTION (To be completed by the business office)

Verify that the solution works for the user.

### USER ACCEPTANCE TEST STRATEGY

User acceptance testing will be conducted to verify that the software meets user requirements, including ease of use, functionality, and efficiency of image processing tasks. As the sole developer and tester, I will test the software to perform common tasks (batch resizing & applying auto white balance) and evaluate the outcome against the requirements.

### USER ACCEPTANCE TEST CASES

| #   | TEST ITEM                           | EXPECTED RESULTS                    | ACTUAL RESULTS                    | DATE                           |
| --- | ----------------------------------- | ----------------------------------- | -------------------------------- | ----------------------------- |
| 1   | Batch resize functionality         | All images resized as specified    | [To be filled during testing]    | [To be filled during testing] |
| 2   | Image format conversion            | Images correctly converted         | [To be filled during testing]    | [To be filled during testing] |
| 3   | Auto white balance functionality   | Image colors balanced correctly    | [To be filled during testing]    | [To be filled during testing] |

## Appendix

- [Project Proposal](https://github.com/KTClements/CSU-Senior-Project/blob/c0ed972d6b85b11a94c032435afdf2e0f1987cbd/docs/Final%20Proposal%20(Kyle%20Clements).md)
- [Requirements Document](https://github.com/KTClements/CSU-Senior-Project/blob/0dd8ef22b01a3bf9938bb4416ff5044b65439d13/docs/Requirements%20Document%20(Kyle%20Clements).pdf)
