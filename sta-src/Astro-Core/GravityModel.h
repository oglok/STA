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
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#ifndef _ASTROCORE_GRAVITYMODEL_H_
#define _ASTROCORE_GRAVITYMODEL_H_

#include <Eigen/Core>

namespace sta
{

class GravityModel
{
    public:
        GravityModel(double mu);
        GravityModel(double mu, const Eigen::VectorXd& zonals);
        ~GravityModel();

        double mu() const
        {
            return m_mu;
        }

        /** Convenience function for accessing low-order zonal harmonic
          * coefficient.
          */
        double J2() const
        {
            if (m_zonalHarmonics.size() > 0)
                return m_zonalHarmonics[0];
            else
                return 0.0;
        }

        /** Convenience function for accessing low-order zonal harmonic
          * coefficient.
          */
        double J3() const
        {
            if (m_zonalHarmonics.size() > 1)
                return m_zonalHarmonics[1];
            else
                return 0.0;
        }

        /** Convenience function for accessing low-order zonal harmonic
          * coefficient.
          */
        double J4() const
        {
            if (m_zonalHarmonics.size() > 2)
                return m_zonalHarmonics[2];
            else
                return 0.0;
        }

        const Eigen::VectorXd& zonals() const
        {
            return m_zonalHarmonics;
        }

        void setZonals(const Eigen::VectorXd& zonals);

    private:
        double m_mu;
        Eigen::VectorXd m_zonalHarmonics;
};

}

#endif // _ASTROCORE_GRAVITYMODEL_H_
