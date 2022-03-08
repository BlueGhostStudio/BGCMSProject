/*****************************************************************************
 * RPCLog_global.h
 *
 * Created: 2022/2/8 2022 by blueghost
 *
 * Copyright 2022 blueghost. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#ifndef RPCLOG_GLOBAL_H
#define RPCLOG_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RPCLOG_LIBRARY)
#define RPCLOG_EXPORT Q_DECL_EXPORT
#else
#define RPCLOG_EXPORT Q_DECL_IMPORT
#endif

#endif  // RPCLOG_GLOBAL_H