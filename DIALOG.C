/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.

   Menu utilities by Rapha‰l Quinet <quinet@montefiore.ulg.ac.be>
   Drop-down menus by Dewi Morgan <d.morgan@bradford.ac.uk>
   
   DIALOG.C - Simple radio button dialogs for DEU (or other programs).
   */

/* the includes */
#include "deu.h"
#include "levels.h"
#include "keys.h"
#include "dialog.h"

#define GenSectorBase					0x20
#define SectorLighting				0x1f
#define SectorDamage					0x60
#define SectorSecret					0x80
#define SectorIcy							0x100
#define SectorWindy						0x200

#define SectorLightingShift		0
#define SectorDamageShift			5
#define SectorSecretShift			7
#define SectorIcyShift				8
#define SectorWindyShift			9

// define names for the Lighting field of the general sector types

typedef enum
{
	NoLighting,
	RandomBlink,
	FastBlink,
	SlowBlink,
	Pulsate,
	SlowSync,
	FastSync,
	Flicker,
} lightingtype_e;

typedef enum
{
	NoDamage,
	Damage5,
	Damage10,
	Damage20,
} damagetype_e;

// define variables for the button states in general sector dialog

int GSLightingValue=0;
int GSDamageValue=0;
int GSSecretValue=0;
int GSIcyValue=0;
int GSWindyValue=0;

// define text names for the fields of the general sector dialog

static char *Lights[]=
{"","Rnd","Fst","Slw","Pls","SSy","FSy","Flk"};
static char *Damage[]=
{"","5","10","20"};
static char *Secrets[]=
{"","Se"};
static char *Ices[]=
{"","Ic"};
static char *Winds[]=
{"","Wn"};

// define a lookup table between lighting field and lighting value

static int LightingValue[] = {0,1,2,3,8,12,13,17};

char *ParseGeneralizedSectorType(BCINT type)
{
	static char linedes[32];

	strcpy(linedes,"");
	if (type>=0x20)			 // Generalized type
	{
		// put in lighting type
		strcat(linedes,Lights[(type&SectorLighting)>>SectorLightingShift]);		
		strcat(linedes," ");

		// put in damage description
		strcat(linedes,Damage[(type&SectorDamage)>>SectorDamageShift]);
		strcat(linedes," ");

		// put in secret
		strcat(linedes,Secrets[(type&SectorSecret)>>SectorSecretShift]);
		strcat(linedes," ");

		// put in icy
		strcat(linedes,Ices[(type&SectorIcy)>>SectorIcyShift]);
		strcat(linedes," ");

		// put in windy
		strcat(linedes,Winds[(type&SectorWindy)>>SectorWindyShift]);
		strcat(linedes," ");
	}
	else return NULL;

	return linedes;
}

int ComputeGeneralizedSectorTypeValue()
{
	int value = 0;

	value += LightingValue[GSLightingValue];
	value += (GSDamageValue << SectorDamageShift)	& SectorDamage;
	value += (GSSecretValue << SectorSecretShift)	& SectorSecret;
	value += (GSIcyValue << SectorIcyShift)	& SectorIcy;
	value += (GSWindyValue << SectorWindyShift)	& SectorWindy;

	return value;
}

// define names for the Speed field of the general linedefs

typedef enum
{
	SpeedSlow,
	SpeedNormal,
	SpeedFast,
	SpeedTurbo,
} motionspeed_e;

// define masks and shifts for the floor type fields

#define FloorCrush            0x1000
#define FloorChange           0x0c00
#define FloorTarget           0x0380
#define FloorDirection        0x0040
#define FloorModel            0x0020
#define FloorSpeed            0x0018

#define FloorCrushShift           12
#define FloorChangeShift          10
#define FloorTargetShift           7
#define FloorDirectionShift        6
#define FloorModelShift            5
#define FloorSpeedShift            3
													     
// define names for the Target field of the general floor

typedef enum
{	FtoHnF,	FtoLnF,	FtoNnF,	FtoLnC,	FtoC,	FbyST, Fby24,	Fby32,}
floortarget_e;

// define names for the Changer Type field of the general floor

typedef enum
{	FNoChg, FChgZero, FChgTxt, FChgTyp,}
floorchange_e;

// define names for the Change Model field of the general floor

typedef enum
{ FTriggerModel, FNumericModel,}
floormodel_t;

// define masks and shifts for the ceiling type fields

#define CeilingCrush          0x1000
#define CeilingChange         0x0c00
#define CeilingTarget         0x0380
#define CeilingDirection      0x0040
#define CeilingModel          0x0020
#define CeilingSpeed          0x0018

#define CeilingCrushShift         12
#define CeilingChangeShift        10
#define CeilingTargetShift         7
#define CeilingDirectionShift      6
#define CeilingModelShift          5
#define CeilingSpeedShift          3

// define names for the Target field of the general ceiling

typedef enum
{	CtoHnC,	CtoLnC,	CtoNnC,	CtoHnF,	CtoF,	CbyST, Cby24, Cby32,}
ceilingtarget_e;

// define names for the Changer Type field of the general ceiling

typedef enum
{	CNoChg, CChgZero, CChgTxt, CChgTyp,}
ceilingchange_e;

// define names for the Change Model field of the general ceiling

typedef enum
{ CTriggerModel, CNumericModel,}
ceilingmodel_e;

// define masks and shifts for the lift type fields

#define LiftTarget         0x0300
#define LiftDelay          0x00c0
#define LiftMonster        0x0020
#define LiftSpeed          0x0018

#define LiftTargetShift         8
#define LiftDelayShift          6
#define LiftMonsterShift        5
#define LiftSpeedShift          3

// define names for the Target field of the general lift

typedef enum
{	F2LnF,	F2NnF,	F2LnC,	LnF2HnF,}
lifttarget_e;

// define masks and shifts for the stairs type fields

#define StairIgnore        0x0200
#define StairDirection     0x0100
#define StairStep          0x00c0
#define StairMonster       0x0020
#define StairSpeed         0x0018

#define StairIgnoreShift        9
#define StairDirectionShift     8
#define StairStepShift          6
#define StairMonsterShift       5
#define StairSpeedShift         3

// define masks and shifts for the crusher type fields

#define CrusherSilent      0x0040
#define CrusherMonster     0x0020
#define CrusherSpeed       0x0018

#define CrusherSilentShift      6
#define CrusherMonsterShift     5
#define CrusherSpeedShift       3

// define masks and shifts for the door type fields

#define DoorDelay					 0x0300
#define DoorMonster        0x0080
#define DoorKind           0x0060
#define DoorSpeed          0x0018

#define DoorDelayShift		      8
#define DoorMonsterShift        7
#define DoorKindShift           5
#define DoorSpeedShift          3

// define names for the door Kind field of the general ceiling

