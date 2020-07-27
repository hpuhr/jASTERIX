#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np
import math
import nvector as nv

class GeoPosition(object):
    wgs84 = nv.FrameE(name='WGS84')

    def __init__(self):
        self.__ecef_pos = None # type: nv.ECEFvector
        self.__geopoint = None # type: nv.GeoPoint

        self.__ned_var = np.matrix([[0, 0, 0], [0, 0, 0], [0, 0, 0]])

        self.pos_usable = False
        self.pos_corrected = False

    @property
    def geopoint(self):
        return self.__geopoint

    @geopoint.setter
    def geopoint(self, point):
        """
        Sets the position to given one.
        
        Parameters
        ----------
        point: nv.GeoPoint
            Contains position to be set

        Returns
        -------
        """

        assert isinstance (point, nv.GeoPoint)

        self.__geopoint = point
        self.__ecef_pos = self.__geopoint.to_ecef_vector()
        self.pos_usable = True

    def geoPosNaN (self):
        assert self.__geopoint
        assert isinstance(self.__geopoint, nv.GeoPoint)

        if np.isscalar(self.__geopoint.latitude_deg):
            return math.isnan(self.__geopoint.latitude_deg) or math.isnan(self.__geopoint.longitude_deg) \
                   or math.isnan(self.__geopoint.z)
        else:
            return math.isnan(self.__geopoint.latitude_deg[0]) or math.isnan(self.__geopoint.longitude_deg[0]) \
                   or math.isnan(-self.__geopoint.z[0])

    def getGeoPos (self):
        """
        
        Returns
        -------
            (float, float, float)
                WGS84 latitude, longitude in degrees, altitude in m (above ellipsoid)
        """

        assert self.__geopoint
        assert isinstance(self.__geopoint, nv.GeoPoint)

        if np.isscalar(self.__geopoint.latitude_deg):
            return self.__geopoint.latitude_deg, self.__geopoint.longitude_deg, -self.__geopoint.z
        else:
            return self.__geopoint.latitude_deg[0], self.__geopoint.longitude_deg[0], -self.__geopoint.z[0]

    def getGeoLatitude(self):
        assert self.__geopoint
        assert isinstance(self.__geopoint, nv.GeoPoint)

        if np.isscalar(self.__geopoint.latitude_deg):
            return self.__geopoint.latitude_deg
        else:
            return self.__geopoint.latitude_deg[0]

    def getGeoLongitude(self):
        assert self.__geopoint
        assert isinstance(self.__geopoint, nv.GeoPoint)

        if np.isscalar(self.__geopoint.latitude_deg):
            return self.__geopoint.longitude_deg
        else:
            return self.__geopoint.longitude_deg[0]

    def setGeoPos(self, latitude, longitude, z=0):
        """
        Sets the position to given WGS84 position.
        
        Parameters
        ----------
        latitude: float
            WGS84 latitude, in degrees
        longitude: float
            WGS84 longitude, in degrees
        z: float
            altitude on WGS84 ellipsoid, in meters above ellipsoid

        Returns
        -------
        """

        self.__geopoint = GeoPosition.wgs84.GeoPoint(latitude, longitude, -z, degrees=True) # -z because of depth
        self.__ecef_pos = self.__geopoint.to_ecef_vector()
        self.pos_usable = True

    def getGeoPosStr (self):
        """
        Returns
        -------
        str
            WGS84 position as string

        """

        assert self.__geopoint
        assert isinstance(self.__geopoint, nv.GeoPoint)

        if np.isscalar(self.__geopoint.latitude_deg):
            return 'latitude {:4.6f} longitude {:4.6f} deg z {:4.2f} m'.format(self.__geopoint.latitude_deg,
                                                                self.__geopoint.longitude_deg,
                                                                -self.__geopoint.z)
        else:
            return 'latitude {:4.6f} longitude {:4.6f} deg z {:4.2f} m'.format(self.__geopoint.latitude_deg[0],
                                                                self.__geopoint.longitude_deg[0],
                                                                -self.__geopoint.z[0])

    def getECEFStr (self):
        """
        Returns
        -------
        str
            ECEF position as string 

        """

        assert self.__ecef_pos
        return 'ecef {} m'.format(self.__ecef_pos.pvector.ravel().tolist())

    @property
    def ecefposition(self):
        return self.__ecef_pos

    @ecefposition.setter
    def ecefposition(self, position):
        """
        Sets the internal position to given ECEF coordinates.
        
        Parameters
        ----------
        position: nv.ECEFvector
            ECEF position to be set.

        Returns
        -------
        """

        assert isinstance (position, nv.ECEFvector)

        self.__ecef_pos = position
        self.__geopoint = self.__ecef_pos.to_geo_point()

    @property
    def nedvariance(self):
        return self.__ned_var

    @nedvariance.setter
    def nedvariance(self, variance):
        assert isinstance (variance, np.matrix)
        assert variance.shape[0] == 3
        assert variance.shape[1] == 3

        self.__ned_var = variance

    def getNEDVarianceStr(self):
        return '{}'.format(self.__ned_var)

    def getNED(self, reference_geo_point):
        """
        Returns north-east-down position, relative to given reference position.
        
        Parameters
        ----------
        reference_geo_point: nv.GeoPoint
            Reference position for NED origin

        Returns
        -------
        (float, float,float)
            north, east, down coordinates
        """

        assert isinstance(reference_geo_point, nv.GeoPoint)

        # calculate distance from rounded geopoint to own
        d_r2g = reference_geo_point.delta_to(self.__geopoint) # type: nv.Pvector

        return d_r2g.pvector.ravel() #n,e,d

    def getENU(self, reference_geo_point):
        """
        Returns east-north-up position, relative to given reference position.

        Parameters
        ----------
        reference_geo_point: nv.GeoPoint
            Reference position for ENU origin

        Returns
        -------
        (float, float,float)
            east, north, up coordinates
        """

        assert isinstance(reference_geo_point, nv.GeoPoint)

        x, y, z = self.getNED(reference_geo_point)

        return y, x, -z

    def setNED(self, n, e, d, reference_geo_point):
        """
        Sets position from north-east-down position, relative to given reference position.
        
        Parameters
        ----------
        n: float
            position north component
        e: float
            position east component
        d: float
            position down component
        reference_geo_point: nv.GeoPoint
            Reference position for ENU origin

        Returns
        -------
        """

        assert isinstance(reference_geo_point, nv.GeoPoint)

        local_frame = nv.FrameN(reference_geo_point)  # North-East-Down frame

        local_p = local_frame.Pvector(np.r_[n, e, d])
        self.__ecef_pos = reference_geo_point.to_ecef_vector() + local_p.to_ecef_vector()
        self.__geopoint = self.__ecef_pos.to_geo_point()

    def setNEDOffset(self, n, e, d):
        """
        Sets position from north-east-down position, relative to the own position.

        Parameters
        ----------
        n: float
            position north component
        e: float
            position east component
        d: float
            position down component
        reference_geo_point: nv.GeoPoint
            Reference position for ENU origin

        Returns
        -------
        """

        local_frame = nv.FrameN(self.__geopoint)  # North-East-Down frame

        local_p = local_frame.Pvector(np.r_[n, e, d])
        self.__ecef_pos = self.__ecef_pos + local_p.to_ecef_vector()
        self.__geopoint = self.__ecef_pos.to_geo_point()

    def getLatLongDelta(self, n, e):
        local_frame = nv.FrameN(self.__geopoint)  # North-East-Down frame
        local_p = local_frame.Pvector(np.r_[n, e, 0])
        ecef_pos = self.__ecef_pos + local_p.to_ecef_vector()
        local_geo = ecef_pos.to_geo_point()  # type: nv.GeoPoint

        return abs(self.__geopoint.latitude_deg-local_geo.latitude_deg)[0], abs(self.__geopoint.longitude_deg-local_geo.longitude_deg)[0]


    def getNEDDelta(self, other_point):
        """
        Returns north-east-down Cartesian delta to other point, measured from this point (self as NED origin).
        
        Parameters
        ----------
        other_point: GeoPosition
         containing other position

        Returns
        -------
        (float,float,float)
            (n,e,d) position delta components
        """
        assert isinstance(other_point, GeoPosition)
        return self.__geopoint.delta_to(other_point.__geopoint)  # type: nv.Pvector

    def getNEDGeoDelta(self, geo_point):
        """
        Returns north-east-down Cartesian delta to other point, measured from this point (self as NED origin).

        Parameters
        ----------
        geo_point: nv.GeoPoint
         containing other position

        Returns
        -------
        (float,float,float)
            (n,e,d) position delta components
        """
        assert isinstance(geo_point, nv.GeoPoint)
        return self.__geopoint.delta_to(geo_point)  # type: nv.Pvector

    def getECEFDelta(self, other_point):
        """
        Returns north-east-down ECEF delta to other point.

        Parameters
        ----------
        other_point: GeoPosition
         containing other position

        Returns
        -------
        nv.ECEFVector
            position delta
        """
        assert isinstance(other_point, GeoPosition)

        return self.__geopoint.delta_to(other_point.__geopoint).to_ecef_vector()  # type: nv.ECEFvector

    def getAltitude (self):
        return -self.__geopoint.z

    def setAltitude (self, z):
        if np.isscalar(self.__geopoint.z):
            self.__geopoint.z = -z
        else:
            self.__geopoint.z[0] = -z
        self.__ecef_pos = self.__geopoint.to_ecef_vector()


    def distance (self, other_point):
        """
        Returns 3D Cartesian distance to other point, measured from this point (self as NED origin). Note that
        the distance is the same as in ECEF.
                
        Parameters
        ----------
        other_point: GeoPosition
            containing other position

        Returns
        -------
        float
            distance in meters
        """

        assert isinstance(other_point, GeoPosition)
        d_r2g = self.__geopoint.delta_to(other_point.__geopoint)  # type: nv.Pvector
        return d_r2g.length[0]

    def distance2D(self, other_point):
        """
        Returns 2D Cartesian distance to other point, measured from this point (self as NED origin). Note that
        the distance is the same as in ECEF.

        Parameters
        ----------
        other_point: GeoPosition
            containing other position

        Returns
        -------
        float
            distance in meters
        """

        assert isinstance(other_point, GeoPosition)
        d_r2g = self.__geopoint.delta_to(other_point.__geopoint)  # type: nv.Pvector
        x,y,z = d_r2g.pvector.ravel()
        return math.sqrt(x**2 + y**2)

    def getNEDAzimuth (self, other_point):
        """
        Returns north-east-down azimuth to other point, measured from north to other (self as NED origin).
        
        Parameters
        ----------
        other_point: GeoPosition
            containing other position

        Returns
        -------
        float
            azimuth in degrees
        """

        assert isinstance(other_point, GeoPosition)
        d_r2g = self.__geopoint.delta_to(other_point.__geopoint)  # type: nv.Pvector
        return nv.deg(d_r2g.azimuth[0])

    def getNEDElevationAngle (self, other_point):
        """
        Returns north-east-down elevation angle to other point, measured from tangential plane to other
        (self as NED origin).
        
        Parameters
        ----------
        other_point: GeoPosition
            containing other position

        Returns
        -------
        float
            elevation angle(pointing up) in degrees
        """
        assert isinstance(other_point, GeoPosition)
        d_r2g = self.__geopoint.delta_to(other_point.__geopoint)  # type: nv.Pvector
        return nv.deg(-d_r2g.elevation[0])

    def setENU(self, e, n, u, reference_geo_point):
        """
        Sets position from east-north-up position, relative to given reference position.
        
        Parameters
        ----------
        e: float
            position east component
        n: float
            position north component
        u: float
            position up component
        reference_geo_point: nv.GeoPoint
            contains reference position

        Returns
        -------
        """

        assert isinstance(reference_geo_point, nv.GeoPoint)

        self.setNED(n, e, -u, reference_geo_point)

    def getPolar (self, reference_geo_point):
        """
        Returns polar position, relative to given reference position.

        Parameters
        ----------
        reference_geo_point: nv.GeoPoint
            Reference position for ENU origin

        Returns
        -------
        (float, float,float)
            distance in meters, azimuth in radians, elevation angle (up) in radians 

        """
        assert isinstance(reference_geo_point, nv.GeoPoint)

        d_r2g = reference_geo_point.delta_to(self.__geopoint)  # type: nv.Pvector

        return d_r2g.length[0], d_r2g.azimuth[0], -d_r2g.elevation[0]

    def setPolar (self, distance, azimuth, elevation_angle, reference_geo_point):
        """
        Sets position from polar position, relative to given reference position.

        Parameters
        ----------
        distance: float
            distance in meters
        azimuth: float
            azimuth in radians
        elevation_angle: float
            elevation angle (up) in radians
        reference_geo_point: nv.GeoPoint
            Reference position for ENU origin

        Returns
        -------

        """
        assert isinstance(reference_geo_point, nv.GeoPoint)

        n = distance * math.cos(azimuth)
        e = distance * math.sin(azimuth)
        d = distance * math.sin(-elevation_angle)

        local_frame = nv.FrameN(reference_geo_point)
        d_rg2 = local_frame.Pvector(np.r_[n, e, d]).to_ecef_vector()

        self.__ecef_pos = reference_geo_point.to_ecef_vector() + d_rg2.to_ecef_vector()
        self.__geopoint = self.__ecef_pos.to_geo_point()

