/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 * Copyright (C) 2005 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/geomgraphindex.h>

namespace geos {

SimpleSweepLineIntersector::SimpleSweepLineIntersector():
	//events(new vector<SweepLineEvent*>()),
	nOverlaps(0)
{
}

SimpleSweepLineIntersector::~SimpleSweepLineIntersector()
{
	for(unsigned int i=0; i<events.size(); ++i)
		delete events[i];
	//delete events;
}

void
SimpleSweepLineIntersector::computeIntersections(vector<Edge*> *edges,
	SegmentIntersector *si, bool testAllSegments)
{
	if (testAllSegments)
		add(edges,NULL);
	else
		add(edges);
	computeIntersections(si);
}

void
SimpleSweepLineIntersector::computeIntersections(vector<Edge*> *edges0,vector<Edge*> *edges1,SegmentIntersector *si)
{
	add(edges0,edges0);
	add(edges1,edges1);
	computeIntersections(si);
}

void
SimpleSweepLineIntersector::add(vector<Edge*> *edges)
{
	for (unsigned int i=0; i<edges->size(); ++i)
	{
		Edge *edge=(*edges)[i];
		// edge is its own group
		add(edge,edge);
	}
}

void
SimpleSweepLineIntersector::add(vector<Edge*> *edges, void* edgeSet)
{
	for (unsigned int i=0; i<edges->size(); ++i)
	{
		Edge *edge=(*edges)[i];
		add(edge,edgeSet);
	}
}

void
SimpleSweepLineIntersector::add(Edge *edge, void* edgeSet)
{
	const CoordinateSequence *pts=edge->getCoordinates();
	int n=pts->getSize()-1;
	for(int i=0; i<n; ++i)
	{
		SweepLineSegment *ss=new SweepLineSegment(edge, i);
		SweepLineEvent *insertEvent=new SweepLineEvent(edgeSet, ss->getMinX(), NULL, ss);
		events.push_back(insertEvent);
		events.push_back(new SweepLineEvent(edgeSet, ss->getMaxX(), insertEvent, ss));
	}
}

/**
 * Because Delete Events have a link to their corresponding Insert event,
 * it is possible to compute exactly the range of events which must be
 * compared to a given Insert event object.
 */
void
SimpleSweepLineIntersector::prepareEvents()
{
	sort(events.begin(), events.end(), SweepLineEventLessThen());
	for(unsigned int i=0; i<events.size(); ++i )
	{
		SweepLineEvent *ev=events[i];
		if (ev->isDelete())
		{
			ev->getInsertEvent()->setDeleteEventIndex(i);
		}
	}
}

void
SimpleSweepLineIntersector::computeIntersections(SegmentIntersector *si)
{
	nOverlaps=0;
	prepareEvents();
	for(unsigned int i=0; i<events.size(); ++i)
	{
		SweepLineEvent *ev=events[i];
		if (ev->isInsert())
		{
			processOverlaps(i,ev->getDeleteEventIndex(),ev,si);
		}
	}
}

void
SimpleSweepLineIntersector::processOverlaps(int start,int end,SweepLineEvent *ev0,
	SegmentIntersector *si)
{

	SweepLineSegment *ss0=(SweepLineSegment*) ev0->getObject();

	/**
	 * Since we might need to test for self-intersections,
	 * include current insert event object in list of event objects to test.
	 * Last index can be skipped, because it must be a Delete event.
 	 */
	for(int i=start; i<end; ++i)
	{
		SweepLineEvent *ev1=events[i];
		if (ev1->isInsert())
		{
			SweepLineSegment *ss1=(SweepLineSegment*) ev1->getObject();
			if (ev0->edgeSet==NULL || (ev0->edgeSet!=ev1->edgeSet))
			{
				ss0->computeIntersections(ss1,si);
				nOverlaps++;
			}
		}
	}
}

} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.6  2005/11/15 10:04:37  strk
 * Reduced heap allocations (vectors, mostly).
 * Enforced const-correctness, changed some interfaces
 * to use references rather then pointers when appropriate.
 *
 * Revision 1.5  2005/11/03 21:28:06  strk
 * Fixed constructors broke by previous commit
 *
 * Revision 1.4  2005/11/03 19:51:28  strk
 * Indentation changes, small vector memory allocation optimization.
 *
 * Revision 1.3  2004/07/08 19:34:49  strk
 * Mirrored JTS interface of CoordinateSequence, factory and
 * default implementations.
 * Added DefaultCoordinateSequenceFactory::instance() function.
 *
 * Revision 1.2  2004/07/02 13:28:27  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.1  2004/04/14 06:04:26  ybychkov
 * "geomgraph/index" committ problem fixed.
 *
 * Revision 1.7  2004/03/19 09:49:29  ybychkov
 * "geomgraph" and "geomgraph/indexl" upgraded to JTS 1.4
 *
 * Revision 1.6  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 * Revision 1.5  2003/10/15 16:39:03  strk
 * Made Edge::getCoordinates() return a 'const' value. Adapted code set.
 *
 **********************************************************************/