typedef enum
{	OdCDoor,	ODoor,	CdODoor,	CDoor,}
doorkind_e;

// define masks and shifts for the locked door type fields

#define LockedNKeys  			 0x0200
#define LockedKey          0x01c0
#define LockedKind         0x0020
#define LockedSpeed        0x0018

#define LockedNKeysShift	      9
#define LockedKeyShift          6
#define LockedKindShift         5
#define LockedSpeedShift        3

// define names for the locked door Key field of the general locked door

typedef enum
{	AnyKey, RCard, BCard, YCard, RSkull, BSkull, YSkull, AllKeys,}
keykind_e;

// define variables to hold the button states in the dialogs

int FTriggerValue = 3;	// floors
int FSpeedValue = 1;
int FTargetValue = 0;
int FChangeValue = 0;
int FModelValue = 0;
int FDirValue = 1;
int FCrushValue = 0;

int CTriggerValue = 3;	// ceilings
int CSpeedValue = 1;
int CTargetValue = 0;
int CChangeValue = 0;
int CModelValue = 0;
int CDirValue = 0;
int CCrushValue = 0;

int DTriggerValue = 3;	// doors
int DSpeedValue = 1;
int DKindValue = 0;
int DDelayValue = 1;
int DMonstValue = 0;

int KTriggerValue = 3;	// keyed doors
int KKeyValue = 0;
int KSpeedValue = 1;
int KKindValue = 0;
int KNKeysValue = 0;

int LTriggerValue = 3;	// lifts
int LSpeedValue = 1;
int LTargetValue = 0;
int LDelayValue = 1;
int LMonstValue = 0;

int STriggerValue = 3;	// stairs
int SSpeedValue = 1;
int SDirValue = 1;
int SStepValue = 2;
int SMonstValue = 0;
int SIgnoreValue = 0;

int RTriggerValue = 3;	// cRushers
int RSpeedValue = 1;
int RSilentValue = 0;
int RMonstValue = 0;

// define text names for the fields of the generalized linedefs

static char *Triggers[]=
{"W1","WR","S1","SR","G1","GR","D1","DR",};

static char *FloorTargets[]=
{"F->HnF","F->LnF","F->NnF","F->LnC","F->C","FbysT","Fby24","Fby32",};

static char *Directions[]=
{"Dn","Up",};

static char *Speeds[]=
{"S","N","F","T",};

static char *Changers[]=
{"","c0","cT","cS",};

static char *Models[]=
{"t","n",};

static char *Crushers[]=
{"","Cr",};

static char *CeilingTargets[]=
{"C->HnC","C->LnC","C->NnC","C->HnF","C->F","CbysT","Cby24","Cby32",};

static char *Doors1[]=
{"OpnD","Opn","ClsD","Cls",};

static char *Doors2[]=
{"Cls","","Opn","",};

static char *Delays[]=
{"1","4","9","30",};

static char *LockedDelays[]=
{"4",};

static char *Locks[]=
{"Any","RC","BC","YC","RS","BS","YS","All6",
 "Any","RK","BK" "YK","RK","BK","YK","All3",};

static char *LiftTargets[]=
{"F->LnF","F->NnF","F->LnC","LnF-HnF",};

static char *LiftDelays[]=
{"1","3","5","10",};

static char *Steps[]=
{"s4","s8","s16","s24",};

char *ParseGeneralizedLinedef(BCINT type)
{
	static char linedes[32];

	strcpy(linedes,"");
	if (type>=0x6000)			 // Floor type
	{
		// put in trigger type
		strcat(linedes,Triggers[type&TriggerType]);		
		if (!(type&FloorChange) && (type&FloorModel))
			strcat(linedes,"m");
		strcat(linedes," ");

		// put in target description
		strcat(linedes,FloorTargets[(type&FloorTarget)>>FloorTargetShift]);
		strcat(linedes," ");

		// put in direction and speed
		strcat(linedes,Directions[(type&FloorDirection)>>FloorDirectionShift]);
		strcat(linedes,Speeds[(type&FloorSpeed)>>FloorSpeedShift]);

		// if a changer, put in change type and model
		if (type&FloorChange)
		{
			strcat(linedes," ");
			strcat(linedes,Changers[(type&FloorChange)>>FloorChangeShift]);
			strcat(linedes,Models[(type&FloorModel)>>FloorModelShift]);
		}
		// if a crusher, put in crusher indicator
		if (type&FloorCrush)
		{
			strcat(linedes," ");
			strcat(linedes,Crushers[(type&FloorCrush)>>FloorCrushShift]);
		}
	}
  else if (type>=0x4000) // Ceiling type
	{
		// put in trigger type
		strcat(linedes,Triggers[type&TriggerType]);		
		if (!(type&CeilingChange) && (type&CeilingModel))
			strcat(linedes,"m");
		strcat(linedes," ");

		// put in target description
		strcat(linedes,CeilingTargets[(type&CeilingTarget)>>CeilingTargetShift]);
		strcat(linedes," ");

		// put in direction and speed
		strcat(linedes,Directions[(type&CeilingDirection)>>CeilingDirectionShift]);
		strcat(linedes,Speeds[(type&CeilingSpeed)>>CeilingSpeedShift]);

		// if a changer, put in change type and model
		if (type&CeilingChange)
		{
			strcat(linedes," ");
			strcat(linedes,Changers[(type&CeilingChange)>>CeilingChangeShift]);
			strcat(linedes,Models[(type&CeilingModel)>>CeilingModelShift]);
		}
		// if a crusher, put in crusher indicator
		if (type&CeilingCrush)
		{
			strcat(linedes," ");
			strcat(linedes,Crushers[(type&CeilingCrush)>>CeilingCrushShift]);
		}
	}
	else if (type>=0x3c00) // Door type
	{
		int k;

		// put in trigger type
		strcat(linedes,Triggers[type&TriggerType]);		
		if (type&DoorMonster)
			strcat(linedes,"m");
		strcat(linedes," ");

		// put in door kind, and any delay
		k = (type&DoorKind)>>DoorKindShift;
		strcat(linedes,Doors1[k]);
		if (!(k&1))
			strcat(linedes,Delays[(type&DoorDelay)>>DoorDelayShift]);
		strcat(linedes,Doors2[k]);
		strcat(linedes," ");

		// put in speed
		strcat(linedes,Speeds[(type&DoorSpeed)>>DoorSpeedShift]);
	}
	else if (type>=0x3800) // Locked Door type
	{
		int k,n;

		// put in trigger type
		strcat(linedes,Triggers[type&TriggerType]);		
		strcat(linedes," ");

		// put in door kind, and any delay
		k = (type&LockedKind)>>LockedKindShift;
		strcat(linedes,Doors1[k]);
		if (!(k&1))
			strcat(linedes,LockedDelays[0]);
		strcat(linedes,Doors2[k]);
		strcat(linedes," ");

		// put in speed
		strcat(linedes,Speeds[(type&LockedSpeed)>>LockedSpeedShift]);
		strcat(linedes," ");

		// put in unlocking key type
		k = (type&LockedKey)>>LockedKeyShift;
		n = (type&LockedNKeys)>>LockedNKeysShift;
		strcat(linedes,Locks[n*8+k]);
	}
	else if (type>=0x3400) // Lift type
	{
		// put in trigger type
		strcat(linedes,Triggers[type&TriggerType]);		
		if (type&LiftMonster)
			strcat(linedes,"m");
		strcat(linedes," Lft ");

		// put in lift target and delay
		strcat(linedes,LiftTargets[(type&LiftTarget)>>LiftTargetShift]);
		strcat(linedes," ");
		strcat(linedes,"D");
		strcat(linedes,LiftDelays[(type&LiftDelay)>>LiftDelayShift]);

		// put in lift speed
		strcat(linedes,Speeds[(type&LiftSpeed)>>LiftSpeedShift]);
	}
	else if (type>=0x3000) // Stairs type
	{
		// put in trigger type
		strcat(linedes,Triggers[type&TriggerType]);		
		if (type&StairMonster)
			strcat(linedes,"m");
		strcat(linedes," Stair");

		// put in direction
		strcat(linedes,Directions[(type&StairDirection)>>StairDirectionShift]);

		// put in stepsize
		strcat(linedes,Steps[(type&StairStep)>>StairStepShift]);

		// put in speed
		strcat(linedes,Speeds[(type&StairSpeed)>>StairSpeedShift]);

		if (type&StairIgnore)
			strcat(linedes," Ign");
	}
	else if (type>=0x2F80) // Crusher type
	{
		// put in trigger type
		strcat(linedes,Triggers[type&TriggerType]);		
		if (type&CrusherMonster)
			strcat(linedes,"m");
		strcat(linedes," Crusher ");

		// put in speed
		strcat(linedes,Speeds[(type&CrusherSpeed)>>CrusherSpeedShift]);

		// put in silent property
		if (type&CrusherSilent)
			strcat(linedes," Silent");
	}
	else return NULL;

	return linedes;
}

