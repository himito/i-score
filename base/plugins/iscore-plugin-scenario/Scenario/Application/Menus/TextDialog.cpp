#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLayout>
#include <QTextEdit>

#include "TextDialog.hpp"

class QWidget;
namespace Scenario
{
TextDialog::TextDialog(const QString& s, QWidget* parent) : QDialog{parent}
{
  this->setLayout(new QGridLayout);
  auto textEdit = new QTextEdit;
  textEdit->setPlainText(s);
  layout()->addWidget(textEdit);
  auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
  layout()->addWidget(buttonBox);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}
}
