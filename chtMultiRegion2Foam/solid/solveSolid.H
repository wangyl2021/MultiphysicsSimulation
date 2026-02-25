#include "OFstream.H"
#include "OSspecific.H"
// ===== 温度驱动退化模型 =====
    volScalarField& damage = damages[i];

    dimensionedScalar RH("RH", dimless, 0.8);
    dimensionedScalar A("A", dimless/dimTime/dimTemperature, 1e-5);
    dimensionedScalar Tcrit("Tcrit", dimTemperature, 300);
{
    // 温度超限量
    volScalarField deltaT
    (
    IOobject
    (
        "deltaT",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    max
    (
        thermo.T() - Tcrit,
        dimensionedScalar("zeroT", dimTemperature, 0.0)
    )
    );

    // 时间累计
    damage += runTime.deltaT()*A*deltaT;

    // 限制上限
    damage = min
    (
       damage,
       dimensionedScalar("maxDamage", dimless, 0.5)
    );
    damage.correctBoundaryConditions();

    // 构造退化后的导热率
    volScalarField alphaEff
    (
    IOobject
    (
        "alphaEff",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    thermo.alpha()*(1.0 - damage)*RH
    );

    fvScalarMatrix hEqn
    (
        fvm::ddt(betav*rho, h)
        - fvm::laplacian(betav*alphaEff, h, "laplacian(alpha,h)")
        ==
        fvOptions(rho, h)
    );

    hEqn.relax();

    fvOptions.constrain(hEqn);

    if (coupled)
    {
        fvMatrixAssemblyPtr->addFvMatrix(hEqn);
    }
    else
    {
        Info<< "\nSolving for solid region "<< solidRegions[i].name() << endl;

        if (finalIter)
        {
            mesh.data().setFinalIteration(true);
        }

        hEqn.solve(h.select(finalIter));

        fvOptions.correct(h);

        thermo.correct();

{
//输出指定 cellZone 的温度
word zoneName("solid"); 

label zoneID = mesh.cellZones().findZoneID(zoneName);
int currentTime = atoi(runTime.timeName().c_str());
if (zoneID != -1 && currentTime % 1000 == 0)
{
    const cellZone& cz = mesh.cellZones()[zoneID];

    fileName dir
    (
        "postProcessing/"
      + solidRegions[i].name()
      + "/zoneT"
    );

    mkDir(dir);

    fileName fileNameOut
    (
        dir/"T_" + runTime.timeName() + ".txt"
    );
    OFstream outFile(fileNameOut);

    const volScalarField& Tfield = thermo.T();

    forAll(cz, j)
    {
        label cellI = cz[j];

        outFile << cellI << " "
                << Tfield[cellI] << nl;
    }
}
}

        Info<< "Min/max T:" << min(thermo.T()).value() << ' '
            << max(thermo.T()).value() << endl;

        if (finalIter)
        {       
            mesh.data().setFinalIteration(false);
        }
    }
}