int ComputeStairsLinedefValue()
{
	int value = GenStairsBase;

	value += (STriggerValue << TriggerTypeShift) & TriggerType;
	value += (SSpeedValue << StairSpeedShift)	& StairSpeed;
	value += (SDirValue << StairDirectionShift)	& StairDirection;
	value += (SStepValue << StairStepShift)	& StairStep;
	value += (SMonstValue << StairMonsterShift)	& StairMonster;
	value += (SIgnoreValue << StairIgnoreShift)	& StairIgnore;

	return value;
}

int ComputeLiftLinedefValue()
{
	int value = GenLiftBase;

	value += (LTriggerValue << TriggerTypeShift) & TriggerType;
	value += (LSpeedValue << LiftSpeedShift)	& LiftSpeed;
	value += (LTargetValue << LiftTargetShift)	& LiftTarget;
	value += (LDelayValue << LiftDelayShift)	& LiftDelay;
	value += (LMonstValue << LiftMonsterShift)	& LiftMonster;

	return value;
}

int ComputeLockedLinedefValue()
{
	int value = GenLockedBase;

	value += (KTriggerValue << TriggerTypeShift) & TriggerType;
	value += (KKeyValue << LockedKeyShift)	& LockedKey;
	value += (KSpeedValue << LockedSpeedShift)	& LockedSpeed;
	value += (KKindValue << LockedKindShift)	& LockedKind;
	value += (KNKeysValue << LockedNKeysShift)	& LockedNKeys;

	return value;
}

int ComputeDoorLinedefValue()
{
	int value = GenDoorBase;

	value += (DTriggerValue << TriggerTypeShift) & TriggerType;
	value += (DSpeedValue << DoorSpeedShift)	& DoorSpeed;
	value += (DKindValue << DoorKindShift)	& DoorKind;
	value += (DDelayValue << DoorDelayShift)	& DoorDelay;
	value += (DMonstValue << DoorMonsterShift)	& DoorMonster;

	return value;
}

int ComputeCrusherLinedefValue()
{
	int value = GenCrusherBase;

	value += (RTriggerValue << TriggerTypeShift) & TriggerType;
	value += (RSpeedValue << CrusherSpeedShift)	& CrusherSpeed;
	value += (RSilentValue << CrusherSilentShift)	& CrusherSilent;
	value += (RMonstValue << CrusherMonsterShift)	& CrusherMonster;

	return value;
}

int ComputeCeilingLinedefValue()
{
	int value = GenCeilingBase;

	value += (CTriggerValue << TriggerTypeShift) & TriggerType;
	value += (CSpeedValue << CeilingSpeedShift)	& CeilingSpeed;
	value += (CTargetValue << CeilingTargetShift)	& CeilingTarget;
	value += (CChangeValue << CeilingChangeShift)	& CeilingChange;
	value += (CModelValue << CeilingModelShift)	& CeilingModel;
	value += (CDirValue << CeilingDirectionShift)	& CeilingDirection;
	value += (CCrushValue << CeilingCrushShift)	& CeilingCrush;

	return value;
}

int ComputeFloorLinedefValue()
{
	int value = GenFloorBase;

	value += (FTriggerValue << TriggerTypeShift) & TriggerType;
	value += (FSpeedValue << FloorSpeedShift)	& FloorSpeed;
	value += (FTargetValue << FloorTargetShift)	& FloorTarget;
	value += (FChangeValue << FloorChangeShift)	& FloorChange;
	value += (FModelValue << FloorModelShift)	& FloorModel;
	value += (FDirValue << FloorDirectionShift)	& FloorDirection;
	value += (FCrushValue << FloorCrushShift)	& FloorCrush;

	return value;
}

char *TriggerItems[]=
{
	"W1","WR","S1","SR","G1","GR","D1","DR",
};
int NTriggerItems = sizeof(TriggerItems)/sizeof(TriggerItems[0]);

char *SpeedItems[]=
{
	"Slow","Norm","Fast","Turb",
};
int NSpeedItems = sizeof(SpeedItems)/sizeof(SpeedItems[0]);

char *FTargetItems[]=
{
	"HnF","LnF","NnF","LnC","C","ST","24","32",
};
int NFTargetItems = sizeof(FTargetItems)/sizeof(FTargetItems[0]);

char *CTargetItems[]=
{
	"HnC","LnC","NnC","HnF","F","ST","24","32",
};
int NCTargetItems = sizeof(CTargetItems)/sizeof(CTargetItems[0]);

char *ChangeItems[]=
{
	"None","Zero","Txtr","Type",
};
int NChangeItems = sizeof(ChangeItems)/sizeof(ChangeItems[0]);

