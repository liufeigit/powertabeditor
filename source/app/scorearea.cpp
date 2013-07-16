/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#include "scorearea.h"

#include <app/documentmanager.h>
#include <app/pubsub/scorelocationpubsub.h>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/timer.hpp>
#include <painters/caretpainter.h>
#include <painters/systemrenderer.h>
#include <QDebug>
#include <QGraphicsItem>
#include <QProgressDialog>
#include <score/score.h>

static const double SYSTEM_SPACING = 50;

#if 1
ScoreArea::ScoreArea(QWidget *parent)
    : QGraphicsView(parent),
      myViewType(Staff::GuitarView),
      myKeySignatureClicked(boost::make_shared<ScoreLocationPubSub>()),
      myTimeSignatureClicked(boost::make_shared<ScoreLocationPubSub>()),
      myBarlineClicked(boost::make_shared<ScoreLocationPubSub>())
{
    setScene(&myScene);
}

void ScoreArea::renderDocument(const Document &document, Staff::ViewType view)
{
    myScene.clear();
    myRenderedSystems.clear();
    myDocument = document;
    myViewType = view;

    const Score &score = document.getScore();

    boost::timer timer;
    QProgressDialog progressDialog(tr("Rendering ..."), "", 0,
                                   score.getSystems().size());
    progressDialog.setCancelButton(0);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.show();

    CaretPainter *caretPainter = new CaretPainter(document.getCaret());

    // Render each system.
    int i = 0;
    double height = 0;
    BOOST_FOREACH(const System &system, score.getSystems())
    {
        progressDialog.setValue(i);
        SystemRenderer render(this, score);

        QGraphicsItem *renderedSystem = render(system, i, myViewType);
        renderedSystem->setPos(0, height);
        myRenderedSystems << renderedSystem;
        myScene.addItem(renderedSystem);
        height += renderedSystem->boundingRect().height() + SYSTEM_SPACING;
#if 0
        renderer.connectSignals();
#endif
        ++i;

        caretPainter->addSystemRect(renderedSystem->sceneBoundingRect());
    }

    myScene.addItem(caretPainter);

    progressDialog.setValue(i);

    qDebug() << "Score rendered in" << timer.elapsed() << "seconds";
    qDebug() << "Rendered " << myScene.items().size() << "items";
}

void ScoreArea::redrawSystem(int index)
{
    // Delete and remove the system from the scene.
    delete myRenderedSystems.takeAt(index);

    const Score &score = myDocument->getScore();
    SystemRenderer render(this, score);
    QGraphicsItem *newSystem = render(score.getSystems()[index], index,
                                        myViewType);

    double height = 0;
    if (index > 0)
    {
        height = myRenderedSystems.at(index - 1)->boundingRect().height() +
                SYSTEM_SPACING;
    }

    newSystem->setPos(0, height);
    height += newSystem->boundingRect().height() + SYSTEM_SPACING;
#if 0
    renderer.connectSignals();
#endif

    myScene.addItem(newSystem);
    myRenderedSystems.insert(index, newSystem);

    // Shift the following systems.
    for (int i = index + 1; i < myRenderedSystems.size(); ++i)
    {
        QGraphicsItem *system = myRenderedSystems[i];
        system->setPos(0, height);
        height += system->boundingRect().height() + SYSTEM_SPACING;
    }

#if 0
    caret->updatePosition();
#endif
}

boost::shared_ptr<ScoreLocationPubSub> ScoreArea::getKeySignaturePubSub() const
{
    return myKeySignatureClicked;
}

boost::shared_ptr<ScoreLocationPubSub> ScoreArea::getTimeSignaturePubSub() const
{
    return myTimeSignatureClicked;
}

boost::shared_ptr<ScoreLocationPubSub> ScoreArea::getBarlinePubSub() const
{
    return myBarlineClicked;
}

boost::shared_ptr<ScoreLocationPubSub> ScoreArea::getSelectionPubSub() const
{
    return myDocument->getCaret().getSelectionPubSub();
}

#else
ScoreArea::ScoreArea(PowerTabEditor *editor) :
    editor(editor),
    caret(NULL),
    scoreIndex(0),
    keySignatureClicked(boost::make_shared<SystemLocationPubSub>()),
    timeSignatureClicked(boost::make_shared<SystemLocationPubSub>()),
    barlineClicked(boost::make_shared<SystemLocationPubSub>())
{
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    setScene(&scene);

    redrawOnNextRefresh = false;
}

ScoreArea::~ScoreArea()
{
    // Prevent double deletion.
    if (caret)
    {
        scene.removeItem(caret.get());
    }
}

void ScoreArea::renderDocument(boost::shared_ptr<PowerTabDocument> doc)
{
    bool newCaret = (caret == NULL);
    uint32_t voice = 0;

    if (caret)
    {
        voice = caret->getCurrentVoice();
        scene.removeItem(caret.get());
    }

    scene.clear();
    systemList.clear();
    document = doc;
    int lineSpacing = document->GetTablatureStaffLineSpacing();

    // Set up the caret
    if (newCaret)
    {
        caret.reset(new Caret(doc->GetTablatureStaffLineSpacing()));
        connect(caret.get(), SIGNAL(moved()), this, SLOT(adjustScroll()));
    }

    caret->setScore(doc->GetScore(scoreIndex));
    caret->setCurrentVoice(voice);

    // Adjust the caret to a valid position, since (for example) a system may
    // have been removed.
    caret->adjustToValidLocation();

    if (newCaret)
    {
        editor->registerCaret(caret.get());
    }

    scene.addItem(caret.get());

    // Render each score
    // Only worry about the guitar score so far
    renderScore(document->GetScore(scoreIndex), lineSpacing);
}

/// Used to request that a full redraw is performed when the ScoreArea is next updated
void ScoreArea::requestFullRedraw()
{
    Q_ASSERT(!redrawOnNextRefresh);
    redrawOnNextRefresh = true;
}

// ensures that the caret is visible when it changes sections
void ScoreArea::adjustScroll()
{
    ensureVisible(caret.get(), 0, 100);
}

#endif
