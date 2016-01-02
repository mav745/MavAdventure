#include "cdlgentprops.h"
#include "ui_cdlgentprops.h"
#include "cBaseEntityTool.h"

#include <QDebug>

//typedef QMap<QString,value_s*> values_map_t;



bool gIgnoreChange = false;


void cDlgEntProps::DoubleClicked(int y, int x)
{
	if (x != 1) return;
	value_s * pVal = ValuePointerByRow(y);
	if (!pVal) return;
	
	if (!pVal->AskValue(ui->table_EntFields->item(y,0)->text()))
	{
		ui->table_EntFields->editItem(ui->table_EntFields->item(y,1));
		//pVal->AskValue(ui->table_EntFields->item(y,1);
	}
}

cDlgEntProps::cDlgEntProps(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cDlgEntProps)
{
	ui->setupUi(this);
	QStringList hdrNames;
	hdrNames.push_back("Field");
	hdrNames.push_back("Value");
	ui->table_EntFields->setHorizontalHeaderLabels(hdrNames);
	ui->table_EntFields->verticalHeader()->hide();
	setWindowTitle("Entity properties");
	
	connect(ui->table_EntFields,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(DoubleClicked(int,int)));
	
	connect(ui->but_OK,SIGNAL(clicked()),this,SLOT(OKHit()));
	connect(ui->table_EntFields,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(itemChanged(QTableWidgetItem*)));
	connect(ui->list_entTypes,SIGNAL(currentTextChanged(QString)),this,SLOT(entityChanged(QString)));
	connect(ui->but_OKMulSel,SIGNAL(clicked()),this,SLOT(OKHit()));
	
	m_pEditObj = NULL;
	m_pCallingTool = NULL;
	
	ui->frame_MulSel->hide();
}

cDlgEntProps::~cDlgEntProps()
{
	delete ui;
}
void cDlgEntProps::closeEvent(QCloseEvent *e)
{
	OKHit();
}

void cDlgEntProps::ShowNormal()
{
	ui->frame_Main->show();
	ui->frame_MulSel->hide();
	show();
}

void cDlgEntProps::ShowWrongSel(const QString &reason)
{
	ui->label_MulSel->setText(reason);
	ui->frame_Main->hide();
	ui->frame_MulSel->show();
	show();
}

value_s * cDlgEntProps::ValuePointerByRow(int row)
{
	if (!m_pEditObj) return NULL;
	if (!m_pEditObj->m_pEntity) return NULL;
	
	QTableWidgetItem *item = ui->table_EntFields->item(row,1);
	if (!item) return NULL;
	
	const QString &fieldname = ui->table_EntFields->item(item->row(),0)->text();
	if (!m_pEditObj->m_pEntity->fields.contains(fieldname)) return NULL;
	
	return m_pEditObj->m_pEntity->fields[fieldname];
}

void cDlgEntProps::itemChanged(QTableWidgetItem *item)
{
	if (item->column() != 1) return;
	if (!m_pEditObj) return;
	if (!m_pEditObj->m_pEntity) return;
	
	//qDebug()<<"field"<<ui->table_EntFields->item(item->row(),0)->text();
	//qDebug()<<"value"<<item->text();
	//qDebug()<<"--------";
	const QString &fieldname = ui->table_EntFields->item(item->row(),0)->text();
	if (!m_pEditObj->m_pEntity->fields.contains(fieldname)) return;
	
	m_pEditObj->m_pEntity->fields[fieldname]->fromString(item->text());
//	SetEntField(ui->table_EntFields->item(item->row(),0)->text(),
//				item->text());
}

void cDlgEntProps::FillTableFromFields(entity_t *pEnt)
{
	for(int i=ui->table_EntFields->rowCount()-1;i>=0;i--)
	{
		delete ui->table_EntFields->item(i,0);
		delete ui->table_EntFields->item(i,1);
	}
	
	ui->table_EntFields->clearContents();
	
	QMap<QString, value_s*>::iterator it2;
	int i;
	for(i = 0, it2 = pEnt->fields.begin();
		 it2 != pEnt->fields.end();
		 i++, it2++)
	{
		if (it2.key()[0] == '@')
		{
			i--;
			continue;
		}
		ui->table_EntFields->setRowCount(i+1);
		QTableWidgetItem *pItem = new QTableWidgetItem(it2.key());
	
		pItem->setFlags(pItem->flags() & (~(Qt::ItemIsEditable | Qt::ItemIsEnabled)));
		ui->table_EntFields->setItem(i,0,pItem);
		ui->table_EntFields->setItem(i,1,new QTableWidgetItem(it2.value()->asString()));
	}
}

void cDlgEntProps::SelectObject(cBase *pObj)
{
	m_pEditObj = pObj;

	gIgnoreChange = true;
	ui->list_entTypes->setCurrentText(pObj->m_pEntity->type_name);
	gIgnoreChange = false;

	FillTableFromFields(pObj->m_pEntity);
}

void cDlgEntProps::SetCallingTool(cBaseEntityTool *pTool)
{
	//m_pCallingTool = pTool;
	FillList(pTool->m_EntsMap.keys());
}

//void cDlgEntProps::SetEntsList(const QStringList &ents_list)
//{
//	FillList(ents_list);
//}

void cDlgEntProps::FillList(const QStringList &ents_list)
{
	QStringList realList = ents_list;
	for(int i=0;i<realList.size();)
	{
		if (realList[i][0] == '@')
		{
			realList.removeAt(i);
			continue;
		}
		i++;
	}
	ui->list_entTypes->clear();
	ui->list_entTypes->addItems(realList);
}

void cDlgEntProps::OKHit(void)
{
	m_pEditObj = NULL;
	m_pCallingTool = NULL;
	this->hide();
	ui->frame_Main->show();
	ui->frame_MulSel->hide();
}

//void cDlgEntProps::CancelHit(void)
//{
//	m_pEditObj = NULL;
//	m_pCallingTool = NULL;
//	this->hide();
//	ui->frame_Main->show();
//	ui->frame_MulSel->hide();
//}

void cDlgEntProps::entityChanged(QString newname)
{
	if (!m_pEditObj) return;
	if (gIgnoreChange)
	{
		//gIgnoreChange = false;
		return;
	}
	qDebug()<<"entityChanged"<<newname;
	m_pEditObj->SetEntityType(newname);//m_pCallingTool->EntCreateByName()
	if (m_pEditObj->m_pEntity)
		FillTableFromFields(m_pEditObj->m_pEntity);
}