char *ModelItems[]=
{
	"Trig","Num",
};
int NModelItems = sizeof(ModelItems)/sizeof(ModelItems[0]);

char *DirItems[]=
{
	"Dn","Up",
};
int NDirItems = sizeof(DirItems)/sizeof(DirItems[0]);

char *CrushItems[]=
{
	"No","Yes",
};
int NCrushItems = sizeof(CrushItems)/sizeof(CrushItems[0]);

char *KindItems[]=
{
	"O->C", "O", "C->O", "C",
};
int NKindItems = sizeof(KindItems)/sizeof(KindItems[0]);

char *DelayItems[]=
{
	"1s", "4s", "9s", "30s",
};
int NDelayItems = sizeof(DelayItems)/sizeof(DelayItems[0]);

char *MonstItems[]=
{
	"No", "Yes",
};
int NMonstItems = sizeof(MonstItems)/sizeof(MonstItems[0]);

char *NKeysItems[]=
{
	"6", "3",
};
int NNKeysItems = sizeof(NKeysItems)/sizeof(NKeysItems[0]);

char *KeyItems[]=
{
	"Any", "RC", "BC", "YC", "RS", "BS", "YS", "All",
};
int NKeyItems = sizeof(KeyItems)/sizeof(KeyItems[0]);

char *KKindItems[]=
{
	"O->C", "O",
};
int NKKindItems = sizeof(KKindItems)/sizeof(KKindItems[0]);

char *LTargetItems[]=
{
	"LnF","NnF","LnC","Perp",
};
int NLTargetItems = sizeof(LTargetItems)/sizeof(LTargetItems[0]);

char *LDelayItems[]=
{
	"1s", "3s", "5s", "10s",
};
int NLDelayItems = sizeof(LDelayItems)/sizeof(LDelayItems[0]);

char *StepItems[]=
{
	"4", "8", "16", "24",
};
int NStepItems = sizeof(StepItems)/sizeof(StepItems[0]);

char *IgnoreItems[]=
{
	"No", "Yes",
};
int NIgnoreItems = sizeof(IgnoreItems)/sizeof(IgnoreItems[0]);

char *SilentItems[]=
{
	"No", "Yes",
};
int NSilentItems = sizeof(SilentItems)/sizeof(SilentItems[0]);

char *LightingItems[]=
{
	"None","Rand","Fast","Slow","Puls","SSyn","FSyn","Flkr"
};
int NLightingItems = sizeof(LightingItems)/sizeof(LightingItems[0]);

char *DamageItems[]=
{
	"None","Dmg5","Dmg10","Dmg20"
};
int NDamageItems = sizeof(DamageItems)/sizeof(DamageItems[0]);

char *SecretItems[]=
{
	"No", "Yes",
};
int NSecretItems = sizeof(SecretItems)/sizeof(SecretItems[0]);

char *IcyItems[]=
{
	"No", "Yes",
};
int NIcyItems = sizeof(IcyItems)/sizeof(IcyItems[0]);

char *WindyItems[]=
{
	"No", "Yes",
};
int NWindyItems = sizeof(WindyItems)/sizeof(WindyItems[0]);

char *OkItems[]=
{
	"Ok",
};
int NOkItems = sizeof(OkItems)/sizeof(OkItems[0]);

int OkValue=0;

ButtonElt SectorButtons[]=
{
	{ 0,0,0,0, LightingItems,&NLightingItems,&GSLightingValue},
	{ 1,1,0,0, LightingItems,&NLightingItems,&GSLightingValue},
	{ 2,2,0,0, LightingItems,&NLightingItems,&GSLightingValue},
	{ 3,3,0,0, LightingItems,&NLightingItems,&GSLightingValue},
	{ 4,4,0,0, LightingItems,&NLightingItems,&GSLightingValue},
	{ 5,5,0,0, LightingItems,&NLightingItems,&GSLightingValue},
	{ 6,6,0,0, LightingItems,&NLightingItems,&GSLightingValue},
	{ 7,7,0,0, LightingItems,&NLightingItems,&GSLightingValue},
	{ 0,0,1,1, DamageItems,  &NDamageItems,  &GSDamageValue},
	{ 1,1,1,1, DamageItems,  &NDamageItems,  &GSDamageValue},
	{ 2,2,1,1, DamageItems,  &NDamageItems,  &GSDamageValue},
	{ 3,3,1,1, DamageItems,  &NDamageItems,  &GSDamageValue},
	{-1,4,1,-1,NULL,         NULL,           NULL},
	{-1,5,1,-1,NULL,         NULL,           NULL},
	{ 0,6,1,2, SecretItems,  &NSecretItems,  &GSSecretValue},
	{ 1,7,1,2, SecretItems,  &NSecretItems,  &GSSecretValue},
	{ 0,0,2,2, IcyItems,     &NIcyItems,     &GSIcyValue},
	{ 1,1,2,2, IcyItems,     &NIcyItems,     &GSIcyValue},
	{-1,2,2,-1,NULL,         NULL,           NULL},
	{ 0,3,2,3, WindyItems,   &NWindyItems,   &GSWindyValue},
	{ 1,4,2,3, WindyItems,   &NWindyItems,   &GSWindyValue},
	{-1,5,2,-1,NULL,         NULL,           NULL},
	{-1,6,2,-1,NULL,         NULL,           NULL},
	{ 0,7,2,-1,OkItems,      &NOkItems,      &OkValue},
};

