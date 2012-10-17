/**
  * (C) LGPL-3
  *
  * Voronoi++ <https://github.com/rlux/voronoi>
  *
  * Copyright: 2012 Lux, Scheibel
  * Authors:
  *     Roland Lux <rollux2000@googlemail.com>
  *     Willy Scheibel <willyscheibel@gmx.de>
  * 
  * This file is part of Voronoi++.
  *
  * Voronoi++ is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  * 
  * Voronoi++ is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  * 
  * You should have received a copy of the GNU Lesser General Public License
  * along with Voronoi++.  If not, see <http://www.gnu.org/licenses/>.
  **/

#include <geometry/Line.h>
#include <algorithm>

using namespace geometry;

Line::Line() : _type(NULL_LINE)
{
}

Line::Line(Type type, const Point& start, const Point& end, const Point& direction) : _type(type), _startPoint(start), _endPoint(end), _direction(direction)
{	
}

Line Line::segment(const Point& point1, const Point& point2) {
	return Line(SEGMENT, point1, point2, point2-point1);
}

Line Line::ray(const Point& supportVector, const Point& direction) {
	return Line(RAY, supportVector, supportVector+direction, direction);
}

Line Line::forDirection(const Point& supportVector, const Point& direction)
{
	return Line(LINE, supportVector, supportVector+direction, direction);
}

Line Line::forNormal(const Point& supportVector, const Point& normal)
{
	return Line::forDirection(supportVector, normal.perpendicular());
}

Point Line::supportVector() const
{
	return _startPoint;
}

Point Line::startPoint() const
{
	return _startPoint;
}

Point Line::direction() const
{
	return _direction;
}

Point Line::endPoint() const
{
	return _endPoint;
}

Line::Type Line::type() const
{
	return _type;
}

void Line::setStartPoint(const Point& point)
{
	_startPoint = point;
	_direction = _endPoint - _startPoint;
}

void Line::setEndPoint(const Point& point)
{
	_endPoint = point;
	_direction = _endPoint - _startPoint;
}

void Line::setDirection(const Point& direction)
{
	_direction = direction;
	_endPoint = _startPoint + _direction;
}

bool Line::isNull() const
{
	return _type==NULL_LINE;
}

bool Line::isLine() const
{
	return _type==LINE;
}

bool Line::isRay() const
{
	return _type==RAY;
}

bool Line::isSegment() const
{
	return _type==SEGMENT;
}

Line Line::asLine() const
{
	return Line::forDirection(_startPoint, _direction);
}

void Line::invertDirection()
{
	_direction *= -1;
	if (isRay()) {
		_endPoint = _startPoint+_direction;
	} else {
		std::swap(_startPoint, _endPoint);
	}
}

bool Line::addPoint(const Point& point)
{
	switch (_type) {
		case LINE:
			_startPoint = point;
			_type = RAY;
			return true;
		case RAY:
			_endPoint = point;
			_direction = _endPoint-_startPoint;
			_type = SEGMENT;
			return true;
		default:
			return false;
	}
}

const LineIntersectionSolutionSet Line::intersection(const Line& line) const
{
	real s;
	real t;
	if (!intersectionCoefficient(line, s) || !line.intersectionCoefficient(*this, t)) {
		//check for infinite solutions: w parallel to u
		//extract vector class, parallelTo()
		return LineIntersectionSolutionSet::noSolution();
	}
	
	if (!containsCoefficient(s) || !line.containsCoefficient(t)) {
		return LineIntersectionSolutionSet::noSolution();
	}
	
	return LineIntersectionSolutionSet(_startPoint+s*_direction);
}

LineIntersectionSolutionSet Line::lineIntersection(const Line& line) const
{
	real s;
	if (!intersectionCoefficient(line, s)) {
		//check for infinite solutions: w parallel to u
		//extract vector class, parallelTo()
		return LineIntersectionSolutionSet::noSolution();
	}
	
	return LineIntersectionSolutionSet(_startPoint+s*_direction);
}

bool Line::intersectionCoefficient(const Line& line, real& coefficient) const
{
	Point u = _direction;
	Point v = line._direction;
	Point w = _startPoint-line._startPoint;
	real denominator = v.x()*u.y()-v.y()*u.x();
	if (denominator==0) {
		return false;
	}
	coefficient= (v.y()*w.x()-v.x()*w.y())/denominator;
	return true;
}

bool Line::containsCoefficient(real coefficient) const
{
	switch (_type) {
		case LINE:
			return true;
		case RAY:
			return coefficient>=0;
		case SEGMENT:
			return coefficient>=0 && coefficient<=1;
		default:
			return false;
	}
}

Point Line::normal() const
{
	return _direction.perpendicular();
}

Point Line::toPoint(const Point& point) const
{
	return point-lineIntersection(Line::forDirection(point, normal())).point();
}

bool Line::sameSide(const Point& p1, const Point& p2) const
{
	return toPoint(p1).dotProduct(toPoint(p2))>0;
}


LineIntersectionSolutionSet::LineIntersectionSolutionSet() : _type(NO_SOLUTION)
{
}

LineIntersectionSolutionSet::LineIntersectionSolutionSet(const Point& point) : _point(point), _type(ONE_SOLUTION)
{
}

LineIntersectionSolutionSet LineIntersectionSolutionSet::noSolution()
{
	return LineIntersectionSolutionSet();
}

LineIntersectionSolutionSet LineIntersectionSolutionSet::infiniteSolutions()
{
	LineIntersectionSolutionSet solutionSet;
	solutionSet._type = INFINITE_SOLUTIONS;
	return solutionSet;
}

bool LineIntersectionSolutionSet::isEmpty() const
{
	return _type==NO_SOLUTION;
}

bool LineIntersectionSolutionSet::isOne() const
{
	return _type==ONE_SOLUTION;
}

bool LineIntersectionSolutionSet::isInfinite() const
{
	return _type==INFINITE_SOLUTIONS;
}

const LineIntersectionSolutionSet::Type& LineIntersectionSolutionSet::type() const
{
	return _type;
}

const Point& LineIntersectionSolutionSet::point() const
{
	return _point;
}

