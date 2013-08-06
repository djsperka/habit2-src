#ifndef STIMULUSSETTINGSEDITORDIALOG_H
#define STIMULUSSETTINGSEDITORDIALOG_H

#include "stimulussettings.h"

#ifndef USING_QT5
#include "mediasoundpreviewplayer.h"
#include <QtGui/QDialog>

class QWidget;
class QLabel;
class QLineEdit;
class QSlider;
class QComboBox;
class QGroupBox;
class QPushButton;

#else
#include "H5MediaSoundPreviewPlayer.h"
#include <QtWidgets>
#endif

class StimulusInfoForm;
class QVBoxLayout;

namespace GUILib {

class StimulusSettingsEditorDialog : public QDialog
{
	Q_OBJECT

public:
	static const QString LEFT_MONITOR;
	static const QString CENTER_MONITOR;
	static const QString RIGHT_MONITOR;
	static const QString INDEPENDENT_SOUND;
	static const QString SOUND_PREVIEW_PLAY;
	static const QString SOUND_PREVIEW_STOP;

public:
	StimulusSettingsEditorDialog(const Habit::StimulusSettings& ss, QWidget* w = 0);
	~StimulusSettingsEditorDialog();

public: 
	void setStimulus(const Habit::StimulusSettings& stimulus);
	Habit::StimulusSettings getStimulus();
	void initialize();

private:
	void createComponents();
	void createSoundOptionsSection();
	void doLayout();
	void doSoundOptionsLayout();
	void makeConnections();
	
	void initializeAudioBalance();
	void initializeSettingsAudioBalance();
	bool validate();
	bool isAnyMediaExists();

private slots:
	void fillConfigurationObject();
	void onSoundPreview();
	void onPlaybackFinished();
	void onLeftSliderValueChanged(int vol);
	void onCenterSliderValueChanged(int vol);
	void onRightSliderValueChanged(int vol);
	void onISSSliderValueChanged(int vol);
	void onClose();
	void onFormSettingsChanged();

signals:
	void configurationChanged(const Habit::StimulusSettings&);
	
private:
	QString currentSource_;
	QGroupBox* nameGroup_;
	QLineEdit* stimulusName_;
	StimulusInfoForm* leftEnabled_;
	StimulusInfoForm* centerEnabled_;
	StimulusInfoForm* rightEnabled_;
	StimulusInfoForm* independentSoundEnabled_;

	QGroupBox* soundOptionsGroup_;	
	QPushButton* previewSound_;
	QGroupBox* audioBalanceGroup_;	
	QSlider* audioBalance_; 

	QVBoxLayout* mainLayout_;
	QPushButton* okButton_;
	QPushButton* cancelButton_;

	Habit::StimulusSettings settings_;

	QSlider * audioBalanceLeft_;
	QSlider * audioBalanceRight_;
	QSlider * audioBalanceCenter_;
	QSlider * audioBalanceISS_;
	static MediaSoundPreviewPlayer* _mediaSoundPreview;
};

} // namespace GUILib

#endif // STIMULUSSETTINGSEDITORDIALOG_H
