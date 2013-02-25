/*
 File name: loiteringTLE.cpp
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */

/*
 ------------------ Author:       Guillermo Ortega               -------------------
 ------------------ Affiliation:  European Space Agency (ESA)    -------------------
 -----------------------------------------------------------------------------------

*/

#include "Loitering/loiteringTLE.h"
#include "Main/scenariotree.h"
#include "Scenario/scenario.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"

#include <QtGui>
#include <iostream>
#include <QTextStream>

#include "ui_loiteringTLE.h"


QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE
class DropArea;

LoiteringTLEDialog::LoiteringTLEDialog(ScenarioTree* parent) :
    QDialog(parent)
{
    setupUi(this);

    // Set up the input validators
    QDoubleValidator* angleValidator = new QDoubleValidator(this);
    angleValidator->setBottom(0.0);
    angleValidator->setTop(360.0);
    QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(this);
    positiveDoubleValidator->setBottom(0.0);
    QDoubleValidator* zeroToOneValidator = new QDoubleValidator(this);
    zeroToOneValidator->setBottom(0.0);
    zeroToOneValidator->setTop(0.9999);
    QDoubleValidator* minusOneToOneValidator = new QDoubleValidator(this);
    minusOneToOneValidator->setBottom(-1.0);
    minusOneToOneValidator->setTop(1.0);

    TimeStepLineEdit->setValidator(positiveDoubleValidator);

    // This creates the drop area inside a QFormLayout called "DropTLEhere" in the UI file
    //DropArea *dropArea;
    dropArea = new DropArea;
    dropArea->setMaximumSize(106, 55); dropArea->setMinimumSize(106, 55); // Set the size
    DropTLEhere->addWidget(dropArea); //Add the widget into the desired area

    // Feeding back what we read as TLE file
    connect(dropArea, SIGNAL(changed(const QMimeData *)), this, SLOT(on_TLE_dropped(const QMimeData *)));
}



LoiteringTLEDialog::~LoiteringTLEDialog()
{

}




/////////////////////////////////// FUNCTIONS ///////////////////////////////////

bool LoiteringTLEDialog::loadValues(ScenarioTleTrajectory* loiteringTLE)
{

    // Loading the time line and the time step
    ScenarioExtendedTimeline* timeline = loiteringTLE->timeline();
    QDateTime startTime = sta::JdToCalendar(sta::MjdToJd(timeline->startTime()));
    QDateTime endTime = sta::JdToCalendar(sta::MjdToJd(timeline->endTime()));

    if (timeline)
    {
	StartDateTimeEdit->setDateTime(startTime);
	EndDateTimeEdit->setDateTime(endTime);
	TimeStepLineEdit->setText(QString::number(timeline->timeStep()));

	// Loading default values for ISSfor the GUI
	QString TLE_line_0  = "ISS (ZARYA)";
	QString TLE_line_1  = "1 25544U 98067A   09282.91732946  .00013034  00000-0  90850-4 0  7559";
	QString TLE_line_2  = "2 25544  51.6398  77.7469 0007731 180.0753 317.5762 15.74717336624025";
	TLEline0Edit->setText(TLE_line_0);
	TLEline1Edit->setText(TLE_line_1);
	TLEline2Edit->setText(TLE_line_2);
	//Writting the values on the variable
	loiteringTLE->setTleLine1(TLE_line_1);
	loiteringTLE->setTleLine2(TLE_line_2);

	return true;
     }
    else
    {
	return false;
    }
} // End of LoiteringTLEDialog::loadValues



void LoiteringTLEDialog::on_LoadTLEpushButton_clicked()
{
    QTextStream out (stdout);

    QString TLEFileName;
    QString TLE_line_1;
    QString TLE_line_2;

    // Loading now the TLE file
    QString CompleteTLEFileName = QFileDialog::getOpenFileName(this, tr("Select TLE File"),
							     QString("./TLEs/"),
							     tr("TLEs (*.tle *.TLE)"));

    if (!CompleteTLEFileName.isEmpty())
    {
	// Strip away the path--we just want the filename
	if (CompleteTLEFileName.contains('/'))
	{
	    TLEFileName = CompleteTLEFileName.remove(0, TLEFileName.lastIndexOf('/') + 1);
	    //out << "TLEFileName: " << TLEFileName << endl;
	};

	// Reading the TLE file
	QFile TLEfile(CompleteTLEFileName);
	TLEfile.open(QIODevice::ReadOnly);
	QTextStream StreamWithTLEs(&TLEfile);
	QString NameOfParticipant = StreamWithTLEs.readLine();
	//out << "NameOfParticipant: " << NameOfParticipant << endl;
	TLE_line_1 = StreamWithTLEs.readLine();
	//out << "TLE_line_1: " << TLE_line_1 << endl;
	TLE_line_2 = StreamWithTLEs.readLine();
	//out << "TLE_line_2: " << TLE_line_2 << endl;
	TLEfile.close();

	//Updating the GUI
	LoiteringTLEDialog::TLEline0Edit->setText(NameOfParticipant);
	LoiteringTLEDialog::TLEline1Edit->setText(TLE_line_1);
	LoiteringTLEDialog::TLEline2Edit->setText(TLE_line_2);

     }
}



