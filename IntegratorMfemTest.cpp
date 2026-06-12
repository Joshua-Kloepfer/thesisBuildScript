#include "mfem.hpp"
#include "mfem-performance.hpp"
#include <fstream>
#include <iostream>
#include <chrono>

using namespace std;
using namespace mfem;

double intFunc(const Vector &x) {
  return x[0];
}

int main(int argc, char *argv[]) {
  Mpi::Init(argc, argv);
  OptionsParser args(argc, argv);
  int order = 1;
  int size = 1;
  int numberOfElements = 1;
  int type = 1;
  int runs = 1;
  args.AddOption(&order, "-o", "--order", "Finite element polynomial degree");
  args.AddOption(&size, "-s", "--size", "Size of mesh");
  args.AddOption(&numberOfElements, "-n", "--numElem", "number of elements in each direction");
  args.AddOption(&type, "-t", "--type", "type of element(1 for tet, 0 for triangle");
  args.AddOption(&runs, "-r", "--runs", "number of times to run test");
  args.Parse();
  Device device("ceed-hip");
  device.Print();
  if (type == 1) {
    Mesh mesh = Mesh::MakeCartesian3D(numberOfElements, numberOfElements, numberOfElements, Element::CUBE, size, size, size);
    H1_FECollection fec(order, mesh.Dimension());
    FiniteElementSpace fespace(&mesh, &fec);

    FunctionCoefficient custom(intFunc);
    GridFunction f_gf(&fespace);
    f_gf.ProjectCoefficient(custom);
    f_gf.GetMemory().UseDevice(true);
    BilinearForm b(&fespace);
    b.AddDomainIntegrator(new MassIntegrator());
    b.SetAssemblyLevel(AssemblyLevel::PARTIAL);
    Vector result(fespace.GetTrueVSize());
    result.UseDevice(true);
    b.Assemble();
    for (int i = 0; i < runs; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        //b.Assemble();
        b.Mult(f_gf, result);
        double sum = result.Sum();
        auto end = std::chrono::high_resolution_clock::now();
        cout << "RESULT: " << mfem::GetGitStr() << "," << mesh.GetNE() << "," << order << "," << sum << "," << std::chrono::duration<double>(end - start).count() << endl;
    }
  }
  else {
    Mesh mesh = Mesh::MakeCartesian2D(numberOfElements, numberOfElements, Element::QUADRILATERAL, size, size);
    H1_FECollection fec(order, mesh.Dimension());
    FiniteElementSpace fespace(&mesh, &fec);
    LinearForm b(&fespace);
    FunctionCoefficient custom(intFunc);
    auto *integrator = new mfem::DomainLFIntegrator(custom);
    const IntegrationRule &ir = IntRules.Get(Geometry::TRIANGLE, 2);
    integrator->SetIntRule(&ir);
    b.AddDomainIntegrator(integrator);
    for (int i = 0; i < runs; ++i) {
      auto start = std::chrono::high_resolution_clock::now();
      b.Assemble();
      double sum = b.Sum();
      auto end = std::chrono::high_resolution_clock::now();
      cout << "RESULT: " << mfem::GetGitStr() << "," << mesh.GetNE() << "," << order << "," << sum << "," << std::chrono::duration<double>(end - start).count() << endl;

    }
  }
  return 0;
}
