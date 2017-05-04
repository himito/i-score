#pragma once
#include <QByteArray>
#include <QObject>
#include <QPair>
#include <QStack>
#include <QString>
#include <QTemporaryFile>
#include <iscore/command/CommandData.hpp>
#include <iscore/command/Command.hpp>

namespace iscore
{
class CommandStack;
/**
 * @brief Serialized command stack data for backup / restore
 */
struct CommandStackBackup
{
  CommandStackBackup(const iscore::CommandStack& stack);

  QStack<CommandData> savedUndo;
  QStack<CommandData> savedRedo;
};

/**
 * @brief Abstraction over the backup of commands
 *
 * Synchronizes the commands of a document to an on-disk file,
 * by maintaining serialized stacks of commands at each new command.
 *
 * This way, if there is a crash, the document can be restored from the
 * last successful command and only the latest user action is lost.
 */
class CommandBackupFile final : public QObject
{
public:
  CommandBackupFile(const iscore::CommandStack& stack, QObject* parent);
  QString fileName() const;

private:
  void on_push();
  void on_undo();
  void on_redo();
  void on_indexChanged();

  //! Writes the current buffers to disk.
  void commit();

  const iscore::CommandStack& m_stack;
  CommandStackBackup m_backup;

  QTemporaryFile m_file;
};
}
