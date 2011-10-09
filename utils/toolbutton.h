#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QToolButton>

#include "utils_global.h"

namespace Utils {

class UTILS_EXPORT ToolButton : public QToolButton
{
    Q_OBJECT

public:
    enum TextLayoutPolicy {
        Eager, Compact
    };
    enum TextWrapPolicy {
        WrapAndElide, SmartSplit
    };
    explicit ToolButton(QWidget *parent = 0);

    void setTextBoundaries(int minTextWidth, int maxTextWidth);
    void setTextLayoutPolicy(TextLayoutPolicy policy);
    void setTextWrapPolicy(TextWrapPolicy policy);
    void setTextMargins(const QMargins &margins);

    int minimumTextWidth() const;
    int maximumTextWidth() const;

    TextLayoutPolicy textLayoutPolicy() const;
    TextWrapPolicy textWrapPolicy() const;
    QMargins textMargins() const;

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

protected:
     virtual void paintEvent(QPaintEvent *);
     virtual void resizeEvent(QResizeEvent *);

private:
     QString wrapAndElideText(const QString &textData);
     QString smartSplit(const QString &textData);

private:
    enum {
        DefaultMinTextWidth = 16,
        DefaultMaxTextWidth = 5555,
        LargeVariationWidth = 32,
        DefaultLargeVariationPadding = 4
    };
    int minTextWidth;
    int maxTextWidth;
    TextLayoutPolicy txtLayoutPolicy;
    TextWrapPolicy txtWrapPolicy;

    QMargins txtMargins;

    QString wrapedText;
    QString oldNotWrapedText;

    bool updateNeeded;
};

} //namespace Utils
#endif // TOOLBUTTON_H
