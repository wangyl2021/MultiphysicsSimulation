// ===== 温度驱动退化模型 =====
    volScalarField& damage = damages[i];

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
    thermo.alpha()*(1.0 - damage)
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

        Info<< "Min/max T:" << min(thermo.T()).value() << ' '
            << max(thermo.T()).value() << endl;

        if (finalIter)
        {       
            mesh.data().setFinalIteration(false);
        }
    }
}
