#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    Highlighter(QTextDocument *parent = 0);

    static void init();
    static void update();

protected:
    void highlightBlock(const QString &text) override;

private:
    static std::pair<QTextCharFormat, QVector<QRegularExpression>> rules[];
    static QRegularExpression num_regex;
};

#endif // HIGHLIGHTER_H