ButtonElt FloorButtons[]=
{
	{ 0,0,0,0, TriggerItems,&NTriggerItems,&FTriggerValue},
	{ 1,1,0,0, TriggerItems,&NTriggerItems,&FTriggerValue},
	{ 2,2,0,0, TriggerItems,&NTriggerItems,&FTriggerValue},
	{ 3,3,0,0, TriggerItems,&NTriggerItems,&FTriggerValue},
	{ 4,4,0,0, TriggerItems,&NTriggerItems,&FTriggerValue},
	{ 5,5,0,0, TriggerItems,&NTriggerItems,&FTriggerValue},
	{ 6,6,0,0, TriggerItems,&NTriggerItems,&FTriggerValue},
	{ 7,7,0,0, TriggerItems,&NTriggerItems,&FTriggerValue},
	{ 0,0,1,1, FTargetItems,&NFTargetItems,&FTargetValue},
	{ 1,1,1,1, FTargetItems,&NFTargetItems,&FTargetValue},
	{ 2,2,1,1, FTargetItems,&NFTargetItems,&FTargetValue},
	{ 3,3,1,1, FTargetItems,&NFTargetItems,&FTargetValue},
	{ 4,4,1,1, FTargetItems,&NFTargetItems,&FTargetValue},
	{ 5,5,1,1, FTargetItems,&NFTargetItems,&FTargetValue},
	{ 6,6,1,1, FTargetItems,&NFTargetItems,&FTargetValue},
	{ 7,7,1,1, FTargetItems,&NFTargetItems,&FTargetValue},
	{ 0,0,2,2, SpeedItems,  &NSpeedItems,  &FSpeedValue},
	{ 1,1,2,2, SpeedItems,  &NSpeedItems,  &FSpeedValue},
	{ 2,2,2,2, SpeedItems,  &NSpeedItems,  &FSpeedValue},
	{ 3,3,2,2, SpeedItems,  &NSpeedItems,  &FSpeedValue},
	{-1,4,2,-1,NULL,        NULL,          NULL},
	{-1,5,2,-1,NULL,        NULL,          NULL},
	{-1,6,2,-1,NULL,        NULL,          NULL},
	{-1,7,2,-1,NULL,        NULL,          NULL},
	{ 0,0,3,3, ChangeItems, &NChangeItems, &FChangeValue},
	{ 1,1,3,3, ChangeItems, &NChangeItems, &FChangeValue},
	{ 2,2,3,3, ChangeItems, &NChangeItems, &FChangeValue},
	{ 3,3,3,3, ChangeItems, &NChangeItems, &FChangeValue},
	{-1,4,3,-1,NULL,        NULL,          NULL},
	{ 0,5,3,4, ModelItems,  &NModelItems,  &FModelValue},
	{ 1,6,3,4, ModelItems,  &NModelItems,  &FModelValue},
	{-1,7,3,-1,NULL,        NULL,          NULL},
	{ 0,0,4,5, DirItems,    &NDirItems,    &FDirValue},
	{ 1,1,4,5, DirItems,    &NDirItems,    &FDirValue},
	{-1,2,4,-1,NULL,        NULL,          NULL},
	{ 0,3,4,6, CrushItems,  &NCrushItems,  &FCrushValue},
	{ 1,4,4,6, CrushItems,  &NCrushItems,  &FCrushValue},
	{-1,5,4,-1,NULL,        NULL,          NULL},
	{-1,6,4,-1,NULL,        NULL,          NULL},
	{ 0,7,4,-1,OkItems,    &NOkItems,     &OkValue},
};

ButtonElt CeilingButtons[]=
{
	{ 0,0,0,0, TriggerItems,&NTriggerItems,&CTriggerValue},
	{ 1,1,0,0, TriggerItems,&NTriggerItems,&CTriggerValue},
	{ 2,2,0,0, TriggerItems,&NTriggerItems,&CTriggerValue},
	{ 3,3,0,0, TriggerItems,&NTriggerItems,&CTriggerValue},
	{ 4,4,0,0, TriggerItems,&NTriggerItems,&CTriggerValue},
	{ 5,5,0,0, TriggerItems,&NTriggerItems,&CTriggerValue},
	{ 6,6,0,0, TriggerItems,&NTriggerItems,&CTriggerValue},
	{ 7,7,0,0, TriggerItems,&NTriggerItems,&CTriggerValue},
	{ 0,0,1,1, CTargetItems,&NCTargetItems,&CTargetValue},
	{ 1,1,1,1, CTargetItems,&NCTargetItems,&CTargetValue},
	{ 2,2,1,1, CTargetItems,&NCTargetItems,&CTargetValue},
	{ 3,3,1,1, CTargetItems,&NCTargetItems,&CTargetValue},
	{ 4,4,1,1, CTargetItems,&NCTargetItems,&CTargetValue},
	{ 5,5,1,1, CTargetItems,&NCTargetItems,&CTargetValue},
	{ 6,6,1,1, CTargetItems,&NCTargetItems,&CTargetValue},
	{ 7,7,1,1, CTargetItems,&NCTargetItems,&CTargetValue},
	{ 0,0,2,2, SpeedItems,  &NSpeedItems,  &CSpeedValue},
	{ 1,1,2,2, SpeedItems,  &NSpeedItems,  &CSpeedValue},
	{ 2,2,2,2, SpeedItems,  &NSpeedItems,  &CSpeedValue},
	{ 3,3,2,2, SpeedItems,  &NSpeedItems,  &CSpeedValue},
	{-1,4,2,-1,NULL,        NULL,          NULL},
	{-1,5,2,-1,NULL,        NULL,          NULL},
	{-1,6,2,-1,NULL,        NULL,          NULL},
	{-1,7,2,-1,NULL,        NULL,          NULL},
	{ 0,0,3,3, ChangeItems, &NChangeItems, &CChangeValue},
	{ 1,1,3,3, ChangeItems, &NChangeItems, &CChangeValue},
	{ 2,2,3,3, ChangeItems, &NChangeItems, &CChangeValue},
	{ 3,3,3,3, ChangeItems, &NChangeItems, &CChangeValue},
	{-1,4,3,-1,NULL,        NULL,          NULL},
	{ 0,5,3,4, ModelItems,  &NModelItems,  &CModelValue},
	{ 1,6,3,4, ModelItems,  &NModelItems,  &CModelValue},
	{-1,7,3,-1,NULL,        NULL,          NULL},
	{ 0,0,4,5, DirItems,    &NDirItems,    &CDirValue},
	{ 1,1,4,5, DirItems,    &NDirItems,    &CDirValue},
	{-1,2,4,-1,NULL,        NULL,          NULL},
	{ 0,3,4,6, CrushItems,  &NCrushItems,  &CCrushValue},
	{ 1,4,4,6, CrushItems,  &NCrushItems,  &CCrushValue},
	{-1,5,4,-1,NULL,        NULL,          NULL},
	{-1,6,4,-1,NULL,        NULL,          NULL},
	{ 0,7,4,-1,OkItems,     &NOkItems,     &OkValue},
};

