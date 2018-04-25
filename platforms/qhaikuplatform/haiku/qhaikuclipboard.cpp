/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Copyright (C) 2015-2017 Gerasim Troeglazov,
** Contact: 3dEyes@gmail.com
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#if !defined(QT_NO_CLIPBOARD)

#include "qhaikuclipboard.h"

#include <QtGui/QColor>

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

#include <String.h>
#include <Clipboard.h>

QT_BEGIN_NAMESPACE

QHaikuClipboard::QHaikuClipboard()
{
}

QHaikuClipboard::~QHaikuClipboard()
{   
}

void QHaikuClipboard::setMimeData(QMimeData *data, QClipboard::Mode mode)
{
    if (mode != QClipboard::Clipboard)
        return;

	if (be_clipboard->Lock()) {
		be_clipboard->Clear();
		if (data){
			BMessage* clip = (BMessage *)NULL;
	    	if( (clip = be_clipboard->Data()) != NULL) {
	    		QStringList formats = data->formats();
				for(int f = 0; f < formats.size(); ++f) {
            		QString mimeType = formats.at(f);	    			
					clip->AddData(mimeType.toUtf8(), B_MIME_TYPE, data->data(mimeType).data(), data->data(mimeType).count());
	    		}
	    	}
		}
		be_clipboard->Commit();
	   	be_clipboard->Unlock();
	}
}

QMimeData *QHaikuClipboard::mimeData(QClipboard::Mode mode)
{
    if (mode != QClipboard::Clipboard)
        return 0;

	QMimeData *md = new QMimeData();

	BMessage* clip = (BMessage *)NULL;
  	if (be_clipboard->Lock()) {
    	if( (clip = be_clipboard->Data()) != NULL) {
    		BMessage *msg = (BMessage*)(be_clipboard->Data());
    		
			char *name;
			uint32 type;
			int32 count;

			for ( int i = 0; msg->GetInfo(B_MIME_TYPE, i, &name, &type, &count) == B_OK; i++ ) {
				const void *data;
				ssize_t dataLen = 0;
				status_t stat = msg->FindData(name,B_MIME_TYPE,&data,&dataLen);
				if(dataLen && stat==B_OK)	{
					QString mime(name);
					if(mime=="text/plain") {
						QString text = QString::fromUtf8((const char*)data, dataLen);
						md->setText(text);
					} else if(mime=="text/html") {
						QString html = QString::fromUtf8((const char*)data, dataLen);
						md->setHtml(html);
					} else {
						QByteArray clip_data((const char*)data, dataLen);
						md->setData(mime,clip_data);
					}
				}
			}		
			be_clipboard->Unlock();
    	}
	}
	return md;
}

QT_END_NAMESPACE

#endif //QT_NO_CLIPBOARD
