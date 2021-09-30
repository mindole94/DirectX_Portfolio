#include "stdafx.h"
#include "ExportFile.h"
#include "Converter.h"

void ExportFile::Initialize()
{
	//Airplane();
	//Tower();
	//Tank();
	//Kachujin();
	//Weapons();

	//Ninja();
}

void ExportFile::Airplane()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"B787/Airplane.fbx");
	conv->ExportMesh(L"B787/Airplane");
	conv->ExportMaterial(L"B787/Airplane");
	SafeDelete(conv);
}

void ExportFile::Tower()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"Tower/Tower.fbx");
	conv->ExportMesh(L"Tower/Tower");
	conv->ExportMaterial(L"Tower/Tower");
	SafeDelete(conv);
}

void ExportFile::Tank()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"Tank/Tank.fbx");
	conv->ExportMesh(L"Tank/Tank");
	conv->ExportMaterial(L"Tank/Tank", false);
	SafeDelete(conv);
}

void ExportFile::Kachujin()
{
	Converter* conv = NULL;
	
	conv = new Converter();
	conv->ReadFile(L"Kachujin/Mesh.fbx");
	conv->ExportMesh(L"Kachujin/Mesh");
	conv->ExportMaterial(L"Kachujin/Mesh");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Kachujin/Sword And Shield Idle.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Sword And Shield Idle");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Kachujin/Sword And Shield Walk.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Sword And Shield Walk");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Kachujin/Sword And Shield Run.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Sword And Shield Run");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Kachujin/Sword And Shield Slash.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Sword And Shield Slash");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Kachujin/Salsa Dancing.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Salsa Dancing");
	SafeDelete(conv);
}

void ExportFile::Weapons()
{
	vector<wstring> names;
	names.push_back(L"Cutter.fbx");
	names.push_back(L"Cutter2.fbx");
	names.push_back(L"Dagger_epic.fbx");
	names.push_back(L"Dagger_small.fbx");
	names.push_back(L"Katana.fbx");
	names.push_back(L"LongArrow.obj");
	names.push_back(L"LongBow.obj");
	names.push_back(L"Rapier.fbx");
	names.push_back(L"Sword.fbx");
	names.push_back(L"Sword2.fbx");
	names.push_back(L"Sword_epic.fbx");

	for (wstring name : names)
	{
		Converter* conv = new Converter();
		conv->ReadFile(L"Weapon/" + name);


		String::Replace(&name, L".fbx", L"");
		String::Replace(&name, L".obj", L"");

		conv->ExportMaterial(L"Weapon/" + name, false);
		conv->ExportMesh(L"Weapon/" + name);
		SafeDelete(conv);
	}
}

void ExportFile::Ninja()
{
	Converter* conv = NULL;

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/Ninja_Body.fbx");
	conv->ExportMesh(L"Ninja_Body/Ninja_Body");
	conv->ExportMaterial(L"Ninja_Body/Ninja_Body");
	SafeDelete(conv);
	//////////////////////////////////////////////////////////////////////////////////////////////
	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/sReaction.FBX");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/sReaction");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/TornadoShot.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/TornadoShot");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/TransFormAtk02Loop.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/TransFormAtk02Loop");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/TransFormAtk02Shot.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/TransFormAtk02Shot");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/TransFormAtk02Start.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/TransFormAtk02Start");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/TransFormAtkFloat.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/TransFormAtkFloat");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/TransFormAtkStart.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/TransFormAtkStart");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/TwinSlashR.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/TwinSlashR");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/TwinSlashShot.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/TwinSlashShot");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/TwinSlashStart.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/TwinSlashStart");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/Wait.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/Wait");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/TransFormAtk02End.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/TransFormAtk02End");
	SafeDelete(conv);

	//////////////////////////////////////////////////////////////////////////////////////////////

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/AssassinationStrikeEnd.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/AssassinationStrikeEnd");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/AssassinationStrikeLoop.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/AssassinationStrikeLoop");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/AssassinationStrikeStop.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/AssassinationStrikeStop");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/BackJumpEnd.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/BackJumpEnd");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/BackJumpStart.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/BackJumpStart");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/bReactionCom2.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/bReactionCom2");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/bReactionCom3.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/bReactionCom3");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/bReactionLand3.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/bReactionLand3");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/bReactionRolling3.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/bReactionRolling3");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/bReactionStart2.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/bReactionStart2");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/bReactionStart3.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/bReactionStart3");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/BuffMove.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/BuffMove");
	SafeDelete(conv);

	//////////////////////////////////////////////////////////////////////////////////////////////

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/bReactionDown2.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/bReactionDown2");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/bReactionDown3.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/bReactionDown3");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/BuffStand.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/BuffStand");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/CatchBackStingEnd.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/CatchBackStingEnd");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/CatchBackStingStart.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/CatchBackStingStart");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/combo1.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/combo1");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/combo1R.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/combo1R");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/combo2.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/combo2");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/combo2R.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/combo2R");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/combo3.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/combo3");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/combo3R.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/combo3R");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/combo4.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/combo4");
	SafeDelete(conv);

	//////////////////////////////////////////////////////////////////////////////////////////////

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/combo4R.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/combo4R");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/Combo5.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/Combo5");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/Combo5R.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/Combo5R");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/Combo6.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/Combo6");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/Combo6R.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/Combo6R");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/Combo7.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/Combo7");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/DashEnd.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/DashEnd");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/DashStart.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/DashStart");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/Death.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/Death");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/deathwait.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/deathwait");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/FlameAtkEnd.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/FlameAtkEnd");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/FlameAtkLoop.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/FlameAtkLoop");
	SafeDelete(conv);

	//////////////////////////////////////////////////////////////////////////////////////////////

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/FlameStrikeR.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/FlameStrikeR");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/FlameStrikeShot.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/FlameStrikeShot");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/FlameStrikeStart.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/FlameStrikeStart");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/groggy1.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/groggy1");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/inweapon.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/inweapon");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/outweapon.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/outweapon");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/RapidCut.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/RapidCut");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/RapidCutShot.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/RapidCutShot");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/RapidSubCombo.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/RapidSubCombo");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/RapidSubComboEnd.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/RapidSubComboEnd");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/RapidSubStart.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/RapidSubStart");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/RisingAttack.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/RisingAttack");
	SafeDelete(conv);

	//////////////////////////////////////////////////////////////////////////////////////////////

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/RotateShurikenR.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/RotateShurikenR");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/RotateShurikenShot.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/RotateShurikenShot");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/RotateShurikenStart.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/RotateShurikenStart");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/RotateSlashShot01.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/RotateSlashShot01");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/RotateSlashShot02.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/RotateSlashShot02");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/RotateSlashStart.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/RotateSlashStart");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/SmokeBomb.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/SmokeBomb");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/run.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/run");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/SlashCut01.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/SlashCut01");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/SlashCut02.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/SlashCut02");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/SlashCut03.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/SlashCut03");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Ninja_Body/clip/SlashCut04.fbx");
	conv->ExportAnimClip(0, L"Ninja_Body/clip/SlashCut04");
	SafeDelete(conv);
}
