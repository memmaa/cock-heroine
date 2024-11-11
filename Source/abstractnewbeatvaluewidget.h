#ifndef ABSTRACTNEWBEATVALUEWIDGET_H
#define ABSTRACTNEWBEATVALUEWIDGET_H

#include <QFrame>

class QVBoxLayout;
class QHBoxLayout;
class QRadioButton;

class AbstractNewBeatValueWidget : public QFrame
{
    Q_OBJECT
public:
    AbstractNewBeatValueWidget(float valueInBeats, QWidget * parent = nullptr);
    virtual void updateForValue(float);
    virtual QString getName();
    virtual float getMatchResistance() = 0;
    void select();
    void deselect();
    virtual int getNumerator() = 0;
    virtual int getDivisor() = 0;
    virtual float getCurrentValue();
    //!
    //! \brief poopness returns a higher number if it's a poor match.
    //! \return 0 = perfect match, 1 = exceptionallly poor match, anything higher is not a match at all
    //!
    float poopness();
    //!
    //! \brief matchness the opposite of poopness. If this is not a match, matchness will be 0
    //! \return between 0 (not a match) and 100 (perfect match)
    //!
    float matchness();
    //!
    //! \brief isAMatch is this value a possible (but not necessarily the best) match for the interval?
    //!                 It only has to be within the maximum allowed deviation to count
    //! \return true if it could be a match
    //!
    bool isAMatch();
    bool isSelected();
signals:
    void selected();
    void selected(AbstractNewBeatValueWidget *);
protected:
    float valueInBeats;
    QVBoxLayout * valueLayout;
    QHBoxLayout * valueFractionLayout;
    QHBoxLayout * valueNameLayout;
    QRadioButton * valueNameRadioButton;
    virtual void updateBackground();

private slots:
    void on_selected(bool on_selected);
};

#endif // ABSTRACTNEWBEATVALUEWIDGET_H