ButtonElt DoorButtons[]=
{
	{ 0,0,0,0, TriggerItems,&NTriggerItems,&DTriggerValue},
	{ 1,1,0,0, TriggerItems,&NTriggerItems,&DTriggerValue},
	{ 2,2,0,0, TriggerItems,&NTriggerItems,&DTriggerValue},
	{ 3,3,0,0, TriggerItems,&NTriggerItems,&DTriggerValue},
	{ 4,4,0,0, TriggerItems,&NTriggerItems,&DTriggerValue},
	{ 5,5,0,0, TriggerItems,&NTriggerItems,&DTriggerValue},
	{ 6,6,0,0, TriggerItems,&NTriggerItems,&DTriggerValue},
	{ 7,7,0,0, TriggerItems,&NTriggerItems,&DTriggerValue},
	{ 0,0,1,1, SpeedItems,  &NSpeedItems,  &DSpeedValue},
	{ 1,1,1,1, SpeedItems,  &NSpeedItems,  &DSpeedValue},
	{ 2,2,1,1, SpeedItems,  &NSpeedItems,  &DSpeedValue},
	{ 3,3,1,1, SpeedItems,  &NSpeedItems,  &DSpeedValue},
	{-1,4,1,-1,NULL,        NULL,          NULL},
	{-1,5,1,-1,NULL,        NULL,          NULL},
	{-1,6,1,-1,NULL,        NULL,          NULL},
	{-1,7,1,-1,NULL,        NULL,          NULL},
	{ 0,0,2,2, KindItems,   &NKindItems,   &DKindValue},
	{ 1,1,2,2, KindItems,   &NKindItems,   &DKindValue},
	{ 2,2,2,2, KindItems,   &NKindItems,   &DKindValue},
	{ 3,3,2,2, KindItems,   &NKindItems,   &DKindValue},
	{-1,4,2,-1,NULL,        NULL,          NULL},
	{-1,5,2,-1,NULL,        NULL,          NULL},
	{ 0,6,2,3, MonstItems,  &NMonstItems,  &DMonstValue},
	{ 1,7,2,3, MonstItems,  &NMonstItems,  &DMonstValue},
	{ 0,0,3,4, DelayItems,  &NDelayItems,  &DDelayValue},
	{ 1,1,3,4, DelayItems,  &NDelayItems,  &DDelayValue},
	{ 2,2,3,4, DelayItems,  &NDelayItems,  &DDelayValue},
	{ 3,3,3,4, DelayItems,  &NDelayItems,  &DDelayValue},
	{-1,4,3,-1,NULL,        NULL,          NULL},
	{-1,5,3,-1,NULL,        NULL,          NULL},
	{-1,6,3,-1,NULL,        NULL,          NULL},
	{ 0,7,3,-1,OkItems,     &NOkItems,     &OkValue},
};

ButtonElt LockedButtons[]=
{
	{ 0,0,0,0, TriggerItems,&NTriggerItems,&KTriggerValue},
	{ 1,1,0,0, TriggerItems,&NTriggerItems,&KTriggerValue},
	{ 2,2,0,0, TriggerItems,&NTriggerItems,&KTriggerValue},
	{ 3,3,0,0, TriggerItems,&NTriggerItems,&KTriggerValue},
	{ 4,4,0,0, TriggerItems,&NTriggerItems,&KTriggerValue},
	{ 5,5,0,0, TriggerItems,&NTriggerItems,&KTriggerValue},
	{ 6,6,0,0, TriggerItems,&NTriggerItems,&KTriggerValue},
	{ 7,7,0,0, TriggerItems,&NTriggerItems,&KTriggerValue},
	{ 0,0,1,1, KeyItems,    &NKeyItems,    &KKeyValue},
	{ 1,1,1,1, KeyItems,    &NKeyItems,    &KKeyValue},
	{ 2,2,1,1, KeyItems,    &NKeyItems,    &KKeyValue},
	{ 3,3,1,1, KeyItems,    &NKeyItems,    &KKeyValue},
	{ 4,4,1,1, KeyItems,    &NKeyItems,    &KKeyValue},
	{ 5,5,1,1, KeyItems,    &NKeyItems,    &KKeyValue},
	{ 6,6,1,1, KeyItems,    &NKeyItems,    &KKeyValue},
	{ 7,7,1,1, KeyItems,    &NKeyItems,    &KKeyValue},
	{ 0,0,2,2, SpeedItems,  &NSpeedItems,  &KSpeedValue},
	{ 1,1,2,2, SpeedItems,  &NSpeedItems,  &KSpeedValue},
	{ 2,2,2,2, SpeedItems,  &NSpeedItems,  &KSpeedValue},
	{ 3,3,2,2, SpeedItems,  &NSpeedItems,  &KSpeedValue},
	{-1,4,2,-1,NULL,        NULL,          NULL},
	{-1,5,2,-1,NULL,        NULL,          NULL},
	{-1,6,2,-1,NULL,        NULL,          NULL},
	{-1,7,2,-1,NULL,        NULL,          NULL},
	{ 0,0,3,3, KKindItems,  &NKKindItems,  &KKindValue},
	{ 1,1,3,3, KKindItems,  &NKKindItems,  &KKindValue},
	{-1,2,3,-1,NULL,        NULL,          NULL},
	{ 0,3,3,4, NKeysItems,  &NNKeysItems,  &KNKeysValue},
	{ 1,4,3,4, NKeysItems,  &NNKeysItems,  &KNKeysValue},
	{-1,5,3,-1,NULL,        NULL,          NULL},
	{-1,6,3,-1,NULL,        NULL,          NULL},
	{ 0,7,3,-1,OkItems,     &NOkItems,     &OkValue},
};

ButtonElt LiftButtons[]=
{
	{ 0,0,0,0, TriggerItems,&NTriggerItems,&LTriggerValue},
	{ 1,1,0,0, TriggerItems,&NTriggerItems,&LTriggerValue},
	{ 2,2,0,0, TriggerItems,&NTriggerItems,&LTriggerValue},
	{ 3,3,0,0, TriggerItems,&NTriggerItems,&LTriggerValue},
	{ 4,4,0,0, TriggerItems,&NTriggerItems,&LTriggerValue},
	{ 5,5,0,0, TriggerItems,&NTriggerItems,&LTriggerValue},
	{ 6,6,0,0, TriggerItems,&NTriggerItems,&LTriggerValue},
	{ 7,7,0,0, TriggerItems,&NTriggerItems,&LTriggerValue},
	{ 0,0,1,1, SpeedItems,  &NSpeedItems,  &LSpeedValue},
	{ 1,1,1,1, SpeedItems,  &NSpeedItems,  &LSpeedValue},
	{ 2,2,1,1, SpeedItems,  &NSpeedItems,  &LSpeedValue},
	{ 3,3,1,1, SpeedItems,  &NSpeedItems,  &LSpeedValue},
	{-1,4,1,-1,NULL,        NULL,          NULL},
	{-1,5,1,-1,NULL,        NULL,          NULL},
	{-1,6,1,-1,NULL,        NULL,          NULL},
	{-1,7,1,-1,NULL,        NULL,          NULL},
	{ 0,0,2,2, LTargetItems,&NLTargetItems,&LTargetValue},
	{ 1,1,2,2, LTargetItems,&NLTargetItems,&LTargetValue},
	{ 2,2,2,2, LTargetItems,&NLTargetItems,&LTargetValue},
	{ 3,3,2,2, LTargetItems,&NLTargetItems,&LTargetValue},
	{-1,4,2,-1,NULL,        NULL,          NULL},
	{-1,5,2,-1,NULL,        NULL,          NULL},
	{ 0,6,2,3, MonstItems,  &NMonstItems,  &LMonstValue},
	{ 1,7,2,3, MonstItems,  &NMonstItems,  &LMonstValue},
	{ 0,0,3,4, LDelayItems, &NLDelayItems, &LDelayValue},
	{ 1,1,3,4, LDelayItems, &NLDelayItems, &LDelayValue},
	{ 2,2,3,4, LDelayItems, &NLDelayItems, &LDelayValue},
	{ 3,3,3,4, LDelayItems, &NLDelayItems, &LDelayValue},
	{-1,4,3,-1,NULL,        NULL,          NULL},
	{-1,5,3,-1,NULL,        NULL,          NULL},
	{-1,6,3,-1,NULL,        NULL,          NULL},
	{ 0,7,3,-1,OkItems,    &NOkItems,     &OkValue},
};

