#ifndef PROJECTFILES_H
#define PROJECTFILES_H

#include <QString>
#include <QStringList>

class ProjectFiles
{
public:
    ProjectFiles(const QString &rootDir);
    ~ProjectFiles();
    QStringList directories();
    QStringList projects();
    bool isDirEmpty(const QString &dirName);
    bool canProjectBeSavedToDir(const QString &name); // relative
    bool isProjectDir();
    bool isProjectDir(const QString &path);
    bool hasTemporaryData();
    void cd(const QString &newRoot);
    bool pathExists(const QString &path);
    bool canCdTo(const QString &path);
    bool canCdToUpper(const QString &path);
    QString upDir(const QString &path);
    QString currentDir();
    QString projectName(const QString &path);
private:
    QStringList dirs;
    QStringList projs;
    QString root;
    bool isPD;
private:
    bool isProjectDirInternal(const QString &path);
};

#endif // PROJECTFILES_H
