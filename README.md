# Bulk Image Manipulation Tool

The project aims to develop a cross-platform bulk image manipulation tool using C++ and the Qt framework. It will feature a user-friendly graphical interface with advanced capabilities like auto-white balance, color correction, noise reduction, and batch processing. By integrating the OpenCV library for image processing, the tool will enable efficient editing operations such as resizing, cropping, format conversion, and undoing and redoing multiple images simultaneously. This comprehensive solution will streamline image editing workflows, enhancing productivity and output quality for professionals and enthusiasts in fields like digital marketing, photography, and graphic design.

## Compile/Deploy

```bash
qmake
make
./BulkImageTool
```

> Requires Qt 5 or 6 and a C++17-compatible compiler (g++ or MSVC).  
> Tested on Windows 10 using Qt Creator and MinGW toolchain.

## Usage

- Launch the app using the executable.
- Click **Select Folder** to load all images in a directory.
- Select an image to preview it below.
- Use the right panel to apply transformations:
  - Rotate (90°, 180°, 270°)
  - Flip (Horizontal/Vertical)
  - Auto White Balance
  - Color Level Adjustment (Histogram/Gamma)
- Click **Export** to save images in bulk with custom size, format, and quality.

## Options

- **Auto White Balance**: Automatically corrects image tone by adjusting pixel RGB balance.
- **Histogram Adjustment**: Fine-tune black/white points and gamma with real-time preview.
- **Export Settings**:
  - Format: JPEG, PNG, XPM
  - Resize: Custom pixel or percent size
  - Preserve aspect ratio / Prevent upscaling
- **Keyboard Navigation**: Use ← and → to switch between images in preview mode.

## Testing

Testing was performed manually and included:

- Unit tests for rotation, flip, gamma correction, resizing, and format conversion
- Regression testing with over 100+ images to ensure stability
- User acceptance testing to validate that exported images match preview settings
- Thread safety testing by canceling image previews mid-edit to ensure no crashes occur

> Threading logic verified using `QFutureWatcher` + `QtConcurrent` to prevent UI freezing and ensure real-time previews are accurate and safe.
