
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

 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

//#include "ascendingNode.h"
#include "threebodyParametersComputation.h"
#include "stamath.h"
#include <QDebug>

void ascendingNodeAngle (const StaBody* firstbody, const StaBody* secondbody, double JD, double &nodeJD, double &ascNodeAng)
{
    //const StaBody* sun = STA_SOLAR_SYSTEM->sun();
    //const StaBody* Planet = STA_SOLAR_SYSTEM->lookup(secondbody->name());
    double step=secondbody->orbitalPeriod()/100;
    JD=JD-secondbody->orbitalPeriod();
    Eigen::Vector3d positionPlanet;

    if (firstbody->id()==10)
        positionPlanet = secondbody->stateVector(JD, STA_SOLAR_SYSTEM->sun(), sta::COORDSYS_EME_J2000).position;
    else if (firstbody->id()==399)
        positionPlanet = secondbody->stateVector(JD, STA_SOLAR_SYSTEM->earth(), sta::COORDSYS_EME_J2000).position;
    else
    {
        Eigen::Vector3d positionFirstPlanet = firstbody->stateVector(JD, STA_SOLAR_SYSTEM->sun(), sta::COORDSYS_EME_J2000).position;
        positionPlanet = secondbody->stateVector(JD, STA_SOLAR_SYSTEM->sun(), sta::COORDSYS_EME_J2000).position;
        positionPlanet=positionPlanet-positionFirstPlanet;
    }

    double flagSign;
    if (positionPlanet.z()>0)
        flagSign=1;
    else if (positionPlanet.z()<0)
        flagSign=2;

    while (fabs(positionPlanet.z())>secondbody->distance()/1000000.0)
    {
        JD=JD+step;
        if (firstbody->id()==10)
            positionPlanet = secondbody->stateVector(JD, STA_SOLAR_SYSTEM->sun(), sta::COORDSYS_EME_J2000).position;
        else if (firstbody->id()==399)
            positionPlanet = secondbody->stateVector(JD, STA_SOLAR_SYSTEM->earth(), sta::COORDSYS_EME_J2000).position;
        else
        {
            Eigen::Vector3d positionFirstPlanet = firstbody->stateVector(JD, STA_SOLAR_SYSTEM->sun(), sta::COORDSYS_EME_J2000).position;
            positionPlanet = secondbody->stateVector(JD, STA_SOLAR_SYSTEM->sun(), sta::COORDSYS_EME_J2000).position;
            positionPlanet=positionPlanet-positionFirstPlanet;
        }

        if (positionPlanet.z()>0 && flagSign==2)
        {
            JD=JD-step;
            step=step/2;
        }
        else if (positionPlanet.z()<0 && flagSign==1)
        {
            flagSign=2;
        }
    }
    //qDebug()<<JD<<positionPlanet.z();
    ascNodeAng=atan2(positionPlanet.y(),positionPlanet.x());
    //qDebug()<<ascNodeAng;
    nodeJD=JD;
}

double trueAnomalyFromAscNode (const StaBody* firstbody, const StaBody* secondbody, double JD, double nodeJD, double ascNodeAng)
{
    double beta;
    Eigen::Vector3d positionPlanet,positionNode;

    if (firstbody->id()==10)
    {
        positionPlanet = secondbody->stateVector(JD, STA_SOLAR_SYSTEM->sun(), sta::COORDSYS_EME_J2000).position;
        positionNode = secondbody->stateVector(nodeJD, STA_SOLAR_SYSTEM->sun(), sta::COORDSYS_EME_J2000).position;
    }
    else if (firstbody->id()==399)
    {
        positionPlanet = secondbody->stateVector(JD, STA_SOLAR_SYSTEM->earth(), sta::COORDSYS_EME_J2000).position;
        positionNode = secondbody->stateVector(nodeJD, STA_SOLAR_SYSTEM->earth(), sta::COORDSYS_EME_J2000).position;
    }
    else
    {
        Eigen::Vector3d positionFirstPlanet = firstbody->stateVector(JD, STA_SOLAR_SYSTEM->sun(), sta::COORDSYS_EME_J2000).position;
        positionPlanet = secondbody->stateVector(JD, STA_SOLAR_SYSTEM->sun(), sta::COORDSYS_EME_J2000).position;
        positionPlanet = positionPlanet-positionFirstPlanet;
        positionNode = secondbody->stateVector(JD, STA_SOLAR_SYSTEM->sun(), sta::COORDSYS_EME_J2000).position;
        positionNode = positionNode-positionFirstPlanet;
    }
    double cosbeta = ((positionNode.dot(positionPlanet))/positionNode.norm()/positionPlanet.norm());
    double sinbeta = ((positionNode.cross(positionPlanet)).norm()/positionNode.norm()/positionPlanet.norm());
    double z=positionNode.cross(positionPlanet).z();
    beta=acos(fabs(cosbeta));
    if (cosbeta>0 && z<0)
        beta=-beta;
    else if (cosbeta<0 && z>0)
        beta=sta::Pi()-beta;
    else if (cosbeta<0 && z<0)
        beta=sta::Pi()+beta;
    
    //qDebug()<<z<<sta::radToDeg(beta);
        //qDebug()<<"betas"<<sta::radToDeg(beta1)<<sta::radToDeg(beta2);
    return beta;
}
