#include "dialtab.h"
#include "dialbox.h"
#include <qlayout.h>

DialTab::DialTab( QWidget *parent ):
    QWidget( parent )
{
    QGridLayout *layout = new QGridLayout( this );

    const int numRows = 3;
    for ( int i = 0; i < 2 * numRows; i++ )
    {
        DialBox *dialBox = new DialBox( this, i );
        layout->addWidget( dialBox, i / numRows, i % numRows );
    }
}

