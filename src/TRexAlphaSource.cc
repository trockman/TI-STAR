/*
 * TRexAlphaSource.cc
 *
 *  Created on: Jun 16, 2014
 *      Author: sklupp
 */

#include "TRexAlphaSource.hh"

#include "TRexSettings.hh"
#include "G4ParticleGun.hh"
#include "G4Alpha.hh"
#include "Randomize.hh"

TRexAlphaSource::TRexAlphaSource() {
	fParticleGun = new G4ParticleGun(1);
}

TRexAlphaSource::~TRexAlphaSource() {
	// TODO Auto-generated destructor stub
}

/************************************************************
 *
 * Simulates a quadruple alpha source
 *
 ************************************************************/
void TRexAlphaSource::GeneratePrimaries(G4Event *anEvent) {
	//choose the emitted alpha
	double tmp = CLHEP::RandFlat::shoot(0.,3.);

	if(tmp < 1) { //239Pu
		tmp = CLHEP::RandFlat::shoot(0., 99.82);//70.77+17.11+11.94

		if(tmp < 70.77) {
			fReactionEnergy = 5156.59*CLHEP::keV;
		} else if(tmp < 70.77+17.11) {
			fReactionEnergy = 5144.3*CLHEP::keV;
		} else {
			fReactionEnergy = 5105.5*CLHEP::keV;
		}
	} else if(tmp < 2) {//241Am
		tmp = CLHEP::RandFlat::shoot(0., 97.9);//84.8+13.1

		if(tmp < 84.8) {
			fReactionEnergy = 5485.56*CLHEP::keV;
		} else {
			fReactionEnergy = 5442.80*CLHEP::keV;
		}
	} else {//244Cm
		tmp = CLHEP::RandFlat::shoot(0., 100.);//76.4+23.6

		if(tmp < 76.4) {
			fReactionEnergy = 5804.77*CLHEP::keV;
		} else {
			fReactionEnergy = 5762.64*CLHEP::keV;
		}
	}

	// shoot the alpha emission point
	ShootReactionPosition();

	fParticleGun->SetParticleDefinition(G4Alpha::AlphaDefinition());
	fParticleGun->SetParticlePosition(G4ThreeVector(fReactionX, fReactionY, fReactionZ));
	fParticleGun->SetParticleEnergy(fReactionEnergy);

	// isotropic distribution, if reaction_z < 0 shoot only to negative z's (theta < 90) otherwise only to positive z's (theta > 90)
	CreateIsotropicDistribution();

	G4ThreeVector direction;
	direction.set(1,1,1);
	direction.setMag(1.);
	direction.setTheta(fThetaCM);
	direction.setPhi(fPhi);
	fParticleGun->SetParticleMomentumDirection(direction);

	fParticleGun->GeneratePrimaryVertex(anEvent);

	FillTree();
}

void TRexAlphaSource::ShootReactionPosition() {
	G4double alphaSourceDiameter = TRexSettings::Get()->GetAlphaSourceDiameter() / CLHEP::mm;
	G4double alphaSourceThickness = TRexSettings::Get()->GetAlphaSourceThickness();

	do {
		fReactionX = CLHEP::RandFlat::shoot(-alphaSourceDiameter / 2., alphaSourceDiameter / 2.);
		fReactionY = CLHEP::RandFlat::shoot(-alphaSourceDiameter / 2., alphaSourceDiameter / 2.);
	} while(sqrt(pow(fReactionX,2) + pow(fReactionY,2)) > alphaSourceDiameter / 2.);

	fReactionX *= CLHEP::mm;
	fReactionY *= CLHEP::mm;

	// alpha source: only from the surface of the 'target' (i.e. the source)
	double tmp = CLHEP::RandFlat::shoot(0.,2.);

	if(tmp < 1.) {
		fReactionZ = -alphaSourceThickness;
	} else {
		fReactionZ = alphaSourceThickness;
	}
}

void TRexAlphaSource::CreateIsotropicDistribution() {
	if(fReactionZ < 0) {
		fThetaCM = CLHEP::RandFlat::shoot(-1., 0.);
	} else {
		fThetaCM = CLHEP::RandFlat::shoot(0., 1.);
	}
	fThetaCM = acos(fThetaCM)*CLHEP::radian;

	fPhi = CLHEP::RandFlat::shoot(-M_PI,M_PI)*CLHEP::radian;
	//fPhi = CLHEP::RandFlat::shoot(0., 2.* M_PI)*CLHEP::radian;
	//fPhi = CLHEP::RandFlat::shoot(-M_PI / 2.,M_PI + M_PI / 2.)*CLHEP::radian;
}

void TRexAlphaSource::CreateTreeBranches() {
	if(!fTree) {
		std::cout << "\n\n\nTRexAlphaSource: Tree doesn't exist!\n\n" << std::endl;
	}
	fTree->Branch("reactionEnergy", &fReactionEnergy, "reactionEnergy/D");
	fTree->Branch("reactionX", &fReactionX, "reactionX/D");
	fTree->Branch("reactionY", &fReactionY, "reactionY/D");
	fTree->Branch("reactionZ", &fReactionZ, "reactionZ/D");
	fTree->Branch("thetaCM", &fThetaCM, "thetaCM/D");
	fTree->Branch("phi", &fPhi, "phi/D");
}



