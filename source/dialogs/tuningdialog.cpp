#include "tuningdialog.h"

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>

#include <powertabdocument/tuning.h>
#include <powertabdocument/generalmidi.h>

#include <algorithm>
#include <functional>

TuningDialog::TuningDialog(const Tuning& tuning, QWidget *parent) :
    QDialog(parent),
    tuning(tuning)
{
    setWindowTitle(tr("Tuning"));
    setModal(true);

    QFormLayout* formLayout = new QFormLayout;
    
    tuningNameEditor = new QLineEdit;
    tuningNameEditor->setText(QString().fromStdString(tuning.GetName()));
    formLayout->addRow(tr("Name:"), tuningNameEditor);
    
    usesSharpsSelector = new QCheckBox;
    usesSharpsSelector->setChecked(tuning.UsesSharps());
    connect(usesSharpsSelector, SIGNAL(toggled(bool)), this, SLOT(toggleSharps(bool)));
    formLayout->addRow(tr("Sharps:"), usesSharpsSelector);
    
    notationOffsetSelector = new QSpinBox;
    notationOffsetSelector->setMinimum(Tuning::MIN_MUSIC_NOTATION_OFFSET);
    notationOffsetSelector->setMaximum(Tuning::MAX_MUSIC_NOTATION_OFFSET);
    notationOffsetSelector->setValue(tuning.GetMusicNotationOffset());
    formLayout->addRow(tr("Music Notation Offset:"), notationOffsetSelector);
    
    numStringsSelector = new QSpinBox;
    numStringsSelector->setMinimum(Tuning::MIN_STRING_COUNT);
    numStringsSelector->setMaximum(Tuning::MAX_STRING_COUNT);
    numStringsSelector->setValue(tuning.GetStringCount());

    formLayout->addRow(tr("Number of Strings:"), numStringsSelector);
    connect(numStringsSelector, SIGNAL(valueChanged(int)), this, SLOT(updateEnabledStrings(int)));

    generateNoteNames(tuning.UsesSharps());
    initStringSelectors();
    updateEnabledStrings(tuning.GetStringCount());

    QHBoxLayout* tuningSelectorsLayout = new QHBoxLayout;
    
    foreach (QComboBox* selector, stringSelectors)
    {
        tuningSelectorsLayout->addWidget(selector);
    }
    
    formLayout->addRow(tr("Tuning:"), tuningSelectorsLayout);
    
    formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addLayout(formLayout);
    buttonsLayout->addWidget(buttonBox);

    buttonsLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(buttonsLayout);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void TuningDialog::accept()
{
    done(QDialog::Accepted);
}

void TuningDialog::reject()
{
    done(QDialog::Rejected);
}

void TuningDialog::initStringSelectors()
{
    for (uint8_t i = 0; i < Tuning::MAX_STRING_COUNT; i++)
    {
        QComboBox* selector = new QComboBox;
        selector->addItems(noteNames);
        
        if (tuning.IsValidString(i))
        {
            selector->setCurrentIndex(tuning.GetNote(i, false));
        }

        stringSelectors.push_back(selector);
    }
}

void TuningDialog::generateNoteNames(bool usesSharps)
{
    noteNames.clear();
    
    for (uint8_t note = midi::MIN_MIDI_NOTE; note < midi::MAX_MIDI_NOTE; note++)
    {
        noteNames << QString().fromStdString(midi::GetMidiNoteText(note, usesSharps)) +
                     QString().setNum(midi::GetMidiNoteOctave(note));
    }
}

void TuningDialog::toggleSharps(bool usesSharps)
{
    generateNoteNames(usesSharps);
    
    for (uint8_t i = 0; i < Tuning::MAX_STRING_COUNT; i++)
    {
        QComboBox* selector = stringSelectors.at(i);
        const int selectedIndex = selector->currentIndex();
        selector->clear();
        selector->addItems(noteNames);
        selector->setCurrentIndex(selectedIndex);
    }
}

void TuningDialog::updateEnabledStrings(int numStrings)
{
    using namespace std::placeholders;
    
    Q_ASSERT(numStrings <= (int)Tuning::MAX_STRING_COUNT && numStrings >= 0);
    
    std::for_each(stringSelectors.begin(), stringSelectors.begin() + numStrings,
                  std::bind(&QComboBox::setEnabled, _1, true));
    
    std::for_each(stringSelectors.begin() + numStrings, stringSelectors.end(),
                  std::bind(&QComboBox::setEnabled, _1, false));
}