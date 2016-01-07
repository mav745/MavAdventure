#include <mainwindow.h>
#include "cstriplayer.h"

#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QInputDialog>

cStripLayer::cStripLayer(int type) : cBaseNode(type)
{
	m_Type = ENT_STRIP;
}

cStripLayer::~cStripLayer()
{
	for(int i=m_StripFrames.size()-1;i>=0;i--)
		delete m_StripFrames[i];
}

cStripLayer *cStripLayer::GetNewStrip(QString filename_numframes) //filename_numframes.png
{
	int w,h,numfr;
	
	QImage strip(filename_numframes);
	if (strip.isNull())
	{
		//can't find file
		return NULL;
	}
	
	w = strip.height();
	h = strip.width();
	QStringList parts = filename_numframes.split("_",QString::SkipEmptyParts);
	if (parts.size() < 1)
	{
		bool ok;
		numfr = QInputDialog::getInt(MainWindow::s_WND, "Input numframes","", 1, 1, INT_MAX,1,&ok);
		if (!ok)
		{
			//user pressed 'Cancel'
			return NULL;
		}
	}
	else
	{
		bool ok;
		QString lastpart = parts[parts.size()-1];
		lastpart = lastpart.left(lastpart.size()-4); //get rid of .png
		numfr = lastpart.toInt(&ok);
		if (!ok)
		{
			numfr = QInputDialog::getInt(MainWindow::s_WND, "Input numframes","", 1, 1, INT_MAX,1,&ok);
			if (!ok)
			{
				//user pressed 'Cancel'
				return NULL;
			}
		}
	}
	if (numfr != 0)
		h /= numfr;
	
	cStripLayer *pLayer = new cStripLayer;
	pLayer->m_FrameH = h;
	pLayer->m_FrameW = w;
	pLayer->m_NumFrames = numfr;
	pLayer->m_Name = QFileInfo(filename_numframes).fileName();
	for(int i=0; i<numfr; i++)
	{
		pLayer->m_StripFrames.push_back( new QImage( strip.copy(i*h, 0, h, w) ) );
	}
	return pLayer;
}
