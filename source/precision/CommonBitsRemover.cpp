/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2005-2006 Refractions Research Inc.
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/precision/CommonBitsRemover.h>
#include <geos/precision/CommonBits.h>
// for CommonCoordinateFilter inheritance
#include <geos/geom/CoordinateFilter.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Geometry.h>

#include <cassert>

using namespace geos::geom;

namespace geos {
namespace precision { // geos.precision

class Translater: public geom::CoordinateFilter {

private:

	geom::Coordinate trans;

public:

	Translater(geom::Coordinate &newTrans)
		:
		trans(newTrans)
	{}

	void filter_ro(const geom::Coordinate *coord){}; //Not used

	void filter_rw(geom::Coordinate *coord) const
	{
		coord->x += trans.x;
		coord->y += trans.y;
	}
};


class CommonCoordinateFilter: public geom::CoordinateFilter {
private:
	CommonBits commonBitsX;
	CommonBits commonBitsY;
public:

	void filter_rw(geom::Coordinate *coord) const
	{
		// CommonCoordinateFilter is a read-only filter
		assert(0);
	}

	void filter_ro(const geom::Coordinate *coord)
	{
		commonBitsX.add(coord->x);
		commonBitsY.add(coord->y);
	}

	void getCommonCoordinate(geom::Coordinate& c)
	{
		c=Coordinate(commonBitsX.getCommon(),
			commonBitsY.getCommon());
	}

};


CommonBitsRemover::CommonBitsRemover()
{
	ccFilter=new CommonCoordinateFilter();
}

CommonBitsRemover::~CommonBitsRemover()
{
	delete ccFilter;
}

/**
 * Add a geometry to the set of geometries whose common bits are
 * being computed.  After this method has executed the
 * common coordinate reflects the common bits of all added
 * geometries.
 *
 * @param geom a Geometry to test for common bits
 */
void
CommonBitsRemover::add(Geometry *geom)
{
	geom->apply_rw(ccFilter);
	ccFilter->getCommonCoordinate(commonCoord);
}

/**
 * The common bits of the Coordinates in the supplied Geometries.
 */
Coordinate&
CommonBitsRemover::getCommonCoordinate()
{ 
	return commonCoord;
}

/**
 * Removes the common coordinate bits from a Geometry.
 * The coordinates of the Geometry are changed.
 *
 * @param geom the Geometry from which to remove the common coordinate bits
 * @return the shifted Geometry
 */
Geometry*
CommonBitsRemover::removeCommonBits(Geometry *geom)
{
	if (commonCoord.x == 0.0 && commonCoord.y == 0.0)
		return geom;
	Coordinate *invCoord=new Coordinate(commonCoord.x,commonCoord.y);
	invCoord->x=-invCoord->x;
	invCoord->y=-invCoord->y;
	Translater *trans=new Translater(*invCoord);
	delete invCoord;
	geom->apply_rw(trans);
	geom->geometryChanged();
	delete trans;
	return geom;
}

/**
 * Adds the common coordinate bits back into a Geometry.
 * The coordinates of the Geometry are changed.
 *
 * @param geom the Geometry to which to add the common coordinate bits
 * @return the shifted Geometry
 */
void
CommonBitsRemover::addCommonBits(Geometry *geom)
{
	Translater *trans=new Translater(commonCoord);
	geom->apply_rw(trans);
	geom->geometryChanged();
	delete trans;
}

} // namespace geos.precision
} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.7  2006/03/23 09:17:19  strk
 * precision.h header split, minor optimizations
 *
 * Revision 1.6  2006/03/06 19:40:47  strk
 * geos::util namespace. New GeometryCollection::iterator interface, many cleanups.
 *
 * Revision 1.5  2006/03/02 16:21:26  strk
 * geos::precision namespace added
 *
 * Revision 1.4  2006/02/09 15:52:47  strk
 * GEOSException derived from std::exception; always thrown and cought by const ref.
 *
 * Revision 1.3  2005/12/08 14:14:07  strk
 * ElevationMatrixFilter used for both elevation and Matrix fill,
 * thus removing CoordinateSequence copy in ElevetaionMatrix::add(Geometry *).
 * Changed CoordinateFilter::filter_rw to be a const method: updated
 * all apply_rw() methods to take a const CoordinateFilter.
 *
 * Revision 1.2  2004/07/02 13:28:29  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.1  2004/04/10 22:41:25  ybychkov
 * "precision" upgraded to JTS 1.4
 *
 *
 **********************************************************************/