ButtonElt StairsButtons[]=
{
	{ 0,0,0,0, TriggerItems,&NTriggerItems,&STriggerValue},
	{ 1,1,0,0, TriggerItems,&NTriggerItems,&STriggerValue},
	{ 2,2,0,0, TriggerItems,&NTriggerItems,&STriggerValue},
	{ 3,3,0,0, TriggerItems,&NTriggerItems,&STriggerValue},
	{ 4,4,0,0, TriggerItems,&NTriggerItems,&STriggerValue},
	{ 5,5,0,0, TriggerItems,&NTriggerItems,&STriggerValue},
	{ 6,6,0,0, TriggerItems,&NTriggerItems,&STriggerValue},
	{ 7,7,0,0, TriggerItems,&NTriggerItems,&STriggerValue},
	{ 0,0,1,1, SpeedItems,  &NSpeedItems,  &SSpeedValue},
	{ 1,1,1,1, SpeedItems,  &NSpeedItems,  &SSpeedValue},
	{ 2,2,1,1, SpeedItems,  &NSpeedItems,  &SSpeedValue},
	{ 3,3,1,1, SpeedItems,  &NSpeedItems,  &SSpeedValue},
	{-1,4,1,-1,NULL,        NULL,          NULL},
	{-1,5,1,-1,NULL,        NULL,          NULL},
	{ 0,6,1,2, DirItems,    &NDirItems,    &SDirValue},
	{ 1,7,1,2, DirItems,    &NDirItems,    &SDirValue},
	{ 0,0,2,3, StepItems,   &NStepItems,   &SStepValue},
	{ 1,1,2,3, StepItems,   &NStepItems,   &SStepValue},
	{ 2,2,2,3, StepItems,   &NStepItems,   &SStepValue},
	{ 3,3,2,3, StepItems,   &NStepItems,   &SStepValue},
	{-1,4,2,-1,NULL,        NULL,          NULL},
	{-1,5,2,-1,NULL,        NULL,          NULL},
	{ 0,6,2,4, MonstItems,  &NMonstItems,  &SMonstValue},
	{ 1,7,2,4, MonstItems,  &NMonstItems,  &SMonstValue},
	{ 0,0,3,5, IgnoreItems, &NIgnoreItems, &SIgnoreValue},
	{ 1,1,3,5, IgnoreItems, &NIgnoreItems, &SIgnoreValue},
	{-1,2,3,-1,NULL,        NULL,          NULL},
	{-1,3,3,-1,NULL,        NULL,          NULL},
	{-1,4,3,-1,NULL,        NULL,          NULL},
	{-1,5,3,-1,NULL,        NULL,          NULL},
	{-1,6,3,-1,NULL,        NULL,          NULL},
	{ 0,7,3,-1,OkItems,    &NOkItems,     &OkValue},
};

ButtonElt CrusherButtons[]=
{
	{ 0,0,0,0, TriggerItems,&NTriggerItems,&RTriggerValue},
	{ 1,1,0,0, TriggerItems,&NTriggerItems,&RTriggerValue},
	{ 2,2,0,0, TriggerItems,&NTriggerItems,&RTriggerValue},
	{ 3,3,0,0, TriggerItems,&NTriggerItems,&RTriggerValue},
	{ 4,4,0,0, TriggerItems,&NTriggerItems,&RTriggerValue},
	{ 5,5,0,0, TriggerItems,&NTriggerItems,&RTriggerValue},
	{ 6,6,0,0, TriggerItems,&NTriggerItems,&RTriggerValue},
	{ 7,7,0,0, TriggerItems,&NTriggerItems,&RTriggerValue},
	{ 0,0,1,1, SpeedItems,  &NSpeedItems,  &RSpeedValue},
	{ 1,1,1,1, SpeedItems,  &NSpeedItems,  &RSpeedValue},
	{ 2,2,1,1, SpeedItems,  &NSpeedItems,  &RSpeedValue},
	{ 3,3,1,1, SpeedItems,  &NSpeedItems,  &RSpeedValue},
	{-1,4,1,-1,NULL,        NULL,          NULL},
	{-1,5,1,-1,NULL,        NULL,          NULL},
	{-1,6,1,-1,NULL,        NULL,          NULL},
	{-1,7,1,-1,NULL,        NULL,          NULL},
	{ 0,0,2,2, SilentItems, &NSilentItems, &RSilentValue},
	{ 1,1,2,2, SilentItems, &NSilentItems, &RSilentValue},
	{-1,2,2,-1,NULL,        NULL,          NULL},
	{ 0,3,2,3, MonstItems,  &NMonstItems,  &RMonstValue},
	{ 1,4,2,3, MonstItems,  &NMonstItems,  &RMonstValue},
	{-1,5,2,-1,NULL,        NULL,          NULL},
	{-1,6,2,-1,NULL,        NULL,          NULL},
	{ 0,7,2,-1,OkItems,    &NOkItems,     &OkValue},
};

char *GSSections[]=
{
	"Lights",
	"Damage",
	"Secret",
	"Icy",
	"Windy",
};

char *FCSections[]=
{
	"Trig",
	"Targ",
	"Speed",
	"Change",
	"Model",
	"Dir",
	"Crush",
};

char *DSections[]=
{
	"Trig",
	"Speed",
	"Kind",
	"Monst",
	"Delay",
};

char *KSections[]=
{
	"Trig",
	"Lock",
	"Speed",
	"Kind",
	"NKeys",
};

char *LSections[]=
{
	"Trig",
	"Speed",
	"Targ",
	"Monst",
	"Delay",
};

char *SSections[]=
{
	"Trig",
	"Speed",
	"Dir",
	"Step",
	"Monst",
	"AnyTx",
};

char *RSections[]=
{
	"Trig",
	"Speed",
	"Quiet",
	"Monst",
};

ButtonDialog SectorDialog=
{
	100,100,
	192,176,
	10,35,
	48,14,
	56,17,
	8,3,
	GSSections,
	"Select Sector Type Properties:",
	SectorButtons,
};

ButtonDialog FloorDialog=
{
	100,100,
	256,176,
	10,35,
	40,14,
	47,17,
	8,5,
	FCSections,
	"Select Floor Mover Properties:",
	FloorButtons,
};

ButtonDialog CeilingDialog=
{
	100,100,
	256,176,
	10,35,
	40,14,
	47,17,
	8,5,
	FCSections,
	"Select Ceiling Mover Properties:",
	CeilingButtons,
};

