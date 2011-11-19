/*  
 *  Copyright 2010-2011 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *
 *  Idea and code for point/line/arc voronoi-vertex positioning code by
 *  Andy Payne, andy "at" payne "dot" org, November, 2010
 *  see: http://www.payne.org/index.php/Calculating_Voronoi_Nodes
 * 
 *  This file is part of OpenVoronoi.
 *
 *  OpenVoronoi is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenVoronoi is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OpenVoronoi.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VERTEX_POSITIONER_HPP
#define VERTEX_POSITIONER_HPP

#include <qd/qd_real.h> // http://crd.lbl.gov/~dhbailey/mpdist/

#include "graph.hpp"
#include "vertex.hpp"

namespace ovd {

class VoronoiDiagram;

// a new vd-vertex position solution
// includes the offset-distamce t, and the offset direction k3
struct Solution {
    Solution(Point pt, double tv, double k) : p(pt), t(tv), k3(k) {}
    Point p;
    double t;
    double k3;
};

// predicate for filtering solutions based on t-value being in [tmin,tmax] range
struct t_filter {
    t_filter(double tmin, double tmax): tmin_(tmin),tmax_(tmax) {}
    bool operator()(Solution s) { 
        return (s.t<tmin_) || (s.t>tmax_); 
    }
private:
    double tmin_;
    double tmax_;
};

// predicate for rejecting out-of-region solutions
struct in_region_filter {
    in_region_filter(Site* s): site_(s) {}
    bool operator()(Solution s) { 
        return !site_->in_region(s.p); 
    }
private:
    Site* site_;
};

class Solver {
public:
    virtual Solution solve(Site* s1, double k1, 
                           Site* s2, double k2, 
                           Site* s3, double k3) = 0;
};

/// Calculates the (x,y) position of vertices in a voronoi diagram
class VertexPositioner {
public:
    VertexPositioner(VoronoiDiagram* vodi);
    virtual ~VertexPositioner();
    /// calculate the position of a new voronoi-vertex lying on the given edge.
    /// The new vertex is equidistant to the two sites that defined the edge
    /// and to the new site. 
    Point position( HEEdge e, Site* s);
    double get_k3() const {return k3;}
    std::vector<double> get_stat() {return errstat;}
private:
    Solution position(Site* p1, double k1, Site* p2, double k2, Site* p3);
    
    int solver(Site* s1, double k1, Site* s2, double k2, Site* s3, double k3, std::vector<Solution>& slns ); 
    
    int lll_solver(std::vector< Eq<qd_real> >& eqns, double k3, std::vector<Solution>& slns ); // linear 3x3 system
    
    int qll_solver( const std::vector< Eq<qd_real> >& lins, int xi, int yi, int ti, 
                    const Eq<qd_real>& quad, qd_real k3, std::vector<Solution>& slns );
    
    int qll_solve( qd_real a0, qd_real b0, qd_real c0, qd_real d0, 
                      qd_real e0, qd_real f0, qd_real g0, 
                      qd_real a1, qd_real b1, 
                      qd_real a2, qd_real b2, 
                      qd_real soln[][3]);

// geometry-checks
    double edge_error(HEEdge e, Solution& s );
    bool solution_on_edge(Solution& s);
    bool check_far_circle(const Point& p);
    bool check_dist(HEEdge e, const Solution& s, Site* s3);
    bool equal(double d1, double d2);
  
    Solver* ppp_solver;
// DATA
    VoronoiDiagram* vd;
    double t_min;
    double t_max;
    double k3; // the offset-direction to the new site
    HEEdge edge;
    std::vector<double> errstat;
};


}
#endif
