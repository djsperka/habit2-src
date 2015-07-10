/*
 * HStimuliSettingsWidget.cpp
 *
 *  Created on: Oct 21, 2014
 *      Author: Oakeslab
 */

#include "HStimuliSettingsWidget.h"
#include "HStimulusSettingsWidget.h"
#include "HStimulusSettingsOrderImportUtil.h"
#include "HWorkspaceUtil.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>

using namespace Habit;
using namespace GUILib;

HStimuliSettingsWidget::HStimuliSettingsWidget(const StimuliSettings& stimuli, const StimulusDisplayInfo& info, QWidget* parent)
: QWidget(parent)
, m_stimuli(stimuli)
, m_stimulusDisplayInfo(info)
{
	create();
	connections();
}

void HStimuliSettingsWidget::create()
{
	QDir root;
	habutilGetStimulusRootDir(root);

	m_pStimulusSettingsListWidget = new HStimulusSettingsListWidget(m_stimuli.stimuli(), m_stimulusDisplayInfo.getStimulusLayoutType());
	m_pStimulusOrderListWidget = new HStimulusOrderListWidget(m_stimuli.orders(), m_stimuli.stimuli(), m_stimuli.getStimContext(), m_stimulusDisplayInfo.getStimulusLayoutType());
	m_pStimulusPreviewWidget = new HStimulusPreviewWidget(m_stimulusDisplayInfo, root, this);

	QGroupBox *g1 = new QGroupBox(QString("%1 Stimuli").arg(m_stimuli.getStimContext().name()));
	QVBoxLayout *v1 = new QVBoxLayout;
	v1->addWidget(m_pStimulusSettingsListWidget);
	g1->setLayout(v1);
	QGroupBox *g2 = new QGroupBox(QString("%1 Orders").arg(m_stimuli.getStimContext().name()));
	QVBoxLayout *v2 = new QVBoxLayout;
	v2->addWidget(m_pStimulusOrderListWidget);
	g2->setLayout(v2);

	QHBoxLayout *h12 = new QHBoxLayout;
	h12->addWidget(g1);
	h12->addWidget(g2);


	QGroupBox *g3 = new QGroupBox("Import");
	QHBoxLayout *h3 = new QHBoxLayout;
	QLabel *l3 = new QLabel("Stimuli and/or orders for this phase can be imported from a text file.");
	m_pbImport = new QPushButton("Import");
	l3->setWordWrap(true);
	h3->addWidget(l3, 1);
	h3->addWidget(m_pbImport, 1);
	g3->setLayout(h3);

	QVBoxLayout *v = new QVBoxLayout;
//	v->addWidget(g1);
//	v->addWidget(g2);
	v->addLayout(h12);
	v->addWidget(m_pStimulusPreviewWidget);
	v->addStretch(1);
	v->addWidget(g3);
	setLayout(v);
}

void HStimuliSettingsWidget::connections()
{
	connect(m_pbImport, SIGNAL(clicked()), this, SLOT(importClicked()));
	connect(m_pStimulusSettingsListWidget, SIGNAL(previewStimulus(int)), this, SLOT(previewStimulus(int)));
	connect(m_pStimulusSettingsListWidget, SIGNAL(clearStimulus()), this, SLOT(clearStimulus()));
}

void HStimuliSettingsWidget::clearStimulus()
{
	m_pStimulusPreviewWidget->clear();
}

void HStimuliSettingsWidget::previewStimulus(int row)
{
	qDebug() << "HStimulusSettingsWidget::previewStimulus " << m_stimuli.stimuli().at(row).getName();
	m_pStimulusPreviewWidget->preview(m_stimuli.stimuli().at(row));
}

void HStimuliSettingsWidget::importClicked()
{
	QString filename = QFileDialog::getOpenFileName(this, "", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
	bool b;
	Habit::StimulusSettingsList slist;
	Habit::HStimulusOrderList olist;
	bool bClobberAllStimuli = false;
	bool bClobberAllOrders = false;

	qDebug() << "Selected file " << filename;
	if (!filename.isEmpty())
	{
		b = importStimulusSettingsAndOrders(filename, slist, olist, m_stimuli.getStimContext());
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


void HStimuliSettingsWidget::stimulusLayoutTypeChanged(int i)
{
	m_pStimulusSettingsListWidget->setStimulusLayoutType(m_stimulusDisplayInfo.getStimulusLayoutType());
	m_pStimulusOrderListWidget->setStimulusLayoutType(m_stimulusDisplayInfo.getStimulusLayoutType());
}

Habit::StimuliSettings HStimuliSettingsWidget::getStimuliSettings()
{
	Habit::StimuliSettings settings(m_stimuli.getStimContext());
	settings.setStimuli(m_stimuli.stimuli());
	settings.setOrderList(m_stimuli.orders());
	return settings;
}
