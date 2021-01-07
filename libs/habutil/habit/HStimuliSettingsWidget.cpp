/*
 * HStimuliSettingsWidget.cpp
 *
 *  Created on: Oct 21, 2014
 *      Author: Oakeslab
 */

#include "HStimuliSettingsWidget.h"
#include "HStimulusSettingsWidget.h"
#include "HStimulusSettingsOrderImportUtil.h"
#include "HGMM.h"
#include "HWorkspaceUtil.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>

#include <QtGlobal>
#if QT_VERSION < 0x050000
#include <QDesktopServices>
#else
#include <QStandardPaths>
#endif

using namespace Habit;
using namespace GUILib;

HStimuliSettingsWidget::HStimuliSettingsWidget(const QString& labelName, const StimuliSettings& stimuli, int context, const StimulusDisplayInfo& info, QWidget* parent)
: QWidget(parent)
, m_stimuli(stimuli)
, m_context(context)
, m_sdi(info)
{
	create(labelName, info);
	connections();
	m_phgmm->addStimuli(stimuli, context);
}

void HStimuliSettingsWidget::create(const QString& labelName, const StimulusDisplayInfo& info)
{
	m_pStimulusSettingsListWidget = new HStimulusSettingsListWidget(m_stimuli.stimuli(), m_sdi);
	m_pStimulusOrderListWidget = new HStimulusOrderListWidget(m_stimuli.orders(), m_stimuli.stimuli(), m_sdi);
	m_pStimulusPreviewWidget = new HStimulusPreviewWidget(info, this);


	m_phgmm = new HGMM(info, m_pStimulusPreviewWidget->getStimulusWidgets(), habutilGetStimulusRootDir());

	connect(m_pStimulusPreviewWidget, SIGNAL(preroll(unsigned int)), m_phgmm, SLOT(preroll(unsigned int)));
	connect(m_pStimulusPreviewWidget, SIGNAL(pause(unsigned int)), m_phgmm, SLOT(pause(unsigned int)));
	connect(m_pStimulusPreviewWidget, SIGNAL(stim(unsigned int)), m_phgmm, SLOT(stim(unsigned int)));
	connect(m_pStimulusPreviewWidget, SIGNAL(rewind(unsigned int)), m_phgmm, SLOT(rewind(unsigned int)));
	connect(m_pStimulusPreviewWidget, SIGNAL(background()), m_phgmm, SLOT(background()));

	QGroupBox *g1 = new QGroupBox(QString("%1 Stimuli").arg(labelName));
	QVBoxLayout *v1 = new QVBoxLayout;
	v1->addWidget(m_pStimulusSettingsListWidget);
	g1->setLayout(v1);
	QGroupBox *g2 = new QGroupBox(QString("%1 Orders").arg(labelName));
	QVBoxLayout *v2 = new QVBoxLayout;
	v2->addWidget(m_pStimulusOrderListWidget);
	g2->setLayout(v2);

	QHBoxLayout *h12 = new QHBoxLayout;
	h12->addWidget(g1);
	h12->addWidget(g2);

	QGroupBox *g4 = new QGroupBox("Stimulus Preview");
	QVBoxLayout *v4 = new QVBoxLayout;
	v4->addWidget(m_pStimulusPreviewWidget);

	g4->setLayout(v4);

	QGroupBox *g3 = new QGroupBox("Import");
	QHBoxLayout *h3 = new QHBoxLayout;
	QLabel *l3 = new QLabel("Stimuli and/or orders for this phase can be imported from a text file.");
	m_pbImport = new QPushButton("Import");
	l3->setWordWrap(true);
	h3->addWidget(l3, 1);
	h3->addWidget(m_pbImport, 1);
	g3->setLayout(h3);

	QVBoxLayout *v = new QVBoxLayout;
	v->addLayout(h12);
	v->addWidget(g4, 1);
//	v->addStretch(1);
	v->addWidget(g3);
	setLayout(v);
}

