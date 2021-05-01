#ifndef AMREX_EB2_IF_STL_H_
#define AMREX_EB2_IF_STL_H_
#include <AMReX_Config.H>

#include <AMReX_Array.H>
#include <AMReX_EB2_IF_Base.H>
#include <AMReX_EB_STL_utils.H>

// For all implicit functions, >0: body; =0: boundary; <0: fluid

namespace amrex { namespace EB2 {

extern STLtools stlhelper;

class STLIF
    : public GPUable
{
public:

    //STLtools m_stlhelper;
    // inside: is the fluid inside the sphere?
    //std::list<Triangle> m_triangles;
    //Tree *m_aabb_tree;
    STLIF (std::string fname,bool a_inside,Real outpx,Real outpy,Real outpz)
    {
      m_sign=a_inside ? 1.0 : -1.0;
      stlhelper.read_ascii_stl_file(fname,outpx,outpy,outpz);  
    }

    STLIF (const STLIF& rhs) noexcept = default;
    STLIF (STLIF&& rhs) noexcept = default;
    STLIF& operator= (const STLIF& rhs) = delete;
    STLIF& operator= (STLIF&& rhs) = delete;

    AMREX_GPU_HOST_DEVICE inline
    Real operator() (AMREX_D_DECL(Real x, Real y, Real z)) const noexcept 
    {
        //Real dist=1.0;
        Real dist=stlhelper.getSignedDistance(x,y,z);
        return(dist*m_sign);
    }

    inline Real operator() (const RealArray& p) const noexcept {
        return this->operator()(AMREX_D_DECL(p[0],p[1],p[2]));
    }

protected:
    //
    Real  m_sign;
};

}}

#endif