bool LoiteringTLEDialog::saveValues(ScenarioTleTrajectory* loiteringTLE)
{
    // Saving the time line that constains start date, end date and time step
    ScenarioExtendedTimeline* timeline = loiteringTLE->timeline();
    timeline->setStartTime(sta::JdToMjd(sta::CalendarToJd(StartDateTimeEdit->dateTime())));
    timeline->setEndTime(sta::JdToMjd(sta::CalendarToJd(EndDateTimeEdit->dateTime())));
    timeline->setTimeStep(TimeStepLineEdit->text().toDouble());

    // Saving now the TLE parameters
    loiteringTLE->setTleLine0(TLEline0Edit->text()); //The name of the vehicle
    loiteringTLE->setTleLine1(TLEline1Edit->text()); //The first TLE line
    loiteringTLE->setTleLine2(TLEline2Edit->text()); //The second TLE line
    return true;
}



void LoiteringTLEDialog::on_TLE_dropped(const QMimeData *mimeData)
{
    QTextStream out (stdout);
    QString TLE_line_0;
    QString TLE_line_1;
    QString TLE_line_2;
    QString DroppedTLEFileName;

    QList<QUrl> urlList = mimeData->urls();
    for (int i = 0; i < urlList.size() && i < 32; ++i)
    {
	QString url = urlList.at(i).path();
	DroppedTLEFileName += url;
    }

    if (!DroppedTLEFileName.isEmpty())
    {
	// Reading the TLE file
	QFile TLEfile(DroppedTLEFileName);
	TLEfile.open(QIODevice::ReadOnly);
	QTextStream StreamWithTLEs(&TLEfile);
	TLE_line_0 = StreamWithTLEs.readLine();
	//out << "TLE_line_0: " << TLE_line_0 << endl;
	TLE_line_1 = StreamWithTLEs.readLine();
	//out << "TLE_line_1: " << TLE_line_1 << endl;
	TLE_line_2 = StreamWithTLEs.readLine();
	//out << "TLE_line_2: " << TLE_line_2 << endl;
	TLEfile.close();

	LoiteringTLEDialog::TLEline0Edit->setText(TLE_line_0);
	LoiteringTLEDialog::TLEline1Edit->setText(TLE_line_1);
	LoiteringTLEDialog::TLEline2Edit->setText(TLE_line_2);
     }

}



// This creates a generic drop area
DropArea::DropArea(QWidget *parent)
    : QLabel(parent)
{
    //setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setFrameStyle(QFrame::Sunken);
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
    setAutoFillBackground(true);
    clear();
}



void DropArea::dragEnterEvent(QDragEnterEvent *event)
{
    setText(tr("drop TLE"));
    setBackgroundRole(QPalette::Highlight);

    event->acceptProposedAction();
    emit changed(event->mimeData());
}




void DropArea::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}


// What to do when dropping a file into the drop area
void DropArea::dropEvent(QDropEvent *event)
{

    QTextStream out (stdout);
    QString DroppedTLEFileName, TLEFileName;
    QString TLE_line_0;
    QString TLE_line_1;
    QString TLE_line_2;

    const QMimeData *mimeData = event->mimeData();

    /*
    if (mimeData->hasText())
    {
	//setText(mimeData->text());
	//setTextFormat(Qt::PlainText);
	QMessageBox::information(this, tr("Good TLE file"), tr("This is a TLE file"));
    }
    else if (mimeData->hasUrls())
    {
	QList<QUrl> urlList = mimeData->urls();
	QString text;
	for (int i = 0; i < urlList.size() && i < 32; ++i) {
	    QString url = urlList.at(i).path();
	    text += url + QString("\n");
	}
	setText(text);
    }
    else
    {
	QMessageBox::information(this, tr("Bad TLE file"), tr("This is not a TLE file: \nError loading TLEs"));
    }
    */

    QList<QUrl> urlList = mimeData->urls();
    for (int i = 0; i < urlList.size() && i < 32; ++i)
    {
	QString url = urlList.at(i).path();
	DroppedTLEFileName += url;
    }


    // Strip away the path--we just want the filename
    if (DroppedTLEFileName.contains('/'))
    {
	TLEFileName = DroppedTLEFileName.remove(0, DroppedTLEFileName.lastIndexOf('/') + 1);
	//out << "TLEFileName: " << TLEFileName << endl;
    };
    setText(TLEFileName);

    setBackgroundRole(QPalette::Dark);
    event->acceptProposedAction();
}



void DropArea::dragLeaveEvent(QDragLeaveEvent *event)
{
    clear();
    event->accept();
}



void DropArea::clear()
{
    setText(tr("drop TLE"));
    setBackgroundRole(QPalette::Dark);
    emit changed();
}

