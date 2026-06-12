#include "KokkosController.hpp"
#include "MeshField.hpp"
#include "MeshField_Element.hpp"    //remove?
#include "MeshField_Fail.hpp"       //remove?
#include "MeshField_For.hpp"        //remove?
#include "MeshField_ShapeField.hpp" //remove?
#ifdef MESHFIELDS_ENABLE_CABANA
#include "CabanaController.hpp"
#endif
#include "Omega_h_build.hpp"
#include "Omega_h_file.hpp"
#include "Omega_h_simplex.hpp"
#include <Kokkos_Core.hpp>
#include <iostream>
#include <sstream>
#include <chrono>

using ExecutionSpace = Kokkos::DefaultExecutionSpace;
using MemorySpace = Kokkos::DefaultExecutionSpace::memory_space;

struct function {
  KOKKOS_INLINE_FUNCTION
  MeshField::Real operator()(MeshField::Real x, MeshField::Real y) const {
    return x;
  }
};


int main(int argc, char **argv) {
  Kokkos::initialize(argc, argv);
  {
  auto lib = Omega_h::Library(&argc, &argv);
  size_t numberOfElems = 1;
  size_t order = 1;
  size_t which = 0;
  if (argc == 4) {
    numberOfElems = atoi(argv[1]);
    order = atoi(argv[2]);
    which = atoi(argv[3]);
  }
  auto world = lib.world();
  const auto family = OMEGA_H_SIMPLEX;
  auto mesh = Omega_h::build_box(world, family, 1, 1, 0, numberOfElems, numberOfElems, 0);
  MeshField::OmegahMeshField<ExecutionSpace, 2, MeshField::KokkosController> omf(mesh);
  auto field = omf.template CreateLagrangeField<MeshField::Real, 1, 2>();
  auto coords = mesh.coords();
  Kokkos::parallel_for(mesh.nverts(), KOKKOS_LAMBDA(int vtx) {
    field(vtx, 0, 0, MeshField::Vertex) = coords[vtx * 2];
    field(vtx, 0, 1, MeshField::Vertex) = coords[vtx * 2 + 1]; 
  }); 
  Kokkos::View<double **> local_coords("", mesh.nelems() * 3, 3); 
  Kokkos::parallel_for("set", mesh.nelems() * 3, KOKKOS_LAMBDA(const int &i) {
    const auto nodeIndex = i % 3;
    local_coords(i, 0) = (nodeIndex == 0); 
    local_coords(i, 1) = (nodeIndex == 1); 
    local_coords(i, 2) = (nodeIndex == 2); 
  }); 
  if (which & 0b1) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = omf.triangleLocalPointEval(local_coords, 3, field);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "time to evaluate: " << mesh.nelems() << "," << std::chrono::duration<double>(end - start).count() << std::endl;
  }
  MeshField::FieldElement f(mesh.nelems(), field, MeshField::LinearTriangleShape(), MeshField::Omegah::LinearTriangleToVertexField(mesh));
  if (which & 0b10) { 
    auto start = std::chrono::high_resolution_clock::now();
    const auto J = MeshField::getJacobians(f, local_coords, 3); 
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "time to get jacobians: " << mesh.nelems() << "," << std::chrono::duration<double>(end - start).count() << std::endl;
  }
  if (which & 0b100) {
    const auto J = MeshField::getJacobians(f, local_coords, 3); 
    auto start = std::chrono::high_resolution_clock::now();
    const auto determinants = MeshField::getJacobianDeterminants(f, J); 
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "time to calculate Jacobian determinants: " << mesh.nelems() << "," << std::chrono::duration<double>(end - start).count() << std::endl;
  }
  }
  Kokkos::finalize();
  return 0;
}
