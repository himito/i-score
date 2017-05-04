#include <QFile>
#include <QMap>
#include <QSettings>
#include <QStringList>
#include <QVariant>
#include <core/application/OpenDocumentsFile.hpp>

#include "Document.hpp"
#include "DocumentBackupManager.hpp"
#include <core/application/CommandBackupFile.hpp>

iscore::DocumentBackupManager::DocumentBackupManager(iscore::Document& doc)
    : QObject{&doc}
{
  m_modelFile.open();

  m_commandFile = new CommandBackupFile{doc.commandStack(), this};
}

iscore::DocumentBackupManager::~DocumentBackupManager()
{
  QSettings s(OpenDocumentsFile::path(), QSettings::IniFormat);
  auto existing_files = s.value("iscore/docs").toStringList();
  existing_files.removeOne(crashDataFile().fileName());
  s.setValue("iscore/docs", existing_files);

  QFile(crashDataFile().fileName()).remove();
  QFile(crashCommandFile().fileName()).remove();
}

void iscore::DocumentBackupManager::saveModelData(const QByteArray& arr)
{
  m_modelFile.resize(0);
  m_modelFile.reset();
  m_modelFile.write(arr);
  m_modelFile.flush();
}

QTemporaryFile& iscore::DocumentBackupManager::crashDataFile()
{
  return m_modelFile;
}

iscore::CommandBackupFile& iscore::DocumentBackupManager::crashCommandFile()
{
  return *m_commandFile;
}

void iscore::DocumentBackupManager::updateBackupData()
{
  // Save the initial state of the document
  QSettings s{iscore::OpenDocumentsFile::path(), QSettings::IniFormat};

  auto existing_files = s.value("iscore/docs").toMap();
  existing_files.insert(
      crashDataFile().fileName(), crashCommandFile().fileName());
  s.setValue("iscore/docs", existing_files);
}
