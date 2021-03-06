#ifndef KCLINENUMBERPANEL_H
#define KCLINENUMBERPANEL_H

#include "kctextpanel.h"

class KCLineNumberPanel : public KCTextPanel
{
    Q_OBJECT
public:
    explicit KCLineNumberPanel(QWidget *parent = 0);
    void drawContent(int x, int y, int width, int height,
                     QTextBlock *block, KCTextBlockData *data, QTextCursor cursor);
    void setPanelWidth(int lineNumberPanelWidth);

signals:
    void requireSelectBlock(int blockNumber);

public slots:
    void setVisible(bool visible);

protected:
    void panelItemClickEvent(QTextBlock *block,
                             KCTextBlockData *data);

private:
    QColor textColor;
    int currentWidth=0;
};

#endif // KCLINENUMBERPANEL_H
