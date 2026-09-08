# `MSL_FSS`: Fixed-Stress Split poromechanical solver (dummy prototype)

This repository holds the **dummy implementation** of the `msl_fss` module
structure specified in the *Projeto Detalhado* (Segundo Trabalho, disciplina
GA-031). The dummy implementation is **standalone**: it compiles and its unit
and integration tests pass **without** requiring MFEM, MUMPS, LUA, METIS or
any of the MSL sibling libraries.

The module structure mirrors the detailed project:

```
include/msl_fss/
  Types/            Strategy, FSSStatus, Field (dummy), Config (dummy)
  Control/          PoromechanicsMFEMProblem<F,M> (facade), FixedStressController<F,M>
  Subproblems/      ISubproblemSolver, SubproblemSolver<Physics,Resolver,Self> (CRTP)
                    FlowPhysics, MechanicsPhysics
  Infrastructure/   GalerkinSolver, MultiscaleSolver, CouplingOperator,
                    SharedMHMHierarchy, OfflineStore, LinearSystem, PartitionInfo
  External/         PhysicalFacade stand-in for the real MSL-MFEM facades
src/                dummy implementations (Types, Control, Infrastructure)
examples/           main_fss_galerkin, main_fss_mhm (+ dummy config .ini)
tests/              Catch2 unit and integration tests
```

The *physical axis* classes (`FlowPhysics`, `MechanicsPhysics`) wrap the
`external::PhysicalFacade` stand-in, which in the real product is the
`msl_mfem` facade (`HeatMFEMProblem` for the transient flow,
`ElasticityMFEMProblem` for the stationary mechanics). The *resolvedor axis*
(`GalerkinSolver`, `MultiscaleSolver`) mirrors the compile-time selection of
strategy materialized by the two entry points. The `SubproblemSolver<Physics,
Resolver, Self>` template uses CRTP static dispatch, the same device used by
`MFEMProblem<Derived>` in the MSL stack.

The entry points make the time loop and the FSS loop explicit. Each time step
calls `BeginTimeStep(dt)` and repeatedly calls `RunFixedStressIteration()` until
it returns `CONVERGED`, `DIVERGED`, or the main reaches `kMax`. The
`PoromechanicsMFEMProblem<FlowSub, MechSub>::Run` facade keeps a convenience
wrapper for clients that do not need the loops exposed. The dummy uses a
discrete H1 norm (coefficient plus first-difference terms) for convergence.

## How to compile

```
mkdir build_debug && cd build_debug
cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS=ON ..
make -j$(nproc)
```

## How to run

Run the two dummy entry points (from `build_debug`):

```
examples/main_fss_mhm      ../examples/config_fss_mhm.ini
examples/main_fss_galerkin ../examples/config_fss_galerkin.ini
```

Run the unit and integration tests:

```
tests/tests_fss -s
```

## Relationship with the real MSL stack

This package is a *prototype*: the real product consumes the MSL-MFEM/MSL-MHM
libraries via `add_subdirectory`, and the stand-ins under `include/msl_fss/External/`
are replaced by the real facades. The dependency chain of the actual product is:

```
msl_fss -> msl_mfem -> msl_mhm -> msl_cg -> msl_core
```

The parallelism is **MPI only (one process per core)**; hybrid MPI+OpenMP is
discouraged by the MSL-MFEM maintainers (see the `msl_mfem` README).
