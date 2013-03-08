/*
 *  HPhaseCriteriaUtil.h
 *  habutil
 *
 *  Created by Oakes Lab on 3/4/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "HPhaseCriteria.h"
#include "habituationsettings.h"
#include "criterionsettings.h"
#include <QtDebug>
#include <QtGlobal>

HPhaseCriteria* createPhaseCriteria(const Habit::HabituationSettings& habsettings, int ntrials);
