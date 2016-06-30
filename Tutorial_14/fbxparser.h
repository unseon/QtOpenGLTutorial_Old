#ifndef FBXPARSER_H
#define FBXPARSER_H

#include <QObject>

class FbxParser : public QObject
{
    Q_OBJECT
public:
    explicit FbxParser(QObject *parent = 0);

signals:

public slots:
};

#endif // FBXPARSER_H