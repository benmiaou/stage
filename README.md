# stage


Install :
- Requier QT4,ITK4.7
- cmake -DITK_DIR="Path to IDK build"



File Menu :
 - Open Directory : Open a directory that contain DICOM files.

Series Menu :
 - List DICOM series of the selected directory.

Processing Menu :
 - Show Edges : show edges.
 - Enhance Contrast : try to enhance contrast of the image.
 - Show histogram : Show the 3D histogram of the serie in a new window.
  - histogram window menu :
    - Apply threshold : apply threshold with the selected value with the scroll bars.
    - Show Threshold : Show the part of the histogram out of the thershold selection.
 -Show 3D View : Show 3D view of the current serie (use after threshold, only show pixel value > 220)

Commands :
 - MouseWheel or ArrowKeys : switch image.
 - <kbd>Shift</kbd> + left click : Place a rectangle zone.
 - Right click : Menu :
  - Enhance Region = Enhance contrast in the region under the cursor.
  - Select Zone = Zoom on the rectangle.
  - selectBrochi = perform an aproximative tubular segmentation in the region under the cursor.




