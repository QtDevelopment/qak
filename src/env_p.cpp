#include "env_p.h"

EnvPrivate::EnvPrivate(QObject *parent)
    : QObject(parent)
{

}

QString EnvPrivate::dataPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation)+QDir::separator()+subEnvPath();
}

QString EnvPrivate::cachePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation)+QDir::separator()+subEnvPath();
}

QString EnvPrivate::configPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+QDir::separator()+subEnvPath();
}

bool EnvPrivate::copy(const QString &src, const QString &dst)
{
    return copy(src, dst, true);
}

bool EnvPrivate::copy(const QString &src, const QString &dst, bool recursively)
{
    if(isFile(src)) {
        // File copy
        return QFile::copy(src , dst);
    }

    if(isDir(src)) {

        bool success = false;

        if(isFile(dst)) {
            qWarning() << "Qak" << "Env::copy" << dst << "is a file";
            return false;
        }

        // Directory copy
        QDir sourceDir(src);

        if(!isDir(dst))
            ensure(dst);

        QStringList entries = sourceDir.entryList(QDir::Files);

        // Copy files first
        for(int i = 0; i< entries.count(); i++) {
            success = QFile::copy(src + QDir::separator() + entries[i],
                                  dst + QDir::separator() + entries[i]);
            if(!success)
                return false;
        }

        if(recursively) {
            // Copy directories
            entries.clear();
            entries = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
            for(int i = 0; i< entries.count(); i++)
            {
                success = copy(src + QDir::separator() + entries[i],
                               dst + QDir::separator() + entries[i],
                               recursively);
                if(!success)
                    return false;
            }
        }

        return true;
    }

    return false;
}

bool EnvPrivate::remove(const QString &path)
{
    if(isFile(path)) {
        QFile file(path);
        return file.remove();
    }

    if(isDir(path)) {
        QDir dir(path);
        return dir.removeRecursively();
    }

    return false;
}

QString EnvPrivate::read(const QString &path)
{
    if(!isFile(path)){
        qWarning() << "Qak" << "Env::read" << "could not read file" << path << "Aborting";
        return QString();
    }

    QString source(path);
    source = source.replace("qrc://",":");
    source = source.replace("file://","");

    QFile file(source);
    QString fileContent;
    if ( file.open(QFile::ReadOnly) ) {
        QString line;
        QTextStream t( &file );
        do {
            line = t.readLine();
            fileContent += line;
         } while (!line.isNull());

        file.close();
    } else {
        qWarning() << "Qak" << "Env::read" << "unable to open" << path << "Aborting";
        return QString();
    }
    return fileContent;
}

bool EnvPrivate::write(const QString &data, const QString &path)
{
    return write(data, path, false);
}

bool EnvPrivate::write(const QString &data, const QString &path, bool overwrite)
{
    if(!overwrite && exists(path)) {
        qWarning() << "Qak" << "Env::write" << path << "already exist";
        return false;
    }

    if(overwrite && exists(path)) {
        if(isFile(path)) {
            if(!remove(path)) {
                qWarning() << "Qak" << "Env::write" << "could not remove" << path << "for writing. Aborting";
                return false;
            }
        }

        if(isDir(path)) {
            qWarning() << "Qak" << "Env::write" << path << "is a directory. Not overwriting";
            return false;
        }
    }

    QFile file(path);
    if(!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "Qak" << "Env::write" << path << "could not be opened for writing. Aborting";
        return false;
    }

    QTextStream out(&file);
    out << data;
    file.close();
    return true;
}

QStringList EnvPrivate::list(const QString &dir)
{
    return list(dir, false);
}

QStringList EnvPrivate::list(const QString &dir, bool recursively)
{
    QStringList entryList;

    if(isDir(dir)) {

        QDir sourceDir(dir);

        QStringList entries = sourceDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

        // Files
        for(int i = 0; i< entries.count(); i++) {
            entryList.append(sourceDir.absolutePath() + QDir::separator() + entries[i]);
        }

        if(recursively) {
            // Decent directories
            entries.clear();
            entries = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
            for(int i = 0; i< entries.count(); i++)
            {
                entryList += list(sourceDir.absolutePath() + QDir::separator() + entries[i], recursively);
            }
        }

        return entryList;
    }

    qWarning() << "Qak" << "Env::list" << dir << "is not a directory";
    return entryList;
}

bool EnvPrivate::ensure(const QString &path)
{
    QDir dir(path);
    if (!dir.exists())
        return dir.mkpath(".");
    return false;
}

bool EnvPrivate::exists(const QString &path)
{
    QFileInfo check(path);
    return check.exists();
}

bool EnvPrivate::isFile(const QString &path)
{
    QFileInfo check(path);
    return check.exists() && check.isFile();
}

bool EnvPrivate::isDir(const QString &path)
{
    QFileInfo check(path);
    return check.exists() && check.isDir();
}

void EnvPrivate::click(const QPointF point)
{
    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point,
            Qt::LeftButton,
            Qt::LeftButton,
            Qt::NoModifier );

    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point,
            Qt::LeftButton,
            Qt::LeftButton,
            Qt::NoModifier );

    qDebug() << "Env::click" << point;
    qApp->postEvent(qApp->focusWindow(), pressEvent);
    qApp->postEvent(qApp->focusWindow(), releaseEvent);
}

QString EnvPrivate::subEnvPath()
{
    QString sub;
    if(QGuiApplication::organizationName() != "")
        sub += QGuiApplication::organizationName() + QDir::separator();
    if(QGuiApplication::organizationDomain() != "")
        sub += QGuiApplication::organizationDomain() + QDir::separator();
    if(QGuiApplication::applicationName() != "")
        sub += QGuiApplication::applicationName() + QDir::separator();
    #ifdef QAK_STORE_VERSION_PATH
    if(QGuiApplication::applicationVersion() != "")
        sub += QGuiApplication::applicationVersion() + QDir::separator() ;
    #endif
    if(sub == "") {
         qWarning() << "Qak" << "Env" << "Couldn't resolve" << sub << "as a valid path. Using generic directory: \"Qak\"";
         sub = "Qak";
    }

    while(sub.endsWith( QDir::separator() )) sub.chop(1);

    return sub;
}
