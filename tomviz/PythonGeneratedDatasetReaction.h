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
#ifndef tomvizPythonGeneratedDatasetReaction_h
#define tomvizPythonGeneratedDatasetReaction_h

#include <pqReaction.h>

namespace tomviz {
class DataSource;

class PythonGeneratedDatasetReaction : public pqReaction
{
  Q_OBJECT

public:
  PythonGeneratedDatasetReaction(QAction* parent, const QString& label,
                                 const QString& source);

  void addDataset();

  static DataSource* createDataSource(const QJsonObject& sourceInformation);

protected:
  void onTriggered() override { this->addDataset(); }

private:
  Q_DISABLE_COPY(PythonGeneratedDatasetReaction)

  QString m_scriptLabel;
  QString m_scriptSource;
};
} // namespace tomviz

#endif
