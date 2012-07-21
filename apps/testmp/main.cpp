/*
 *  main.cpp
 *  myp
 *
 *  Created by Oakes Lab on 5/17/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "PlayerControlDialog.h"
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	
/*	
	ImageWidget im(0, true, true);
	im.setFileName("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/husky.jpg");
	im.show();
*/
	
	PlayerControlDialog dialog;
	dialog.show();

	return app.exec();
}
