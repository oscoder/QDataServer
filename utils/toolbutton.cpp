#include "toolbutton.h"

#include <QtGui/QStylePainter>
#include <QtGui/QStyleOptionToolButton>

using namespace Utils;

/*!
 * \class Utils::ToolButton
 * \brief Tool button with automatical text wrapping
 */

/*!
  Constructor
  \param parent the parent widget
  */
ToolButton::ToolButton(QWidget *parent) :
    QToolButton(parent),
    minTextWidth(ToolButton::DefaultMinTextWidth),
    maxTextWidth(ToolButton::DefaultMaxTextWidth),
    txtLayoutPolicy(ToolButton::Eager),
    txtWrapPolicy(ToolButton::WrapAndElide),
    txtMargins(QMargins(0, 0, 0, 0)),
    updateNeeded(false)
{
}

/*!
  Sets the min and max bounds of text
  \param minTextWidth min
  \param maxTextWidth max
  */
void ToolButton::setTextBoundaries(int minTextWidth, int maxTextWidth)
{
    if((minTextWidth != this->minTextWidth ||
        maxTextWidth != this->maxTextWidth) && !updateNeeded)
        updateNeeded = true;

    this->minTextWidth = minTextWidth;
    this->maxTextWidth = maxTextWidth;
}

/*!
  sets the layout policy
  \param policy the layout policy set for text
  */
void ToolButton::setTextLayoutPolicy(TextLayoutPolicy policy)
{
    if(policy != txtLayoutPolicy && !updateNeeded)
        updateNeeded = true;

    txtLayoutPolicy = policy;
}

/*!
  Wrap policy
  \param policy the wrap policy
  */
void ToolButton::setTextWrapPolicy(TextWrapPolicy policy)
{
    if(policy != txtWrapPolicy && !updateNeeded)
        updateNeeded = true;

    txtWrapPolicy = policy;
}

/*!
  sets the Text margins
  \param margins the margin to be set
  */
void ToolButton::setTextMargins(const QMargins &margins)
{
    txtMargins = margins;
}

/*!
  \return Minimum text width
  */
int ToolButton::minimumTextWidth() const
{
    return minTextWidth;
}

/*!
  \return Max text width
  */
int ToolButton::maximumTextWidth() const
{
    return maxTextWidth;
}

/*!
  \return Texts layout policy
  */
ToolButton::TextLayoutPolicy ToolButton::textLayoutPolicy() const
{
    return txtLayoutPolicy;
}

/*!
  \return Wrap policy of text
  */
ToolButton::TextWrapPolicy ToolButton::textWrapPolicy() const
{
    return txtWrapPolicy;
}

/*!
  \return Texts margins
  */
QMargins ToolButton::textMargins() const
{
    return txtMargins;
}

/*!
  \return the min size hint
  */
QSize ToolButton::minimumSizeHint() const
{
    return sizeHint();
}

/*!
  \return size hint
  */
QSize ToolButton::sizeHint() const
{
    const QSize originalSize = QToolButton::sizeHint();

    int w = 0, h = 0;
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    QFontMetrics fm = fontMetrics();

    if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
        QStringList textDataList = wrapedText.split('\n');
        const QString line = textDataList.size() == 1 ?
            textDataList.at(0) : (fm.width(textDataList.at(0)) >
                fm.width(textDataList.at(1)) ? textDataList.at(0) :
                    textDataList.at(1));
        QSize textSize = fm.size(Qt::TextShowMnemonic, line);
        const int topBottomMargin = txtMargins.top() + txtMargins.bottom();
        const int leftRightMargin = txtMargins.left() + txtMargins.right();
        const QSize originalTextSize =
                fm.size(Qt::TextShowMnemonic, wrapedText);
        textSize.setWidth(textSize.width() + leftRightMargin
                          + fm.width(QLatin1Char(' '))*2);
        if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
            h += topBottomMargin + 4 + originalTextSize.height() > textSize
                 .height() ? originalTextSize.height() : textSize.height();
            if ((textSize.width() + leftRightMargin) > w)
                w = textSize.width() + leftRightMargin;
            if(w <= LargeVariationWidth + (leftRightMargin == 0
                    ? 2 * DefaultLargeVariationPadding : leftRightMargin))
                w = text() == wrapedText ? originalSize.width()
                    : LargeVariationWidth + (2 * DefaultLargeVariationPadding);
        } else if (opt.toolButtonStyle == Qt::ToolButtonTextBesideIcon) {
            w = originalSize.width();
            h = originalSize.height() + topBottomMargin;
        } else if (opt.toolButtonStyle == Qt::ToolButtonTextOnly
                || icon().isNull()) {
            w = originalSize.width();
            h = originalSize.height();
        }
    } else {
        w = originalSize.width();
        h = originalSize.height();
    }

    opt.rect.setSize(QSize(w, h));
    if (popupMode() == MenuButtonPopup || popupMode() == InstantPopup)
        w += style()->pixelMetric(QStyle::PM_MenuButtonIndicator);

    return QSize(w, h);
}

/*!
  \reimp
  */
