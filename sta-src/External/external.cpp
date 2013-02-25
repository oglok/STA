/*
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

#include "External/external.h"
#include "Main/scenariotree.h"
#include "Scenario/scenario.h"
#include "Astro-Core/date.h"
#include "Astro-Core/stacoordsys.h"

#include <QtGui>
#include <iostream>
#include <QTextStream>
#include <QTemporaryFile>

#include "ui_external.h"


QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE
class DropArea;

ExternalDialog::ExternalDialog(ScenarioTree* parent) :
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

    // This creates the drop area inside a QFormLayout called "DropExternalhere" in the UI file
    ExternaldropArea = new ExternalDropArea;
    ExternaldropArea->setMaximumSize(106, 55); ExternaldropArea->setMinimumSize(106, 55); // Set the size
    DropExternalhere->addWidget(ExternaldropArea); //Add the widget into the desired area

    // Feeding back what we read as External file
    connect(ExternaldropArea, SIGNAL(changed(const QMimeData *)), this, SLOT(on_ExternalFile_dropped(const QMimeData *)));
}



ExternalDialog::~ExternalDialog()
{

}




/////////////////////////////////// FUNCTIONS ///////////////////////////////////

bool ExternalDialog::loadValues(ScenarioExternalTrajectory* externalTrajectory)
{
    // Loading the time line and the time step
    ScenarioExtendedTimeline* timeline = externalTrajectory->timeline();
    QDateTime startTime = sta::JdToCalendar(sta::MjdToJd(timeline->startTime()));
    QDateTime endTime = sta::JdToCalendar(sta::MjdToJd(timeline->endTime()));

    if (timeline)
    {
	StartDateTimeEdit->setDateTime(startTime);
	EndDateTimeEdit->setDateTime(endTime);
	TimeStepLineEdit->setText(QString::number(timeline->timeStep()));

        // Loading default values for ISS for the GUI
        QString StateVector1  = "2455563, 6778, 0, 0, 0, 4.76335586717, 6.00985940331";
        QString StateVector2  = "2455563.0006944, 6762.38865439, 285.581895497, 360.314678963, -0.520178335644, 4.75238472603, 5.99601726814";
        ExternalDialog::FileTextEdit->setPlainText(StateVector1 + "\n" + StateVector2 + "\n");

        //Writting the values on the variable
        externalTrajectory->setFirstExternalPropagatedLine(StateVector1);
        externalTrajectory->setLastExternalPropagatedLine(StateVector2);

	return true;
     }
    else
    {
	return false;
    }
} // End of LoiteringTLEDialog::loadValues



void ExternalDialog::on_LoadExternalpushButton_clicked()
{
    QTextStream out (stdout);

    QString ExternalFileName;
    QString PropagatedLines = "";
    int NumberOfItems;
    int NumberOfStateVectorsRead;
    QStringList ListOfItems;

    // Loading now the EXTERNAL file
    // Any external file must be first renamed with an extension *.stae
    /* Recalling STA file name convention:
        *.stae  Ephemeris
        *.staa Attitude
        *.stag ground track
        *.stam miscelaneous
        *.stap pipes between GUI and modules
        *.stal locations of ground stations or launch pads, etc.
        *.stat time-lines of mission arcs or maneuvers
        *.sta.txt temporary files
        *.stad databases (atmosphere, CD, CL, etc)
    */
    QString CompleteExternalFileName = QFileDialog::getOpenFileName(this, tr("Select External File"),
                                                             QString("./external/"),
                                                             tr("TLEs (*.stae *.STAE)"));

    if (!CompleteExternalFileName.isEmpty())
    {
        // Reading the External file
        QFile ExternalFile(CompleteExternalFileName);
        ExternalFile.open(QIODevice::ReadOnly);
        QTextStream StreamWithExternals(&ExternalFile);

        // TODO read any external file format here


        NumberOfStateVectorsRead = 0;
        while (!StreamWithExternals.atEnd())
        {
            PropagatedLines = PropagatedLines + StreamWithExternals.readLine() + "," + "\n";      //read in a line
            //out << "PropagatedLines: " << PropagatedLines << endl;
            NumberOfStateVectorsRead = NumberOfStateVectorsRead + 1;
        }
        ExternalFile.close();

        //PropagatedLines = StreamWithExternals.readAll();

        // Spliting now the big string into a list of strings using the comma as separator
        ListOfItems = PropagatedLines.split(",");

        ///////// The rest from this line onwards should be common to all external files ///////////
        NumberOfItems = ListOfItems.size();
        //out << "NumberOfItems: " << NumberOfItems << endl;

        /*
        int i;
        for (i=0; i<NumberOfItems; i++)
        {
            out << "Number " << i <<": " << ListOfItems.value(i) << endl;
        }
        */

        if (ListOfItems.size() > 7)  // Atempting to read more than 1 line of state vectors
        {
           // Obtaining the time line. We now that the first item and the last-6 item are the times we are looking for
           StartDateTimeEdit->setDateTime(sta::JdToCalendar(ListOfItems.value(0).toDouble()));
           //out << "StartDateTimeEdit: " << ListOfItems.value(0).toDouble() << endl;
           EndDateTimeEdit->setDateTime(sta::JdToCalendar(ListOfItems.value(NumberOfStateVectorsRead * 7 - 7).toDouble()));
           //out << "EndDateTimeEdit: " << ListOfItems.value(NumberOfStateVectorsRead * 7 - 7).toDouble() << endl;
           // Obtaining the time step
           double TimeStepInJulianDates = ListOfItems.value(NumberOfStateVectorsRead * 7 - 7).toDouble() - ListOfItems.value(0).toDouble();
           //out << "TimeStepInJulianDates: " << TimeStepInJulianDates << endl;
           QString TimeStepString = TimeStepString.setNum(floor(TimeStepInJulianDates*86400/NumberOfStateVectorsRead + 0.5));
           TimeStepLineEdit->setText(TimeStepString);

           //Updating the GUI of the text window now
           ExternalDialog::FileTextEdit->setPlainText(PropagatedLines);
        }
        else
        {
            QMessageBox::information(this, tr("Bar External file"), tr("Not very nice... \nThis file is almost empty"));
        }
    //////////////////////////////////////////
    // Now storing the read file into a temporary file for the 'abstract" trajectory method to read and plot
    QFile ExternalFileToWrite("ephem-external.stae");   // This file if fixed. TODO make it generic
    ExternalFileToWrite.open(QIODevice::WriteOnly);
    QTextStream StreamWithExternalsToWrite(&ExternalFileToWrite);
    // Output values at full double precision
    StreamWithExternalsToWrite.setRealNumberPrecision(16);

    // Write all samples
    for (int i = 0; i < NumberOfStateVectorsRead; i = i + 7)
    {
        StreamWithExternalsToWrite  << ListOfItems.value(i+0).toDouble()  << ", "
                                    << ListOfItems.value(i+1).toDouble()  << ", "
                                    << ListOfItems.value(i+2).toDouble()  << ", "
                                    << ListOfItems.value(i+3).toDouble()  << ", "
                                    << ListOfItems.value(i+4).toDouble()  << ", "
                                    << ListOfItems.value(i+5).toDouble()  << ", "
                                    << ListOfItems.value(i+6).toDouble()  << endl;
    }

    ExternalFileToWrite.close();

    }
}  ////////////////////// end of void ExternalDialog::on_LoadExternalpushButton_clicked()






