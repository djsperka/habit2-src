#ifndef IMAGEWIDGET_ORIGINAL_H
#define IMAGEWIDGET_ORIGINAL_H

#include <QtGui/QWidget>
#include <QtGui/QImage>

namespace System
{

class ImageWidget : public QWidget
{
	Q_OBJECT

public:
	ImageWidget(QWidget *parent = 0);
	~ImageWidget();
	bool setFileName(const QString& fileName, bool fullScreen = false, bool aspectRatio = false);
protected:
	void paintEvent ( QPaintEvent * event );

private:
	QImage _image;
};
};
#endif // IMAGEWIDGET_H
