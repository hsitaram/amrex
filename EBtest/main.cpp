#include <AMReX.H>
#include <AMReX_EB2.H>
#include <AMReX_EB2_IF.H>
#include <AMReX_ParmParse.H>
#include <AMReX_PlotFileUtil.H>

using namespace amrex;
amrex::STLtools amrex::EB2::stlhelper;
int main (int argc, char* argv[])
{
    amrex::Initialize(argc, argv);

    {
        Vector<Real> plo;
        Vector<Real> phi;
        Vector<int> ncells;
        Vector<Real> pointoutside;
        int max_grid_size = 32;
        std::string stl_fname;

        ParmParse pp;
        pp.getarr("prob_lo",plo);
        pp.getarr("prob_hi",phi);
        pp.getarr("ncells",ncells);
        pp.get("stl_file",stl_fname);
        pp.query("max_grid_size",max_grid_size);
        pp.getarr("outside_point",pointoutside);

        RealBox real_box({AMREX_D_DECL(plo[0], plo[1], plo[2])},
                {AMREX_D_DECL(phi[0], phi[1], phi[2])});

        Array<int,AMREX_SPACEDIM> is_periodic{AMREX_D_DECL(0,0,0)};

        IntVect domain_lo(AMREX_D_DECL(0,0,0));
        IntVect domain_hi(AMREX_D_DECL(ncells[0]-1,ncells[1]-1,ncells[2]-1));

        Box domain(domain_lo, domain_hi);
        BoxArray ba(domain);
        ba.maxSize(max_grid_size);

        Geometry geom(domain,real_box,CoordSys::cartesian,is_periodic);
        DistributionMapping dm(ba);

        amrex::Print()<<"Using STL\n";
        
        bool has_fluid_inside=false;
        EB2::STLIF stlif(stl_fname, has_fluid_inside,
                pointoutside[0],pointoutside[1],pointoutside[2]);

        EB2::GeometryShop<EB2::STLIF> gshop(stlif);
        EB2::Build(gshop, geom, 0, 0);


        MultiFab mf;
        std::unique_ptr<EBFArrayBoxFactory> factory
            = amrex::makeEBFabFactory(geom, ba, dm, {2,2,2}, EBSupport::full);
        mf.define(ba, dm, 1, 0, MFInfo(), *factory);
        mf.setVal(1.0);

        EB_WriteSingleLevelPlotfile("plt", mf, {"rho"}, geom, 0.0, 0);
    }

    amrex::Finalize();
}
