# stage


Install :
- Requier QT4,ITK4.7 DDGtal0.8
- cmake -DITK_DIR="Path to IDK build" -DDGtal_DIR="Path to DGtal build"



File Menu :
 - Open Directory : Open a directory that contain DICOM files.

Series Menu :
 - List DICOM series of the selected directory.

Processing Menu :
 - Show Edges : show edges.
 - Enhance Contrast : try to enhance contrast of the image.
 - Show histogram : Show the 3D histogram of the serie in a new window.
  - histogram Preview menu :
      - Preview Lung Segmentation : Preview the lung segmentation and the convex hull.
      - Preview Simple Threshold : Preview of a simple treshold.
  - histogram Apply menu :
      - Apply Lung Segmentation : Apply the lung segmentation and the convex hull.
      - Apply Simple Threshold : Apply of a simple treshold.
  - histogram Histogram Type menu :
      - Current 2D : Show histogram of the actual image.
      - Total 2D : Show 2D histogram of the actual serie.
      - 3D : Show 3D histogram of the actual serie.
 - Show 3D View : Show 3D view of the current serie.

Commands :
 - MouseWheel or ArrowKeys : switch image.
 - <kbd>Shift</kbd> + left click : Place a rectangle zone.
 - Right click : Menu :
  - Enhance Region = Enhance contrast in the region under the cursor.
  - Select Zone = Zoom on the rectangle.
  - selectBrochi = perform an aproximative tubular segmentation in the region under the cursor.




