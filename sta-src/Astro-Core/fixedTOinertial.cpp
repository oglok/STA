/*

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */


/*
 --- Copyright (C) 2009 STA Steering Board (space.trajectory.analysis AT gmail.com) ---
*/

/*
 ------------------ Author: Tiziana Sabatini  -------------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ----------------------------
 */

#include "fixedTOinertial.h"
#include<float.h>
#include<math.h>

void fixedTOinertial(int body, double GreenwichAngle,
        double xFixed, double yFixed, double zFixed, double xdFixed, double ydFixed, double zdFixed,
        double& xInertial, double& yInertial, double& zInertial, double& xdInertial, double& ydInertial, double& zdInertial)
{
    double rgha, cosgha, singha, term1, term2, omega;

   if(body==0)
      omega = 7.29211585494e-5;  /// Earth rotation rate. Only Earth for the time being
   if(body==3)
      omega = 7.088216383964474e-5;

    rgha = GreenwichAngle;
    cosgha = cos(rgha);
    singha = sin(rgha);

//  Position rotation
    xInertial = xFixed*cosgha - yFixed*singha;
    yInertial = xFixed*singha + yFixed*cosgha;
    zInertial = zFixed;

//  Velocity rotation
    term1 = xdFixed*cosgha - ydFixed*singha;
    term2 = -omega*yInertial;
    xdInertial = term1+term2;
    term1 = xdFixed*singha + ydFixed*cosgha;
    term2 = omega*xInertial;
    ydInertial = term1+term2;
    zdInertial = zdFixed;

}
