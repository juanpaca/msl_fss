###############################################################################
#
# Compilation and configuration defaults for the msl_fss product.
#
# The dummy implementation is standalone: it defines the internal module
# structure (Control, Subproblems, Infrastructure, Types) and uses thin
# stand-in facades for the real MSL-MFEM/MSL-MHM libraries. Therefore the
# targets below do not require MFEM, MUMPS, LUA or METIS to compile.
#
###############################################################################

# In the real product the MSL sibling projects are consumed via
# add_subdirectory (as the other msl_* projects do). They are declared here
# for documentation only and are NOT added when the dummy build is used.
set (MSL_MFEM_PATH ../msl_mfem)
set (MSL_MHM_PATH ../msl_mhm)
set (MSL_CG_PATH ../msl_cg)
set (MSL_CORE_PATH ../msl_core)

# Intended executable targets (one per compiled strategy).
set (INTENDED_TARGETS msl_fss_galerkin msl_fss_mhm)

# Whether to consume the real MSL stack (not implemented in the dummy build).
set (DUMMY_ONLY ON)
