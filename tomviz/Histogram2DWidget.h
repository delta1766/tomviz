/******************************************************************************

  This source file is part of the tomviz project.

  Copyright Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/
#ifndef tomvizHistogram2DWidget_h
#define tomvizHistogram2DWidget_h

#include <QWidget>

#include <vtkNew.h>
#include <vtkRect.h>
#include <vtkSmartPointer.h>

/**
 * \brief Chart to edit a 2D transfer function (scalar value vs. gradient
 * magnitude).
 */

class vtkChartTransfer2DEditor;
class vtkContextView;
class vtkEventQtSlotConnect;
class vtkImageData;
class vtkTransferFunctionBoxItem;

namespace tomviz {

class QVTKGLWidget;

class Histogram2DWidget : public QWidget
{
  Q_OBJECT

public:
  explicit Histogram2DWidget(QWidget* parent_ = nullptr);
  ~Histogram2DWidget() override;

  /**
   * Set the computed 2D histogram.
   */
  void setHistogram(vtkImageData* histogram);

  /**
   * Add transfer function box items.  These items define a bounded section
   * in the lookup table. Each of them defines an RGBA transfer function.
   */
  void addFunctionItem(vtkSmartPointer<vtkTransferFunctionBoxItem> item);

  /**
   * Set the vtkImageData object into which the 2D transfer function will be
   * rastered from the available vtkTransferFunctionBoxItems and the vtkRectd
   * that will store the box points for this transfer function 2D.
   */
  void setTransfer2D(vtkImageData* transfer2D, vtkRectd* box);

public slots:
  void onTransfer2DChanged();

  /**
   * Force the chart to regenerate the transfer func.
   */
  void updateTransfer2D();

protected:
  void showEvent(QShowEvent* event) override;

  vtkNew<vtkChartTransfer2DEditor> m_chartHistogram2D;
  vtkNew<vtkContextView> m_histogramView;
  vtkNew<vtkEventQtSlotConnect> m_eventLink;

private:
  QVTKGLWidget* m_qvtk;
};
} // namespace tomviz
#endif // tomvizHistogram2DWidget_h