void HStimuliSettingsWidget::connections()
{
	connect(m_pbImport, SIGNAL(clicked()), this, SLOT(importClicked()));
	connect(m_pStimulusSettingsListWidget, SIGNAL(previewStimulus(int)), this, SLOT(previewStimulus(int)));
	connect(m_pStimulusOrderListWidget, SIGNAL(previewOrder(int)), this, SLOT(previewOrder(int)));
	connect(m_pStimulusSettingsListWidget, SIGNAL(clearStimulus()), this, SLOT(clearStimulus()));
	connect(m_pStimulusOrderListWidget, SIGNAL(clearStimulus()), this, SLOT(clearStimulus()));
	connect(m_pStimulusSettingsListWidget, SIGNAL(stimulusSelectionChanged()), this, SLOT(stimulusSelectionChanged()));
	connect(m_pStimulusOrderListWidget, SIGNAL(orderSelectionChanged()), this, SLOT(orderSelectionChanged()));
	connect(m_pStimulusSettingsListWidget, SIGNAL(stimulusAdded(int)), this, SLOT(stimulusAdded(int)));
	connect(m_pStimulusSettingsListWidget, SIGNAL(stimulusAboutToBeRemoved(int)), this, SLOT(stimulusAboutToBeRemoved(int)));
	connect(m_pStimulusSettingsListWidget, SIGNAL(stimulusSettingsChanged(int)), this, SLOT(stimulusSettingsChanged(int)));

	// when preview widget "stop" button is hit, the current playing stim is stopped and it switches to bkgd.
	connect(m_pStimulusPreviewWidget, SIGNAL(stopped()), this, SLOT(previewStopButtonHit()));
}


void HStimuliSettingsWidget::stimulusAdded(int row)
{
	// in the stimulusSettingsLIst widget a new stim was added. WE find the stim at row 'row'.
	qDebug() << "stimulus added at row " << row;
	qDebug() << m_stimuli.stimuli().at(row);

	// add this stimulus to the media manager.
	m_phgmm->addStimulus(m_stimuli.stimuli().at(row), m_context);
}

void HStimuliSettingsWidget::stimulusSettingsChanged(int row)
{
	// When this happens, force preview to the background
	clearStimulus();

	// replace stimulus in media manager
	QList<unsigned int> list = m_phgmm->getContextStimList(m_context);
	m_phgmm->replaceStimulus(list.at(row), m_stimuli.stimuli().at(row));
	m_pStimulusPreviewWidget->preview(list.at(row), true);

}

// Called before the StimulusSettingsListModel removes a row (a stimulus) from its list.
// Since that list is a reference to the master list held here, this is the place to
// make sure that HGMM is in sync with the stimuli remaining.
void HStimuliSettingsWidget::stimulusAboutToBeRemoved(int row)
{
	QList<unsigned int> list = m_phgmm->getContextStimList(m_context);

	// in the stimulusSettingsLIst widget a stim was selected and "Remove" was clicked.
	// We have to remove the stimulus from the media manager.
	m_phgmm->remove(list.at(row));
}


void HStimuliSettingsWidget::clearStimulus()
{
	m_pStimulusPreviewWidget->clear();
}

void HStimuliSettingsWidget::previewStimulus(int row)
{
	QList<unsigned int> list = m_phgmm->getContextStimList(m_context);
	qDebug() << "HStimuliSettingsWidget::previewStimulus at row " << row << " key " << list.at(row);
	m_pStimulusPreviewWidget->preview(list.at(row), true);
}

void HStimuliSettingsWidget::previewOrder(int row)
{
	QList<unsigned int> contextStimList = m_phgmm->getContextStimList(m_context);
	qDebug() << "HStimuliSettingsWidget::previewOrder at row " << row;
	QList< QPair<int, QString> > indexedOrderList;
	QString orderName;
	orderName = m_stimuli.orders().at(row).getName();

	qDebug() << "HStimuliSettingsWidget::previewOrder at row " << row << " name " << orderName;

	if (!m_stimuli.getIndexedOrderList(orderName, indexedOrderList))
	{
		qDebug() << "Cannot get order list for order \"" << orderName << "\"";
		qDebug() << m_stimuli;
	}
	else
	{
		QList<unsigned int> orderKeyList;
		QPair<int, QString> p;
		foreach(p, indexedOrderList)
		{
			//qDebug() << "index " << p.first << " label " << p.second << " key " << contextStimList[p.first];
			orderKeyList.push_back(contextStimList[p.first]);
		}
		m_pStimulusPreviewWidget->preview(orderKeyList);
	}
}

void HStimuliSettingsWidget::importClicked()
{
#if QT_VERSION >= 0x050000
	QString filename = QFileDialog::getOpenFileName(this, "", QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)[0]);
