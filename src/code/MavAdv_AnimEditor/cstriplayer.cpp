#include <mainwindow.h>
#include "cstriplayer.h"

#include <QImage>
#include <QInputDialog>

cStripLayer::cStripLayer(QObject *parent) : cBaseNode(parent)
{
	
}

cStripLayer::~cStripLayer()
{
	for(int i=m_StripFrames.size()-1;i>=0;i--)
		delete m_StripFrames[i];
}

void cStripLayer::LoadStrip(QString filename_numframes) //filename_numframes.png
{
	for(int i=m_StripFrames.size()-1;i>=0;i--)
		delete m_StripFrames[i];
	m_StripFrames.clear();
	
	QImage strip(filename_numframes);
	if (strip.isNull())
	{
		//can't find file
		return;
	}
	
	int w,h, numfr;
	h = strip.height();
	w = strip.width();
	QStringList parts = filename_numframes.split("_",QString::SkipEmptyParts);
	if (parts.size() < 1)
	{
		bool ok;
		numfr = QInputDialog::getInt(MainWindow::s_WND, tr("Input numframes"),tr(""), 1, 1, INT_MAX,1,&ok);
		if (!ok)
		{
			//user pressed 'Cancel'
			return;
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
			numfr = QInputDialog::getInt(MainWindow::s_WND, tr("Input numframes"),tr(""), 1, 1, INT_MAX,1,&ok);
			if (!ok)
			{
				//user pressed 'Cancel'
				return;
			}
		}
	}
	if (numfr != 0)
		w /= numfr;
	
	for(int i=0; i<numfr; i++)
	{
		m_StripFrames.push_back( new QImage( strip.copy(i*w, 0, w, h) ) );
	}
}