void ToolButton::paintEvent(QPaintEvent *)
{
    //if not initialized or if text changed
    if (updateNeeded || wrapedText.isNull() ||
        wrapedText.isEmpty() || text() != oldNotWrapedText) {
        wrapedText = txtWrapPolicy == ToolButton::WrapAndElide ?
                     wrapAndElideText(text()) : smartSplit(text());
        oldNotWrapedText = text();
        updateNeeded = false;
        updateGeometry();
    }

    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    opt.text = wrapedText;
    p.drawComplexControl(QStyle::CC_ToolButton, opt);
}

/*!
  \reimp
  */
void ToolButton::resizeEvent(QResizeEvent *)
{
    updateNeeded = true;
}

/*!
  Wraps and elides the given text
  \param textData input text
  \return wrapped and elited form of text
  */
QString ToolButton::wrapAndElideText(const QString &textData)
{
    const int iconWidth = iconSize().width();
    const int textWidth = txtLayoutPolicy == ToolButton::Compact
            ? (iconWidth > minTextWidth ? iconWidth : minTextWidth)
            : (iconWidth > maxTextWidth ? iconWidth : maxTextWidth);

    const QStringList textDataList = textData.split(' ');

#ifdef Q_WS_WIN
    const QFontMetrics fontMetrics(font());
#else
    QFont currFont = font();
    currFont.setPixelSize(11);
    setFont(currFont);
    const QFontMetrics fontMetrics(currFont);
#endif

    QString newTextData = "";
    QString firstLine = "";
    QString secondLine = "";
    int lines = 1;
    int index = 1;
    bool firstWordOnSecondLine = true;

    foreach (const QString &textDataPart, textDataList) {
        bool needForElide = false;
        bool needForBreak = false;
        if (lines == 1 && fontMetrics.width(QString("%1 %2")
                .arg(firstLine).arg(textDataPart).trimmed()) > textWidth) {
            if (index == 1)
                needForElide = true;
            else
                ++lines;
        } else if (lines == 2 && fontMetrics.width(QString("%1 %2")
                .arg(secondLine).arg(textDataPart).trimmed()) > textWidth) {
            ++lines;
        }

        if (lines == 1) {
            const QString space = index == 1 ? "" : " ";
            firstLine.append(QString("%1%2").arg(space).arg(textDataPart));

            if(needForElide || fontMetrics.width(firstLine) > textWidth) {
                firstLine = fontMetrics.elidedText(firstLine,
                                        Qt::ElideRight,
                                        textWidth);
                needForBreak = true;
            }
        }

        if (lines >= 2) {
            const QString space = firstWordOnSecondLine == true ? "" : " ";
            secondLine.append(QString("%1%2").arg(space).arg(textDataPart));
            firstWordOnSecondLine = false;
        }

        if (index == textDataList.size() || needForBreak || lines == 3) {
            if (lines >= 2 && !needForElide) {
                secondLine = fontMetrics.elidedText(secondLine,
                                        Qt::ElideRight,
                                        textWidth);
            }
            newTextData = secondLine.isEmpty()
                    ? firstLine : QString("%1\n%2")
                        .arg(firstLine)
                        .arg(secondLine);

            newTextData = newTextData.trimmed();
            break;
        }

        ++index;
    }

    return newTextData;
}

/*!
  Performs smart split to text
  \param textData input
  \return output
  */
QString ToolButton::smartSplit(const QString &textData)
{
    const int textWidth = iconSize().width() > minTextWidth ?
                          iconSize().width() : minTextWidth;
#ifdef Q_WS_WIN
    const QFontMetrics fontMetrics(font());
#else
    QFont currFont = font();
    currFont.setPixelSize(11);
    setFont(currFont);
    const QFontMetrics fontMetrics(currFont);
#endif

    QString newTextData = fontMetrics.width(textData) <= textWidth ||
        toolButtonStyle() != Qt::ToolButtonTextUnderIcon ? textData : "";

    if (newTextData.isEmpty()) {
        const int halfTotalTextWidth = fontMetrics.width(textData) / 2;
        const QStringList textDataList = textData.split(' ');
        QString firstLine;
        QString secondLine;
        QStringListIterator i(textDataList);
        int index = 1;
        while (i.hasNext()) {
            const QString textDataPart = i.next();
            const int tempStringWidth = fontMetrics.width(QString("%1 %2")
                            .arg(firstLine).arg(textDataPart).trimmed());
            /*ident to second line if the width of previously composed
              string + width of the new part is bigger than half of width of
              the original not indented string AND if the width of newly
              composed string - the width of a half of the original string
              is bigger than half width of the new part
            */
            if (tempStringWidth > halfTotalTextWidth
                    && (tempStringWidth - halfTotalTextWidth)
                        > (fontMetrics.width(textDataPart) / 2)) {
                if (index == 1) {
                    firstLine.append(textDataPart);
                } else {
                    secondLine.append(QString("%1 ").arg(textDataPart));
                }

                while (i.hasNext()) {
                    secondLine.append(QString("%1 ").arg(i.next()));
                }

                secondLine = secondLine.trimmed();
                break;
            } else {
                firstLine.append(QString("%1 ").arg(textDataPart));
            }

            ++index;
        }

        newTextData = QString("%1\n%2").arg(firstLine.trimmed())
                      .arg(secondLine.trimmed()).trimmed();
    }

    return newTextData;
}