ButtonDialog DoorDialog=
{
	100,100,
	208,176,
	10,35,
	40,14,
	47,17,
	8,4,
	DSections,
	"Select Door Properties:",
	DoorButtons,
};

ButtonDialog LockedDialog=
{
	100,100,
	208,176,
	10,35,
	40,14,
	47,17,
	8,4,
	KSections,
	"Select Keyed Door Properties:",
	LockedButtons,
};

ButtonDialog LiftDialog=
{
	100,100,
	208,176,
	10,35,
	40,14,
	47,17,
	8,4,
	LSections,
	"Select Lift Properties:",
	LiftButtons,
};

ButtonDialog StairsDialog=
{
	100,100,
	208,176,
	10,35,
	40,14,
	47,17,
	8,4,
	SSections,
	"Select Stair Building Properties:",
	StairsButtons,
};

ButtonDialog CrusherDialog=
{
	100,100,
	168,176,
	12,35,
	40,14,
	47,17,
	8,3,
	RSections,
	"Select Crusher Properties:",
	CrusherButtons,
};

void DrawScreenButtonInPlainColor( BCINT Xstart, BCINT Ystart, BCINT Width, BCINT Height, char *Text, int color, Bool Filled);

void DisplayDialogButton(ButtonDialog *d,int row,int col)
{
	int place = d->NR*col+row;

	if (d->btns[place].idx<0)
		return;
	DrawScreenButtonInPlainColor
	(
		d->X0+d->BX0+d->BXS*col,
		d->Y0+d->BY0+d->BYS*row,
		d->BW,
		d->BH,
		d->btns[place].caparray[d->btns[place].idx],
		d->btns[place].idx!=*d->btns[place].curval? BLACK : RED,
		1
	);
	if (d->btns[place].idx!=*d->btns[place].curval)
		DrawScreenButtonOut
		(
			d->X0+d->BX0+d->BXS*col,
			d->Y0+d->BY0+d->BYS*row,
			d->BW,
			d->BH,
			0
		);
}

void ChangeDialogButton(ButtonDialog *d,int r,int c)
{
	int i,j,k,grp,idx;

	if (0<=r && r<d->NR && 0<=c && c<d->NC)
	{
		k = d->NR*c+r;
		idx = d->btns[k].idx;
		grp = d->btns[k].grp;
		if (idx>=0)
			*d->btns[k].curval = idx;

		SetColor(BLACK);
		for (i=0;i<d->NR;i++)
			for (j=0;j<d->NC;j++)
				if (d->btns[d->NR*j+i].grp==grp)
					DisplayDialogButton(d,i,j);
	}
}

// jff display a dialog to set the properties of a generalized lindedef

BCINT DoButtonDialog(ButtonDialog *d)
{
	int i,j,col,line,s;
  Bool  ok;
  BCINT key, buttons, oldbuttons, x0, y0;

	HideMousePointer(); 
	DrawScreenBox3D(d->X0,d->Y0,d->X0+d->W,d->X0+d->H);

  DrawScreenButtonIn(d->X0+3, d->Y0+20, d->W-6, d->H-23, "", FALSE);
	SetColor(CYAN);
	DrawScreenBox( d->X0+3, d->Y0+3, d->X0+d->W-3,d->Y0+17);
	SetColor(BLACK);
	DrawScreenTextFonted(2, 0, d->X0+10, d->Y0+4, d->title);

	SetColor(BLACK);
	for (i=0;i<d->NR;i++)
		for (j=0;j<d->NC;j++)
			DisplayDialogButton(d,i,j);

	s=0;
	SetColor(WHITE);
	for (j=0;j<d->NC;j++)
		for (i=0;i<d->NR;i++)
		{
			if (d->btns[d->NR*j+i].idx==0 &&
				  (i<d->NR-1 || j<d->NC-1))
			{
				int offs = (d->BW-8*strlen(d->sections[s]))/2;
				DrawScreenText
				(
					d->X0+d->BX0+d->BXS*j+offs,
					d->Y0+d->BY0+d->BYS*i-10,
					d->sections[s++]
				);
			}
		}

	DrawScreenButton
	(
		d->X0+d->BX0+(d->NC-1)*d->BXS,
		d->Y0+d->BY0+(d->NR-1)*d->BYS,
		d->BW,
		d->BH,
		"Ok",
		1
	);

	ShowMousePointer(); 

  oldbuttons = 0x0000;
  ok = FALSE;
  
	line=0;
	col=0;
  while (!ok)
	{
		if (UseMouse)
		{
			GetMouseCoords( &PointerX, &PointerY, &buttons);
			/* right button = cancel */
			if ((buttons & 0x0002) == 0x0002)
				return 0;
			/* left button = select */
			else if (buttons == 0x0001 && PointerX >= d->X0+d->BX0 && PointerX < d->X0+d->W)
			{
					line = (PointerY - (d->Y0+d->BY0)) / d->BYS;
					col = (PointerX - (d->X0+d->BX0)) / d->BXS;
			}
			/* release left button = accept selection */
			else if (buttons == 0x0000 && oldbuttons == 0x0001)
			{
				x0 = d->X0+d->BX0+col*d->BXS;
				y0 = d->Y0+d->BY0+line*d->BYS;
				if (x0<=PointerX && PointerX<x0+d->BW &&
					  y0<=PointerY && PointerY<y0+d->BH)
				{
					if (line==d->NR-1 && col==d->NC-1)
						ok = TRUE;
					else
					{
						HideMousePointer();
						ChangeDialogButton(d,line,col);
						ShowMousePointer();
					}
				}
			}
			oldbuttons = buttons;
		}
		if (bioskey( 1))
		{
			key = bioskey( 0);
			
			/* enter key = accept selection */
			if ((key & 0x00FF) == 0x000D)
				ok = TRUE;
			/* escape key = cancel */
			else if ((key & 0x00FF) == 0x001B)
				return 0;
		}
	}
	return 1;
}


int DisplayGeneralFloorDialog()
{
	return DoButtonDialog(&FloorDialog);
}

int DisplayGeneralCeilingDialog()
{
	return DoButtonDialog(&CeilingDialog);
}

int DisplayGeneralDoorDialog()
{
	return DoButtonDialog(&DoorDialog);
}

int DisplayGeneralLockedDialog()
{
	return DoButtonDialog(&LockedDialog);
}

int DisplayGeneralLiftDialog()
{
	return DoButtonDialog(&LiftDialog);
}

int DisplayGeneralStairsDialog()
{
	return DoButtonDialog(&StairsDialog);
}

int DisplayGeneralCrusherDialog()
{
	return DoButtonDialog(&CrusherDialog);
}

int DisplayGeneralSectorTypeDialog()
{
	return DoButtonDialog(&SectorDialog);
}
