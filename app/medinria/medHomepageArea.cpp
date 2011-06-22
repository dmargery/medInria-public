/*
    Copyright (c) 2011, Alexandre Abadie <Alexandre.Abadie@irisa.fr>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY Alexandre <email> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Alexandre <email> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "medHomepageArea.h"

#include <medViewerArea.h>
#include <medHomepageButton.h>
#include <medGui/medViewerConfiguration.h>
#include <medGui/medViewerConfigurationFactory.h>

class medHomepageAreaPrivate
{
public:
    QGraphicsScene * configurationsButtonsScene;
    QGraphicsProxyWidget * configurationsButtonsProxyWidget;
    QWidget * configurationsButtonsWidget;

    QGraphicsScene * userButtonsScene;
    QGraphicsProxyWidget * userButtonsProxyWidget;
    QWidget * userButtonsWidget;

    QGraphicsScene * infoScene;
    QGraphicsProxyWidget * infoProxyWidget;
    QWidget * infoWidget;
    
    QPropertyAnimation * animation;

    medViewerArea * viewerArea;
};

medHomepageArea::medHomepageArea ( QWidget * parent ) : QWidget ( parent ), d ( new medHomepageAreaPrivate )
{
    d->viewerArea = NULL;
    setupUi(this);

//     d->configurationsButtonsView = new QGraphicsView ( this );
    d->configurationsButtonsScene = new QGraphicsScene ( configurationsGraphicsView );
    configurationsGraphicsView->setScene ( d->configurationsButtonsScene );
    configurationsGraphicsView->setStyleSheet ( "background: #313131;border: 0px;padding: 0px 0px 0px 0px;" );
    configurationsGraphicsView->setFocusPolicy ( Qt::NoFocus );

    //Configurations buttons (browser, visualization, registration, diffusion, etc)
    d->configurationsButtonsWidget = new QWidget ( configurationsGraphicsView );
    d->configurationsButtonsWidget->setMinimumWidth ( 200 );
    
    d->userButtonsScene = new QGraphicsScene ( infoGraphicsView );
    userGraphicsView->setScene ( d->userButtonsScene );
    userGraphicsView->setStyleSheet ( "background: #313131;border: 0px;padding: 0px 0px 0px 0px;" );
    userGraphicsView->setFocusPolicy ( Qt::NoFocus );

    //Configurations buttons (Help, about, settings)
    d->userButtonsWidget = new QWidget ( userGraphicsView );
    d->userButtonsWidget->setMinimumWidth ( 200 );

    d->infoScene = new QGraphicsScene ( userGraphicsView );
    infoGraphicsView->setScene ( d->infoScene );
    infoGraphicsView->setStyleSheet ( "background: #313131;border: 0px;padding: 0px 0px 0px 0px;" );
    infoGraphicsView->setFocusPolicy ( Qt::NoFocus );



    QHBoxLayout * userButtonsLayout = new QHBoxLayout(userGraphicsView);
//     QPushButton * helpButton = new QPushButton(userGraphicsView);
    medHomepageButton * helpButton = new medHomepageButton(userGraphicsView);
    helpButton->setText ( "Help" );
    helpButton->setMinimumHeight ( 30 );
    helpButton->setMaximumWidth ( 150 );
    helpButton->setMinimumWidth ( 200 );
    helpButton->setFocusPolicy ( Qt::NoFocus );

//     QPushButton * aboutButton = new QPushButton(userGraphicsView);
    medHomepageButton * aboutButton = new medHomepageButton(userGraphicsView);
    aboutButton->setText("About");
    aboutButton->setMinimumHeight ( 30 );
    aboutButton->setMaximumWidth ( 150 );
    aboutButton->setMinimumWidth ( 200 );
    aboutButton->setFocusPolicy ( Qt::NoFocus );

//     QPushButton * settingsButton = new QPushButton(userGraphicsView);
    medHomepageButton * settingsButton = new medHomepageButton(userGraphicsView);
    settingsButton->setText("Settings");
    settingsButton->setMinimumHeight ( 30 );
    settingsButton->setMaximumWidth ( 150 );
    settingsButton->setMinimumWidth ( 200 );
    settingsButton->setFocusPolicy ( Qt::NoFocus );
    
    userButtonsLayout->insertWidget (0, settingsButton);
    userButtonsLayout->insertWidget (1, aboutButton);
    userButtonsLayout->insertWidget (2, helpButton);

    d->userButtonsWidget->setLayout(userButtonsLayout);
    
    //     //Special widget : image, text, etc. QtWebkit ?
    d->infoWidget = new QWidget(infoGraphicsView);
    QVBoxLayout * infoLayout = new QVBoxLayout(this);
    QLabel * medinriaLabel = new QLabel(this);
    medinriaLabel->setPixmap(QPixmap(":pixmaps/medinria-logo.png").scaled(300,100));
    QLabel * textLabel = new QLabel;
    textLabel->setText("The ultimate medical imaging platform");
    infoLayout->insertWidget(0,medinriaLabel);
    infoLayout->insertWidget(1, textLabel);
    
    d->infoWidget->setLayout(infoLayout);
}

medHomepageArea::~medHomepageArea()
{
    delete d;

    d = NULL;
}

void medHomepageArea::initPage ( void )
{   
    QList<QString> configList = medViewerConfigurationFactory::instance()->configurations();
    QGridLayout * configurationButtonsLayout = new QGridLayout ( this );
// //     QPushButton * buttonBrowser = new QPushButton ( this );
    medHomepageButton * browserButton = new medHomepageButton ( this );
    browserButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    browserButton->setIcon(QIcon(":/icons/folder.png"));
    browserButton->setText ( "Browser" );
    browserButton->setMinimumHeight ( 30 );
    browserButton->setMaximumWidth ( 200 );
    browserButton->setMinimumWidth ( 200 );
    browserButton->setFocusPolicy ( Qt::NoFocus );
    configurationButtonsLayout->addWidget ( browserButton,0,0 );
    QObject::connect ( browserButton, SIGNAL ( clicked() ),this, SLOT ( onShowBrowser() ) );

    for ( int i = 0; i< configList.size(); i++ )
    {
//         QPushButton * button = new QPushButton ( this );
        medHomepageButton * button = new medHomepageButton ( this );
        button->setText ( configList.at ( i ) );
        button->setFocusPolicy ( Qt::NoFocus );
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setMinimumHeight ( 30 );
        button->setMaximumWidth ( 200 );
        button->setMinimumWidth ( 200 );;
        configurationButtonsLayout->addWidget ( button,i + 1,0 );
        QObject::connect ( button, SIGNAL ( clicked ( QString ) ),this, SLOT ( onShowConfiguration ( QString ) ) );
    }
    d->configurationsButtonsWidget->setLayout ( configurationButtonsLayout );
    d->configurationsButtonsProxyWidget = d->configurationsButtonsScene->addWidget ( d->configurationsButtonsWidget );

    d->animation = new QPropertyAnimation ( d->configurationsButtonsWidget, "pos" );
    d->animation->setDuration ( 750 );
    d->animation->setEasingCurve ( QEasingCurve::OutCubic );
    d->animation->setStartValue ( QPoint ( ( d->configurationsButtonsScene->width() / 2 ) - 250, 250 ) );
    d->animation->setEndValue ( QPoint ( ( d->configurationsButtonsScene->width() / 2 ) + 100 ,  250 ) );
}

QPropertyAnimation* medHomepageArea::getAnimation ( void )
{
    return d->animation;
}

void medHomepageArea::setViewerArea ( medViewerArea* viewer )
{
    d->viewerArea = viewer;
}


void medHomepageArea::onShowBrowser ( void )
{
    emit showBrowser();
}

void medHomepageArea::onShowConfiguration ( QString configuration )
{
    emit showViewer();
    emit showConfiguration(configuration);
}