#else
	QString filename = QFileDialog::getOpenFileName(this, "", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
#endif


	bool b;
	Habit::StimulusSettingsList slist;
	Habit::HStimulusOrderList olist;
	bool bClobberAllStimuli = false;
	bool bClobberAllOrders = false;


	qDebug() << "Selected file " << filename;
	if (!filename.isEmpty())
	{
		b = importStimulusSettingsAndOrders(filename, slist, olist);
		qDebug() << slist.size() << "stim imported " << olist.size() << " orders imported";

		// We append the stimuli and orders to the widgets so the display isupdated correctly.
		// The widget and model use the same reference, so the local m_stimuli is also updated.
		// Check/verify that no duplicate names are added to the list

		QStringList stimNames = m_stimuli.getStimulusNames();
		QListIterator<Habit::StimulusSettings> imported_ss(slist);
		while (imported_ss.hasNext())
		{
			bool bAppend = false;
			bool bClobber = false;
			Habit::StimulusSettings ss = imported_ss.next();
			if (stimNames.contains(ss.getName()))
			{
				if (bClobberAllStimuli)
				{
					bAppend = false;
					bClobber = true;
				}
				else
				{
					int response = QMessageBox::warning(this, "Name collision!", QString("A stimulus named %1 already exists for this phase. Overwrite?").arg(ss.getName()),
							QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No, QMessageBox::Yes);
					if (response == QMessageBox::Yes)
					{
						bAppend = false;
						bClobber = true;
					}
					else if (response == QMessageBox::YesToAll)
					{
						bAppend = false;
						bClobber = bClobberAllStimuli = true;
					}
				}
			}
			else
			{
				bAppend = true;
				bClobber = false;
			}
			if (bAppend) m_pStimulusSettingsListWidget->append(ss);
			if (bClobber) m_pStimulusSettingsListWidget->clobber(ss);
		}

		// get list of stim names and send it to the StimulusOrderListWidget to verify orders.
		//m_stimuli.stimuli().names()
		// set new names in
		//m_pStimulusOrderListWidget->setStimulusNames(m_stimuli.stimuli().names());

		QStringList orderNames = m_stimuli.getOrderNames();
		QListIterator<Habit::HStimulusOrder> imported_orders(olist);
		while (imported_orders.hasNext())
		{
			bool bAppend = false;
			bool bClobber = false;
			Habit::HStimulusOrder so = imported_orders.next();
			if (orderNames.contains(so.getName()))
			{
				if (bClobberAllOrders)
				{
					bAppend = false;
					bClobber = true;
				}
				else
				{
					int response = QMessageBox::warning(this, "Name collision!", QString("An order named %1 already exists for this phase. Overwrite?").arg(so.getName()),
							QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No, QMessageBox::Yes);
					if (response == QMessageBox::Yes)
					{
						bAppend = false;
						bClobber = true;
					}
					else if (response == QMessageBox::YesToAll)
					{
						bAppend = false;
						bClobber = bClobberAllOrders = true;
					}
				}
			}
			else
			{
				bAppend = true;
			}
			if (bAppend) m_pStimulusOrderListWidget->append(so);
			if (bClobber) m_pStimulusOrderListWidget->clobber(so);
		}
	}
}

Habit::StimuliSettings HStimuliSettingsWidget::getStimuliSettings()
{
	Habit::StimuliSettings settings;
	settings.setStimuli(m_stimuli.stimuli());
	settings.setOrderList(m_stimuli.orders());
	return settings;
}

void HStimuliSettingsWidget::currentStimulusSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
	Q_UNUSED(previous);

	// look at current selection in order widget
	if (current.row() > -1)
	{
		m_pStimulusOrderListWidget->clearSelection();
	}
}

void HStimuliSettingsWidget::currentOrderSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
	Q_UNUSED(previous);
	//qDebug() << "HStimuliSettingsWidget::currentOrderSelectionChanged( " << current.row() << ", " << previous.row() << ")";
	if (current.row() > -1)
	{
		m_pStimulusSettingsListWidget->clearSelection();
	}
}

void HStimuliSettingsWidget::stimulusSelectionChanged()
{
	//qDebug() << "HStimuliSettingsWidget::stimulusSelectionChanged()";
	m_pStimulusOrderListWidget->clearSelection();
}

void HStimuliSettingsWidget::orderSelectionChanged()
{
	//qDebug() << "HStimuliSettingsWidget::orderSelectionChanged()";
	m_pStimulusSettingsListWidget->clearSelection();
}


// When "stop" button is hit in preview widget, it stops currently playing
// stim and swiutches to background. In this case we should de-select anything
// that's selected in either of the list views.
void HStimuliSettingsWidget::previewStopButtonHit()
{
	stimulusSelectionChanged();
	orderSelectionChanged();
}
