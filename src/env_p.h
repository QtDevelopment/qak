#ifndef QAK_PATH_PRIVATE_H
#define QAK_PATH_PRIVATE_H

#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QVariant>
#include <QMoveEvent>
#include <QWindow>
#include <QApplication>

#include <QtGui/QGuiApplication>

class EnvPrivate : public QObject
{
    Q_OBJECT

public:
    EnvPrivate(QObject* parent = 0);

    Q_INVOKABLE static QString dataPath();
    Q_INVOKABLE static QString cachePath();
    Q_INVOKABLE static QString configPath();

    Q_INVOKABLE static bool copy(const QString &src, const QString &dst);
    Q_INVOKABLE static bool copy(const QString &src, const QString &dst, bool recursively);

    Q_INVOKABLE static bool remove(const QString &path);

    Q_INVOKABLE QString read(const QString &path);

    Q_INVOKABLE bool write(const QString& data, const QString &path);
    Q_INVOKABLE bool write(const QString& data, const QString &path, bool overwrite);

    Q_INVOKABLE static QStringList list(const QString &dir);
    Q_INVOKABLE static QStringList list(const QString &dir, bool recursively);

    Q_INVOKABLE static bool ensure(const QString &path);

    Q_INVOKABLE static bool exists(const QString &path);
    Q_INVOKABLE static bool isFile(const QString &path);
    Q_INVOKABLE static bool isDir(const QString &path);

    // TODO move to seperate type / remove ?
    Q_INVOKABLE static void click(const QPointF point);

private:
    static QString subEnvPath();
};

#endif // QAK_PATH_PRIVATE_H