bool ExternalDialog::saveValues(ScenarioExternalTrajectory* externalTrajectory)
{
    // Saving the time line that constains start date, end date and time step
    ScenarioExtendedTimeline* timeline = externalTrajectory->timeline();
    timeline->setStartTime(sta::JdToMjd(sta::CalendarToJd(StartDateTimeEdit->dateTime())));
    timeline->setEndTime(sta::JdToMjd(sta::CalendarToJd(EndDateTimeEdit->dateTime())));
    timeline->setTimeStep(TimeStepLineEdit->text().toDouble());

    // Saving now the externalTrajectory parameters
    // First reading the temporay file
    QString PropagatedLines = "";
    QFile ExternalFile("ephem-external.stae");
    ExternalFile.open(QIODevice::ReadOnly);
    QTextStream StreamWithExternals(&ExternalFile);
    while (!StreamWithExternals.atEnd())
    {
        PropagatedLines = PropagatedLines + StreamWithExternals.readLine() + "\n";      //read in a line
    }
    ExternalFile.close();
    // Saving into the variable
    externalTrajectory->setAllExternalPropagatedLines(PropagatedLines);

    return true;
}



void ExternalDialog::on_ExternalFile_dropped(const QMimeData *mimeData)
{
    QTextStream out (stdout);
    QString PropagatedLines = "";
    QString DroppedExternalFileName;

    QList<QUrl> urlList = mimeData->urls();
    for (int i = 0; i < urlList.size() && i < 32; ++i)
    {
	QString url = urlList.at(i).path();
        DroppedExternalFileName += url;
    }

    if (!DroppedExternalFileName.isEmpty())
    {    
        // Reading the External file
        QFile ExternalFile(DroppedExternalFileName);
        ExternalFile.open(QIODevice::ReadOnly);
        QTextStream StreamWithExternals(&ExternalFile);
        int NumberOfStateVectorsRead = 0;
        while (!StreamWithExternals.atEnd())
        {
            PropagatedLines = PropagatedLines + StreamWithExternals.readLine() + "," + "\n";      //read in a line
            //out << "PropagatedLines: " << PropagatedLines << endl;
            NumberOfStateVectorsRead = NumberOfStateVectorsRead + 1;
        }
        ExternalFile.close();

        // Spliting now the big string into a list of strings using the comma as separator
        QStringList ListOfItems = PropagatedLines.split(",");
        //int NumberOfItems = ListOfItems.size();
        //out << "NumberOfItems: " << NumberOfItems << endl;

        /*
        int i;
        for (i=0; i<NumberOfItems; i++)
        {
            out << "Number " << i <<": " << ListOfItems.value(i) << endl;
        }
        */

        if (ListOfItems.size() > 7)  // Atempting to read more than 1 line of state vectors
        {
           // Obtaining the time line. We now that the first item and the last-6 item are the times we are looking for
           StartDateTimeEdit->setDateTime(sta::JdToCalendar(ListOfItems.value(0).toDouble()));
           //out << "StartDateTimeEdit: " << ListOfItems.value(0).toDouble() << endl;
           EndDateTimeEdit->setDateTime(sta::JdToCalendar(ListOfItems.value(NumberOfStateVectorsRead * 7 - 7).toDouble()));
           //out << "EndDateTimeEdit: " << ListOfItems.value(NumberOfStateVectorsRead * 7 - 7).toDouble() << endl;
           // Obtaining the time step
           double TimeStepInJulianDates = ListOfItems.value(NumberOfStateVectorsRead * 7 - 7).toDouble() - ListOfItems.value(0).toDouble();
           //out << "TimeStepInJulianDates: " << TimeStepInJulianDates << endl;
           QString TimeStepString = TimeStepString.setNum(floor(TimeStepInJulianDates*86400/NumberOfStateVectorsRead + 0.5));
           TimeStepLineEdit->setText(TimeStepString);
           //Updating the GUI of the text window now
           ExternalDialog::FileTextEdit->setPlainText(PropagatedLines);
        }
        else
        {
            QMessageBox::information(this, tr("Bar External file"), tr("Not very nice... \nThis file is almost empty"));
        }
    //////////////////////////////////////////
    // Now storing the read file into a temporary file for the 'abstract" trajectory method to read and plot
    QFile ExternalFileToWrite("ephem-external.stae");   // This file if fixed. TODO make it generic
    ExternalFileToWrite.open(QIODevice::WriteOnly);
    QTextStream StreamWithExternalsToWrite(&ExternalFileToWrite);
    // Output values at full double precision
    StreamWithExternalsToWrite.setRealNumberPrecision(16);

    // Write all samples
    for (int i = 0; i < NumberOfStateVectorsRead; i = i + 7)
    {
        StreamWithExternalsToWrite  << ListOfItems.value(i+0).toDouble()  << ", "
                                    << ListOfItems.value(i+1).toDouble()  << ", "
                                    << ListOfItems.value(i+2).toDouble()  << ", "
                                    << ListOfItems.value(i+3).toDouble()  << ", "
                                    << ListOfItems.value(i+4).toDouble()  << ", "
                                    << ListOfItems.value(i+5).toDouble()  << ", "
                                    << ListOfItems.value(i+6).toDouble()  << endl;
    }

    ExternalFileToWrite.close();

    }
}



