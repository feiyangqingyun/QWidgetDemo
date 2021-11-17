#include "knobtab.h"
#include "knobbox.h"
#include <qlayout.h>

KnobTab::KnobTab( QWidget *parent ):
    QWidget( parent )
{
    QGridLayout *layout = new QGridLayout( this );

    const int numRows = 3;
    for ( int i = 0; i < 2 * numRows; i++ )
    {
        KnobBox *knobBox = new KnobBox( this, i );
        layout->addWidget( knobBox, i / numRows, i % numRows );
    }
}

