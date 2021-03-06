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
#include "OperatorResultPropertiesPanel.h"

#include "ActiveObjects.h"
#include "DataSource.h"
#include "EditOperatorDialog.h"
#include "OperatorResult.h"
#include "Pipeline.h"
#include "Utilities.h"

#include "vtkMolecule.h"
#include "vtkPeriodicTable.h"

#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QVBoxLayout>

namespace tomviz {

OperatorResultPropertiesPanel::OperatorResultPropertiesPanel(QWidget* p)
  : QWidget(p)
{
  // Show active module in the "OperatorResult Properties" panel.
  connect(&ActiveObjects::instance(), SIGNAL(resultChanged(OperatorResult*)),
          SLOT(setOperatorResult(OperatorResult*)));

  // Set up a very simple layout with a description label widget.
  m_layout = new QVBoxLayout;
  setLayout(m_layout);
}

OperatorResultPropertiesPanel::~OperatorResultPropertiesPanel() = default;

void OperatorResultPropertiesPanel::setOperatorResult(OperatorResult* result)
{
  if (result != m_activeOperatorResult) {
    deleteLayoutContents(m_layout);
    if (result) {
      m_layout->addWidget(new QLabel(result->label()));

      if (vtkMolecule::SafeDownCast(result->dataObject())) {
        makeMoleculeProperties(vtkMolecule::SafeDownCast(result->dataObject()));
      }
    }
    m_layout->addStretch();
  }

  m_activeOperatorResult = result;
}

void OperatorResultPropertiesPanel::makeMoleculeProperties(
  vtkMolecule* molecule)
{
  auto table = initializeAtomTable();

  // Formula label
  auto speciesCount = moleculeSpeciesCount(molecule);
  QString formula;
  QMapIterator<QString, int> it(speciesCount);
  while (it.hasNext()) {
    it.next();
    formula += QString("%1<sub>%2 </sub>").arg(it.key()).arg(it.value());
  }
  auto formulaBox = new QGroupBox("Formula:");
  auto formulaLabel = new QLabel(formula);
  auto vbox = new QVBoxLayout;
  vbox->addWidget(formulaLabel);
  formulaBox->setLayout(vbox);

  // Button to save molecule to file
  auto saveButton = new QPushButton("Export to File");
  connect(saveButton, &QPushButton::clicked, this,
          [molecule]() { moleculeToFile(molecule); });

  // Button to show a table with individual atoms/positions
  // the table is lazily populated only if the user clicks on the button
  // to preserve resources in case thousands of atoms are part ot the molecule
  auto showButton = new QPushButton("Show Atoms Position");
  showButton->setCheckable(true);
  connect(showButton, &QPushButton::clicked, this,
          [this, table, molecule, showButton]() {
            if (table->rowCount() == 0) {
              populateAtomTable(table, molecule);
            }
            bool toggle = !table->isVisible();
            showButton->setChecked(toggle);
            table->setVisible(toggle);
          });

  m_layout->addWidget(formulaBox);
  m_layout->addWidget(saveButton);
  m_layout->addWidget(showButton);
  m_layout->addWidget(table);
}

QTableWidget* OperatorResultPropertiesPanel::initializeAtomTable()
{
  auto table = new QTableWidget();
  table->setRowCount(0);
  table->setColumnCount(4);
  QStringList labels;
  labels << "Symbol"
         << "X"
         << "Y"
         << "Z";
  table->setHorizontalHeaderLabels(labels);
  table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  table->setVisible(false);
  return table;
}

void OperatorResultPropertiesPanel::populateAtomTable(QTableWidget* table,
                                                      vtkMolecule* molecule)
{
  table->setRowCount(molecule->GetNumberOfAtoms());
  vtkNew<vtkPeriodicTable> periodicTable;
  for (int i = 0; i < molecule->GetNumberOfAtoms(); ++i) {
    vtkAtom atom = molecule->GetAtom(i);
    auto symbolItem = new QTableWidgetItem(
      QString(periodicTable->GetSymbol(atom.GetAtomicNumber())));
    symbolItem->setFlags(Qt::ItemIsEnabled);

    auto position = atom.GetPosition();
    auto xItem = new QTableWidgetItem(QString::number(position[0]));
    xItem->setFlags(Qt::ItemIsEnabled);
    auto yItem = new QTableWidgetItem(QString::number(position[1]));
    yItem->setFlags(Qt::ItemIsEnabled);
    auto zItem = new QTableWidgetItem(QString::number(position[2]));
    zItem->setFlags(Qt::ItemIsEnabled);

    table->setItem(i, 0, symbolItem);
    table->setItem(i, 1, xItem);
    table->setItem(i, 2, yItem);
    table->setItem(i, 3, zItem);
  }
}

QMap<QString, int> OperatorResultPropertiesPanel::moleculeSpeciesCount(
  vtkMolecule* molecule)
{
  QMap<QString, int> speciesCount;
  vtkNew<vtkPeriodicTable> periodicTable;
  for (int i = 0; i < molecule->GetNumberOfAtoms(); ++i) {
    vtkAtom atom = molecule->GetAtom(i);
    QString symbol = QString(periodicTable->GetSymbol(atom.GetAtomicNumber()));
    int count = speciesCount.value(symbol, 0);
    speciesCount[symbol] = count + 1;
  }
  return speciesCount;
}

} // namespace tomviz