// This creates a generic drop area
ExternalDropArea::ExternalDropArea(QWidget *parent)
    : QLabel(parent)
{
    //setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setFrameStyle(QFrame::Sunken);
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
    setAutoFillBackground(true);
    clear();
}



void ExternalDropArea::dragEnterEvent(QDragEnterEvent *event)
{
    setText(tr("drop Ext."));
    setBackgroundRole(QPalette::Highlight);

    event->acceptProposedAction();
    emit changed(event->mimeData());
}




void ExternalDropArea::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}


// What to do when dropping a file into the drop area
void ExternalDropArea::dropEvent(QDropEvent *event)
{
    QTextStream out (stdout);
    QString DroppedExternalFileName, ExternalFileName;

    const QMimeData *mimeData = event->mimeData();

    QList<QUrl> urlList = mimeData->urls();
    for (int i = 0; i < urlList.size() && i < 32; ++i)
    {
	QString url = urlList.at(i).path();
        DroppedExternalFileName += url;
    }


    // Strip away the path--we just want the filename
    if (DroppedExternalFileName.contains('/'))
    {
        ExternalFileName = DroppedExternalFileName.remove(0, DroppedExternalFileName.lastIndexOf('/') + 1);
    };
    setText(ExternalFileName);

    setBackgroundRole(QPalette::Dark);
    event->acceptProposedAction();
}



void ExternalDropArea::dragLeaveEvent(QDragLeaveEvent *event)
{
    clear();
    event->accept();
}



void ExternalDropArea::clear()
{
    setText(tr("drop Ext."));
    setBackgroundRole(QPalette::Dark);
    emit changed();
}

